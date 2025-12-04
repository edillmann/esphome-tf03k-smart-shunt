#pragma once

#include <array>

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace tf03k_shunt {

static const char *const TAG = "tf03k_shunt";

class TF03KShunt : public Component, public uart::UARTDevice {
 public:
   TF03KShunt() = default;
  void set_publish_interval(uint32_t publish_interval_ms) { publish_interval_ms_ = publish_interval_ms; }
  void set_online_timeout(uint32_t online_timeout_ms) { online_timeout_ms_ = online_timeout_ms; }

  void set_state_of_charge_sensor(sensor::Sensor *sensor) { state_of_charge_sensor_ = sensor; }
  void set_voltage_sensor(sensor::Sensor *sensor) { voltage_sensor_ = sensor; }
  void set_capacity_sensor(sensor::Sensor *sensor) { capacity_sensor_ = sensor; }
  void set_current_sensor(sensor::Sensor *sensor) { current_sensor_ = sensor; }
  void set_remaining_time_sensor(sensor::Sensor *sensor) { remaining_time_sensor_ = sensor; }
  void set_power_sensor(sensor::Sensor *sensor) { power_sensor_ = sensor; }
  void set_charging_power_sensor(sensor::Sensor *sensor) { charging_power_sensor_ = sensor; }
  void set_discharging_power_sensor(sensor::Sensor *sensor) { discharging_power_sensor_ = sensor; }
  void set_online_binary_sensor(binary_sensor::BinarySensor *binary_sensor) { online_binary_sensor_ = binary_sensor; }

  void dump_config() override;
  void loop() override;

 protected:
  void handle_byte_(uint8_t byte);
  void process_frame_();
  void publish_values_();
  void update_online_state_(uint32_t now);
  void reset_frame_();

  std::array<uint8_t, 16> frame_{};
  uint8_t frame_index_{0};
  uint32_t last_frame_ms_{0};
  uint32_t last_publish_ms_{0};
  uint32_t publish_interval_ms_{5000};
  uint32_t online_timeout_ms_{5000};
  bool has_values_{false};
  bool online_state_{false};
  bool online_initialized_{false};

  float state_of_charge_{NAN};
  float voltage_{NAN};
  float capacity_{NAN};
  float current_{NAN};
  float remaining_time_{NAN};
  float power_{NAN};

  sensor::Sensor *state_of_charge_sensor_{nullptr};
  sensor::Sensor *voltage_sensor_{nullptr};
  sensor::Sensor *capacity_sensor_{nullptr};
  sensor::Sensor *current_sensor_{nullptr};
  sensor::Sensor *remaining_time_sensor_{nullptr};
  sensor::Sensor *power_sensor_{nullptr};
  sensor::Sensor *charging_power_sensor_{nullptr};
  sensor::Sensor *discharging_power_sensor_{nullptr};
  binary_sensor::BinarySensor *online_binary_sensor_{nullptr};
};

}  // namespace tf03k_shunt
}  // namespace esphome
