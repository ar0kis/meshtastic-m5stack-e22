#include "UITask.h"
#include <Arduino.h>
#include <helpers/CommonCLI.h>
#include <target.h>

#ifndef USER_BTN_PRESSED
#define USER_BTN_PRESSED LOW
#endif

#ifdef WITH_MQTT_BRIDGE
#include <WiFi.h>
#endif

#define AUTO_OFF_MILLIS 20000   // 20 seconds
#define BOOT_SCREEN_MILLIS 4000 // 4 seconds

// 'meshcore', 128x13px
static const uint8_t meshcore_logo[] PROGMEM = {
    0x3c, 0x01, 0xe3, 0xff, 0xc7, 0xff, 0x8f, 0x03, 0x87, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x3c, 0x03, 0xe3, 0xff, 0xc7,
    0xff, 0x8e, 0x03, 0x8f, 0xfe, 0x3f, 0xfe, 0x1f, 0xff, 0x1f, 0xfe, 0x3e, 0x03, 0xc3, 0xff, 0x8f, 0xff, 0x0e, 0x07, 0x8f, 0xfe,
    0x7f, 0xfe, 0x1f, 0xff, 0x1f, 0xfc, 0x3e, 0x07, 0xc7, 0x80, 0x0e, 0x00, 0x0e, 0x07, 0x9e, 0x00, 0x78, 0x0e, 0x3c, 0x0f, 0x1c,
    0x00, 0x3e, 0x0f, 0xc7, 0x80, 0x1e, 0x00, 0x0e, 0x07, 0x1e, 0x00, 0x70, 0x0e, 0x38, 0x0f, 0x3c, 0x00, 0x7f, 0x0f, 0xc7, 0xfe,
    0x1f, 0xfc, 0x1f, 0xff, 0x1c, 0x00, 0x70, 0x0e, 0x38, 0x0e, 0x3f, 0xf8, 0x7f, 0x1f, 0xc7, 0xfe, 0x0f, 0xff, 0x1f, 0xff, 0x1c,
    0x00, 0xf0, 0x0e, 0x38, 0x0e, 0x3f, 0xf8, 0x7f, 0x3f, 0xc7, 0xfe, 0x0f, 0xff, 0x1f, 0xff, 0x1c, 0x00, 0xf0, 0x1e, 0x3f, 0xfe,
    0x3f, 0xf0, 0x77, 0x3b, 0x87, 0x00, 0x00, 0x07, 0x1c, 0x0f, 0x3c, 0x00, 0xe0, 0x1c, 0x7f, 0xfc, 0x38, 0x00, 0x77, 0xfb, 0x8f,
    0x00, 0x00, 0x07, 0x1c, 0x0f, 0x3c, 0x00, 0xe0, 0x1c, 0x7f, 0xf8, 0x38, 0x00, 0x73, 0xf3, 0x8f, 0xff, 0x0f, 0xff, 0x1c, 0x0e,
    0x3f, 0xf8, 0xff, 0xfc, 0x70, 0x78, 0x7f, 0xf8, 0xe3, 0xe3, 0x8f, 0xff, 0x1f, 0xfe, 0x3c, 0x0e, 0x3f, 0xf8, 0xff, 0xfc, 0x70,
    0x3c, 0x7f, 0xf8, 0xe3, 0xe3, 0x8f, 0xff, 0x1f, 0xfc, 0x3c, 0x0e, 0x1f, 0xf8, 0xff, 0xf8, 0x70, 0x3c, 0x7f, 0xf8,
};

void UITask::begin(NodePrefs *node_prefs, const char *build_date, const char *firmware_version)
{
    _prevBtnState = HIGH;
    _auto_off = millis() + AUTO_OFF_MILLIS;
    _node_prefs = node_prefs;
    _display->turnOn();

    // strip off dash and commit hash by changing dash to null terminator
    // e.g: v1.2.3-abcdef -> v1.2.3
    char *version = strdup(firmware_version);
    char *dash = strchr(version, '-');
    if (dash) {
        *dash = 0;
    }

    // v1.2.3 (1 Jan 2025)
    sprintf(_version_info, "%s (%s)", version, build_date);
}

#ifdef M5STACK_CORE_E22
void UITask::renderHeader(const char *title)
{
    char tmp[20];
    _display->setColor(DisplayDriver::BLUE);
    _display->fillRect(0, 0, 160, 16);
    _display->setTextSize(1);
    _display->setColor(DisplayDriver::LIGHT);
    _display->setCursor(4, 1);
    _display->print(title);

    const int battery = board.getBatteryLevelPercent();
    if (battery >= 0) {
        snprintf(tmp, sizeof(tmp), "%d%%%s", battery, board.isBatteryCharging() ? "+" : "");
        _display->drawTextRightAlign(156, 1, tmp);
    }
}

void UITask::drawValue(int x, int y, const char *value)
{
    _display->setTextSize(2);
    _display->setCursor(x, y);
    _display->print(value);
}

void UITask::drawValueCentered(int middle_x, int y, const char *value)
{
    _display->setTextSize(2);
    const int x = middle_x - _display->getTextWidth(value) / 2;
    drawValue(x, y, value);
}

void UITask::renderEnvironmentScreen()
{
    char tmp[32];
    renderHeader("ПОГОДА  /  ENV II");

    _display->setColor(DisplayDriver::BLUE);
    _display->fillRect(0, 20, 96, 48);
    _display->fillRect(100, 20, 60, 48);
    _display->fillRect(0, 72, 78, 35);
    _display->fillRect(82, 72, 78, 35);

    if (!sensors.isAvailable()) {
        _display->setColor(DisplayDriver::RED);
        _display->setTextSize(1);
        _display->drawTextCentered(80, 45, "ДАТЧИК НЕ НАЙДЕН");
        return;
    }

    _display->setTextSize(0);
    _display->setColor(DisplayDriver::ORANGE);
    _display->setCursor(5, 24);
    _display->print("ТЕМПЕРАТУРА");
    _display->setCursor(105, 24);
    _display->print("ВЛАЖН.");
    _display->setCursor(5, 76);
    _display->print("ДАВЛЕНИЕ");
    _display->setCursor(87, 76);
    _display->print("ТОЧКА РОСЫ");

    _display->setColor(DisplayDriver::LIGHT);
    if (!isnan(sensors.getTemperatureC())) {
        snprintf(tmp, sizeof(tmp), "%.1f°C", sensors.getTemperatureC());
        drawValue(5, 40, tmp);
    }
    if (!isnan(sensors.getHumidityPercent())) {
        snprintf(tmp, sizeof(tmp), "%.0f%%", sensors.getHumidityPercent());
        drawValueCentered(130, 40, tmp);
    }

    _display->setTextSize(1);
    if (!isnan(sensors.getPressureHpa())) {
        snprintf(tmp, sizeof(tmp), "%.0f гПа", sensors.getPressureHpa());
        _display->setCursor(5, 90);
        _display->print(tmp);
    }
    if (!isnan(sensors.getDewPointC())) {
        snprintf(tmp, sizeof(tmp), "%.1f°C", sensors.getDewPointC());
        _display->setCursor(87, 90);
        _display->print(tmp);
    }

    _display->setTextSize(0);
    _display->setColor(DisplayDriver::ORANGE);
    if (!isnan(sensors.getAltitudeM())) {
        snprintf(tmp, sizeof(tmp), "ВЫСОТА %.0f м", sensors.getAltitudeM());
        _display->setCursor(3, 111);
        _display->print(tmp);
    }
    _display->drawTextRightAlign(157, 111, "B: РАДИО  1/2");
}

void UITask::renderRadioScreen()
{
    char tmp[40];
    renderHeader("РАДИО  /  СЕТЬ");

    _display->setColor(DisplayDriver::BLUE);
    _display->fillRect(0, 20, 160, 44);
    _display->fillRect(0, 68, 78, 35);
    _display->fillRect(82, 68, 78, 35);

    _display->setTextSize(0);
    _display->setColor(DisplayDriver::ORANGE);
    _display->setCursor(5, 24);
    _display->print("ЧАСТОТА");
    _display->setCursor(5, 72);
    _display->print("ПРОФИЛЬ LORA");
    _display->setCursor(87, 72);
    _display->print("СЕТЬ / MQTT");

    _display->setColor(DisplayDriver::LIGHT);
    snprintf(tmp, sizeof(tmp), "%.3f МГц", _node_prefs->freq);
    drawValue(5, 39, tmp);

    _display->setTextSize(1);
    snprintf(tmp, sizeof(tmp), "SF%d / %.1f", _node_prefs->sf, _node_prefs->bw);
    _display->setCursor(5, 86);
    _display->print(tmp);

#ifdef WITH_MQTT_BRIDGE
    if (WiFi.status() == WL_CONNECTED) {
        IPAddress ip = WiFi.localIP();
        _display->setColor(DisplayDriver::GREEN);
        snprintf(tmp, sizeof(tmp), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    } else {
        _display->setColor(DisplayDriver::YELLOW);
        snprintf(tmp, sizeof(tmp), "НЕТ WI-FI");
    }
    _display->setTextSize(0);
    _display->setCursor(87, 86);
    _display->print(tmp);
#endif

    _display->setTextSize(0);
    _display->setColor(DisplayDriver::ORANGE);
    snprintf(tmp, sizeof(tmp), "CR 4/%d  TX %d", _node_prefs->cr, _node_prefs->tx_power_dbm);
    _display->setCursor(3, 106);
    _display->print(tmp);
    _display->drawTextRightAlign(157, 106, sensors.isAvailable() ? "ДАТЧИК OK" : "ОШИБКА ENV");
    _display->drawTextRightAlign(157, 113, "B: ПОГОДА  2/2");
}
#endif

void UITask::renderCurrScreen()
{
    char tmp[80];
    if (millis() < BOOT_SCREEN_MILLIS) { // boot screen
        // meshcore logo
        _display->setColor(DisplayDriver::ORANGE);
        int logoWidth = 128;
        _display->drawXbm((_display->width() - logoWidth) / 2, 3, meshcore_logo, logoWidth, 13);

        // meshcore website
        const char *website = "https://meshcore.io";
        _display->setColor(DisplayDriver::LIGHT);
        _display->setTextSize(0);
        uint16_t websiteWidth = _display->getTextWidth(website);
        _display->setCursor((_display->width() - websiteWidth) / 2, 22);
        _display->print(website);

        // version info
        _display->setColor(DisplayDriver::LIGHT);
        _display->setTextSize(0);
        uint16_t versionWidth = _display->getTextWidth(_version_info);
        _display->setCursor((_display->width() - versionWidth) / 2, 35);
        _display->print(_version_info);

        // node type
        _display->setTextSize(1);
        const char *node_type = "< НАБЛЮДАТЕЛЬ >";
        uint16_t typeWidth = _display->getTextWidth(node_type);
        _display->setCursor((_display->width() - typeWidth) / 2, 48);
        _display->print(node_type);
    } else { // home screen
#ifdef M5STACK_CORE_E22
        if (_page == 0)
            renderEnvironmentScreen();
        else
            renderRadioScreen();
#else
        // node name
        _display->setCursor(0, 0);
        _display->setTextSize(1);
        _display->setColor(DisplayDriver::GREEN);
        _display->print(_node_prefs->node_name);

        // freq / sf
        _display->setCursor(0, 20);
        _display->setColor(DisplayDriver::YELLOW);
        sprintf(tmp, "FREQ: %06.3f SF%d", _node_prefs->freq, _node_prefs->sf);
        _display->print(tmp);

        // bw / cr
        _display->setCursor(0, 30);
        sprintf(tmp, "BW: %03.2f CR: %d", _node_prefs->bw, _node_prefs->cr);
        _display->print(tmp);

#ifdef WITH_MQTT_BRIDGE
        // Display IP address for MQTT bridge devices
        if (WiFi.status() == WL_CONNECTED) {
            IPAddress ip = WiFi.localIP();
            _display->setCursor(0, 40);
            _display->setColor(DisplayDriver::LIGHT);
            snprintf(tmp, sizeof(tmp), "IP: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
            _display->print(tmp);
        }
#endif

#endif
    }
}

void UITask::loop()
{
#ifdef PIN_USER_BTN
    if (millis() >= _next_read) {
        int btnState = digitalRead(PIN_USER_BTN);
        if (btnState != _prevBtnState) {
            if (btnState == USER_BTN_PRESSED) { // pressed?
                if (_display->isOn()) {
#ifdef M5STACK_CORE_E22
                    _page = (_page + 1) % 2;
                    _next_refresh = 0;
#endif
                } else {
                    _display->turnOn();
                }
                _auto_off = millis() + AUTO_OFF_MILLIS; // extend auto-off timer
            }
            _prevBtnState = btnState;
        }
        _next_read = millis() + 200; // 5 reads per second
    }
#endif

    if (_display->isOn()) {
        if (millis() >= _next_refresh) {
            _display->startFrame();
            renderCurrScreen();
            _display->endFrame();

            _next_refresh = millis() + 1000; // refresh every second
        }
        if (millis() > _auto_off) {
            _display->turnOff();
        }
    }
}
