# TF03K Smart Shunt (ESPHome)

Expose TF03K coulomb-meter data to Home Assistant through ESPHome running on an ESP32. The ESP listens on the shunt's UART output, decodes frames, and publishes battery metrics (state of charge, voltage, current, power, remaining time, daily charged/discharged energy).

![](images/tf03k.png)

## Hardware Needed
- TF03K coulomb meter with shunt.
- ESP32 dev board (3.3V UART). ESP8266 may work, but the config targets ESP32.
- Shared ground between the shunt and ESP32, plus a data lead from the shunt TX to the chosen ESP32 GPIO.

## Wiring
- Connect the TF03K UART `TX` pin to the ESP32 RX pin defined by `uart_gpio` (default `GPIO17`).
- Tie the TF03K ground to the ESP32 ground.
- Power the TF03K/shunt from the esp32 VCC; power the ESP32 from USB or a regulated 5V/3.3V source.
- Refer to `docs/TF03K communication specification.pdf` for the vendor pinout if your board silkscreen differs.

## ESPHome Configuration
Pick the entry point that fits your workflow:
- `esphome-LP-tf03k-smart-shunt.yaml` (Local Package): uses the `packages/tf03k_shunt.yaml` file that lives in this repo. Choose this if you cloned the project and want to hack locally or build offline.
- `esphome-RP-tf03k-smart-shunt.yaml` (Remote Package): uses ESPHome’s Remote Packages to pull `packages/tf03k_shunt.yaml` directly from GitHub (`main` branch, refresh daily). Choose this if you just want to drop a single YAML into ESPHome (e.g., HA add-on) and let it keep itself updated.

Both variants expose the same substitutions and sensors.

Tunables (via `substitutions` in whichever entry YAML you use):
- `device_name` / `friendly_name`: hostname and display name.
- `uart_gpio`: ESP32 pin connected to TF03K TX.
- `refresh_interval`: publish interval for the exposed sensors (raw values are updated every frame).
- `restore_counters`: whether daily charged/discharged energy counters survive reboots by writing their state to flash (`true`) or reset on every restart (`false`).

Secrets required in `secrets.yaml` (example):
```yaml
wifi_ssid: "YourWiFi"
wifi_password: "YourPassword"
api_encryption_key: "generated-by-esphome"
```
Add OTA/API passwords or other secrets as needed by your environment.

## Build and Upload
1) Install ESPHome (CLI via `pipx install esphome` or the Home Assistant ESPHome add-on).
2) Plug the ESP32 into USB and connect to this repo.
3) Flash from the CLI (replace the filename with `esphome-RP-tf03k-smart-shunt.yaml` if you prefer the Remote Package variant):
```bash
esphome run esphome-LP-tf03k-smart-shunt.yaml --device /dev/ttyUSB0
```
This builds, uploads, and shows logs. After the first flash, you can use OTA:
```bash
esphome upload esphome-LP-tf03k-smart-shunt.yaml
```
To only build the firmware without uploading:
```bash
esphome compile esphome-LP-tf03k-smart-shunt.yaml
```
If you use the Home Assistant add-on, copy the YAML files into your ESPHome folder, adjust substitutions/secrets, and click "Install".

## Flash Wear Warning
`restore_counters: true` persists the daily energy sensors by writing to flash. ESP32 flash endurance is finite, so frequent updates can accelerate wear. Set `restore_counters: false` if you don't need those counters to survive reboots.

## Home Assistant Entities
Published sensors include:
- State of charge (%)
- Voltage (V)
- Battery capacity (Ah)
- Current (A; charging/discharging signed)
- Power (W) plus daily charged/discharged energy (kWh)
- Remaining time (seconds)

## Troubleshooting
- If you see no data, verify the TF03K TX voltage level and wiring (RX pin and ground).
- Keep `uart_gpio` on a hardware UART-capable pin (default `GPIO17` on ESP32).
- Use `esphome logs esphome-LP-tf03k-smart-shunt.yaml` (or the RP file) to watch incoming frames and parser output.***
