# Note: keep this file ASCII only.
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, sensor, uart
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_CONNECTIVITY,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_AMPERE,
    UNIT_PERCENT,
    UNIT_SECOND,
    UNIT_VOLT,
    UNIT_WATT,
)

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "binary_sensor"]
CODEOWNERS = ["@ericbarch"]

CONF_PUBLISH_INTERVAL = "publish_interval"
CONF_ONLINE_TIMEOUT = "online_timeout"
CONF_STATE_OF_CHARGE = "state_of_charge"
CONF_VOLTAGE = "voltage"
CONF_CAPACITY = "capacity"
CONF_CURRENT = "current"
CONF_REMAINING_TIME = "remaining_time"
CONF_POWER = "power"
CONF_CHARGING_POWER = "charging_power"
CONF_DISCHARGING_POWER = "discharging_power"
CONF_ONLINE = "online"

tf03k_shunt_ns = cg.esphome_ns.namespace("tf03k_shunt")
TF03KShunt = tf03k_shunt_ns.class_("TF03KShunt", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(TF03KShunt),
            cv.Optional(CONF_PUBLISH_INTERVAL, default="5s"): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_ONLINE_TIMEOUT, default="5s"): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_STATE_OF_CHARGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                device_class=DEVICE_CLASS_BATTERY,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                device_class=DEVICE_CLASS_VOLTAGE,
                accuracy_decimals=2,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CAPACITY): sensor.sensor_schema(
                unit_of_measurement="Ah",
                device_class=DEVICE_CLASS_BATTERY,
                accuracy_decimals=3,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CURRENT): sensor.sensor_schema(
                unit_of_measurement=UNIT_AMPERE,
                device_class=DEVICE_CLASS_CURRENT,
                accuracy_decimals=2,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_REMAINING_TIME): sensor.sensor_schema(
                unit_of_measurement=UNIT_SECOND,
                device_class=DEVICE_CLASS_DURATION,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_POWER): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                device_class=DEVICE_CLASS_POWER,
                accuracy_decimals=1,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CHARGING_POWER): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                device_class=DEVICE_CLASS_POWER,
                accuracy_decimals=1,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_DISCHARGING_POWER): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                device_class=DEVICE_CLASS_POWER,
                accuracy_decimals=1,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_ONLINE): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_CONNECTIVITY
            ),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    cg.add(var.set_publish_interval(config[CONF_PUBLISH_INTERVAL]))
    cg.add(var.set_online_timeout(config[CONF_ONLINE_TIMEOUT]))

    if CONF_STATE_OF_CHARGE in config:
        sens = await sensor.new_sensor(config[CONF_STATE_OF_CHARGE])
        cg.add(var.set_state_of_charge_sensor(sens))
    if CONF_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_VOLTAGE])
        cg.add(var.set_voltage_sensor(sens))
    if CONF_CAPACITY in config:
        sens = await sensor.new_sensor(config[CONF_CAPACITY])
        cg.add(var.set_capacity_sensor(sens))
    if CONF_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_CURRENT])
        cg.add(var.set_current_sensor(sens))
    if CONF_REMAINING_TIME in config:
        sens = await sensor.new_sensor(config[CONF_REMAINING_TIME])
        cg.add(var.set_remaining_time_sensor(sens))
    if CONF_POWER in config:
        sens = await sensor.new_sensor(config[CONF_POWER])
        cg.add(var.set_power_sensor(sens))
    if CONF_CHARGING_POWER in config:
        sens = await sensor.new_sensor(config[CONF_CHARGING_POWER])
        cg.add(var.set_charging_power_sensor(sens))
    if CONF_DISCHARGING_POWER in config:
        sens = await sensor.new_sensor(config[CONF_DISCHARGING_POWER])
        cg.add(var.set_discharging_power_sensor(sens))
    if CONF_ONLINE in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_ONLINE])
        cg.add(var.set_online_binary_sensor(bs))
