#!/usr/bin/env python3
"""Static contract for the ESP32-C3 Super Mini E22-900M22S observer."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CONFIG = ROOT / "variants" / "supermini_c3_e22_m22s" / "platformio.ini"


def require(text: str, fragment: str) -> None:
    assert fragment in text, f"missing required configuration: {fragment}"


def main() -> None:
    assert CONFIG.exists(), f"variant config does not exist: {CONFIG}"
    text = CONFIG.read_text(encoding="utf-8")

    required = (
        "[env:supermini_c3_e22_m22s_repeater_observer_mqtt]",
        "board = esp32-c3-devkitm-1",
        "board_build.flash_size = 4MB",
        "board_build.partitions = min_spiffs.csv",
        "-D P_LORA_SCLK=0",
        "-D P_LORA_MISO=3",
        "-D P_LORA_MOSI=2",
        "-D P_LORA_NSS=1",
        "-D P_LORA_BUSY=4",
        "-D P_LORA_DIO_1=21",
        "-D P_LORA_RESET=10",
        "-D SX126X_RXEN=6",
        "-D SX126X_TXEN=7",
        "-D SX126X_DIO3_TCXO_VOLTAGE=1.8",
        "-D LORA_FREQ=869.05",
        "-D LORA_BW=62.5",
        "-D LORA_SF=8",
        "-D LORA_CR=7",
        "-D LORA_TX_POWER=22",
        "-D WITH_MQTT_BRIDGE=1",
        "-D MQTT_NEIGHBORS_WITHOUT_PSRAM=1",
        "-D ADVERT_NAME='\"SNodeREP-1\"'",
        "+<helpers/bridges/MQTTBridge.cpp>",
        "+<../examples/simple_repeater>",
        "bblanchon/ArduinoJson @ 7.4.3",
    )
    for fragment in required:
        require(text, fragment)

    forbidden = ("meshcoretel", "DISPLAY_CLASS", "WIFI_SSID", "WIFI_PWD", "MQTT_SERVER")
    lowered = text.lower()
    for fragment in forbidden:
        assert fragment.lower() not in lowered, f"forbidden baked-in setting: {fragment}"

    print("supermini_c3_e22_m22s observer contract: OK")


if __name__ == "__main__":
    main()
