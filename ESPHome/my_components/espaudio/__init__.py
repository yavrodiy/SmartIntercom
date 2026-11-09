import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.const import CONF_ID

DOMAIN = "espaudio"

# Объявляем пространство имён и класс C++
espaudio_ns = cg.esphome_ns.namespace("espaudio")
ESPAudio = espaudio_ns.class_("ESPAudio", cg.Component)

# Схема конфигурации: компонент не требует обязательных параметров
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ESPAudio),
}).extend(cv.COMPONENT_SCHEMA)

# Функция, которая вызывается при сборке для регистрации компонента
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)

    # ESP8266Audio decoder library
    cg.add_library("ESP8266Audio", "2.4.1")

    # Arduino ESP32 libraries
    cg.add_library("FS", None)
    cg.add_library("LittleFS", None)
    cg.add_library("SPIFFS", None)
    cg.add_library("WiFi", None)
    cg.add_library("NetworkClientSecure", None)
    cg.add_library("HTTPClient", None)
    cg.add_library("SD", None)
    cg.add_library("SPI", None)
