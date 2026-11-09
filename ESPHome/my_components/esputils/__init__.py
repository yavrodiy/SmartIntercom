import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.const import CONF_ID

DEPENDENCIES = ["wifi"]

CONF_USERNAME = "username"
CONF_PASSWORD = "password"

esputils_ns = cg.esphome_ns.namespace("esputils")
ESPUtils = esputils_ns.class_("ESPUtils", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ESPUtils),
    cv.Optional(CONF_USERNAME, default=""): cv.string,
    cv.Optional(CONF_PASSWORD, default=""): cv.string,
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_USERNAME],
        config[CONF_PASSWORD],
    )

    await cg.register_component(var, config)

    # FTPClientServer
    cg.add_library(
        "Improv",
        None,
        "https://github.com/SCratORS/FTPClientServer"
    )

    # ESP32 Arduino filesystem and network
    cg.add_library("FS", None)
    cg.add_library("LittleFS", None)
    cg.add_library("WiFi", None)
