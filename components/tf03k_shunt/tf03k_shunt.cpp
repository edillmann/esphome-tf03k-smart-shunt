#include "tf03k_shunt.h"

namespace esphome {
namespace tf03k_shunt {

void TF03KShunt::dump_config() {
  ESP_LOGCONFIG(TAG, "TF03K Shunt");
  this->check_uart_settings(9600, 8, uart::UART_CONFIG_PARITY_NONE, 1);
  ESP_LOGCONFIG(TAG, "  Publish interval: %ums", publish_interval_ms_);
  ESP_LOGCONFIG(TAG, "  Online timeout: %ums", online_timeout_ms_);
  LOG_SENSOR("  ", "State of charge", state_of_charge_sensor_);
  LOG_SENSOR("  ", "Voltage", voltage_sensor_);
  LOG_SENSOR("  ", "Battery capacity", capacity_sensor_);
  LOG_SENSOR("  ", "Current", current_sensor_);
  LOG_SENSOR("  ", "Remaining time", remaining_time_sensor_);
  LOG_SENSOR("  ", "Power", power_sensor_);
  LOG_SENSOR("  ", "Charging power", charging_power_sensor_);
  LOG_SENSOR("  ", "Discharging power", discharging_power_sensor_);
  LOG_BINARY_SENSOR("  ", "Online", online_binary_sensor_);
}

void TF03KShunt::loop() {
  while (this->available()) {
    uint8_t byte;
    if (!this->read_byte(&byte))
      break;
    this->handle_byte_(byte);
  }

  const uint32_t now = millis();
  if (this->has_values_) {
    if (this->last_publish_ms_ == 0 || now - this->last_publish_ms_ >= this->publish_interval_ms_) {
      this->publish_values_();
      this->last_publish_ms_ = now;
    }
  }
  this->update_online_state_(now);
}

void TF03KShunt::handle_byte_(uint8_t byte) {
  if (frame_index_ == 0) {
    if (byte != 0xA5)
      return;
    frame_[frame_index_++] = byte;
    return;
  }

  frame_[frame_index_++] = byte;

  if (frame_index_ < frame_.size())
    return;

  this->process_frame_();
  this->reset_frame_();
}

void TF03KShunt::process_frame_() {
  uint8_t checksum = 0;
  for (size_t i = 0; i < frame_.size() - 1; i++) {
    checksum += frame_[i];
  }
  if (checksum != frame_[15]) {
    ESP_LOGW(TAG, "Dropping frame with bad checksum %02X (expected %02X)", frame_[15], checksum);
    return;
  }

  state_of_charge_ = frame_[1];
  const uint16_t voltage_cV = (static_cast<uint16_t>(frame_[2]) << 8) | frame_[3];
  voltage_ = voltage_cV / 100.0f;

  const uint32_t capacity_mah = (static_cast<uint32_t>(frame_[4]) << 24) | (static_cast<uint32_t>(frame_[5]) << 16) |
                                (static_cast<uint32_t>(frame_[6]) << 8) | frame_[7];
  capacity_ = capacity_mah / 1000.0f;

  const int32_t current_ma = static_cast<int32_t>((static_cast<uint32_t>(frame_[8]) << 24) |
                                                  (static_cast<uint32_t>(frame_[9]) << 16) |
                                                  (static_cast<uint32_t>(frame_[10]) << 8) | frame_[11]);
  current_ = current_ma / 1000.0f;

  const uint32_t remaining_seconds = (static_cast<uint32_t>(frame_[12]) << 16) |
                                     (static_cast<uint32_t>(frame_[13]) << 8) | frame_[14];
  remaining_time_ = remaining_seconds;

  power_ = voltage_ * current_;

  this->last_frame_ms_ = millis();
  this->has_values_ = true;
  ESP_LOGV(TAG,
           "Parsed frame: soc=%.0f%% voltage=%.2fV capacity=%.3fAh current=%.3fA remaining=%.0fs power=%.3fW "
           "(checksum ok)",
           state_of_charge_, voltage_, capacity_, current_, remaining_time_, power_);
}

void TF03KShunt::publish_values_() {
  const float charging_power = this->power_ > 0 ? this->power_ : 0.0f;
  const float discharging_power = this->power_ < 0 ? -this->power_ : 0.0f;

  if (state_of_charge_sensor_ != nullptr)
    state_of_charge_sensor_->publish_state(this->state_of_charge_);
  if (voltage_sensor_ != nullptr)
    voltage_sensor_->publish_state(this->voltage_);
  if (capacity_sensor_ != nullptr)
    capacity_sensor_->publish_state(this->capacity_);
  if (current_sensor_ != nullptr)
    current_sensor_->publish_state(this->current_);
  if (remaining_time_sensor_ != nullptr)
    remaining_time_sensor_->publish_state(this->remaining_time_);
  if (power_sensor_ != nullptr)
    power_sensor_->publish_state(this->power_);
  if (charging_power_sensor_ != nullptr)
    charging_power_sensor_->publish_state(charging_power);
  if (discharging_power_sensor_ != nullptr)
    discharging_power_sensor_->publish_state(discharging_power);
}

void TF03KShunt::update_online_state_(uint32_t now) {
  if (online_binary_sensor_ == nullptr)
    return;

  const bool online = this->last_frame_ms_ > 0 && now - this->last_frame_ms_ <= this->online_timeout_ms_;
  if (!this->online_initialized_ || online != this->online_state_) {
    this->online_state_ = online;
    this->online_initialized_ = true;
    this->online_binary_sensor_->publish_state(online);
  }
}

void TF03KShunt::reset_frame_() {
  frame_index_ = 0;
}

}  // namespace tf03k_shunt
}  // namespace esphome
