#pragma once

#include <helpers/ESP32Board.h>

class M5StackE22Board : public ESP32Board
{
    int readIP5306Register(uint8_t reg)
    {
        Wire.beginTransmission(0x75);
        Wire.write(reg);
        if (Wire.endTransmission(false) != 0 || Wire.requestFrom(0x75, 1) != 1) {
            return -1;
        }
        return Wire.read();
    }

  public:
    void begin()
    {
        ESP32Board::begin();

        // M5Stack Core Basic IP5306: keep the boost converter enabled when the
        // board is running from its internal battery.
        Wire.beginTransmission(0x75);
        Wire.write(0x00);
        if (Wire.endTransmission(false) == 0 && Wire.requestFrom(0x75, 1) == 1) {
            const uint8_t sys_ctl0 = Wire.read();
            Wire.beginTransmission(0x75);
            Wire.write(0x00);
            Wire.write(sys_ctl0 | 0x04);
            Wire.endTransmission();
        }
    }

    int8_t getBatteryLevelPercent()
    {
        const int value = readIP5306Register(0x78);
        if (value < 0)
            return -1;

        // IP5306 exposes a coarse state-of-charge gauge, not a voltage ADC.
        switch (value & 0xF0) {
        case 0x00:
            return 100;
        case 0x80:
            return 75;
        case 0xC0:
            return 50;
        case 0xE0:
            return 25;
        default:
            return 0;
        }
    }

    bool isBatteryCharging()
    {
        const int value = readIP5306Register(0x70);
        return value >= 0 && (value & 0x08) != 0;
    }

    uint16_t getBattMilliVolts() override
    {
        // IP5306 only reports charge bands. Return the midpoint of the official
        // M5Stack discharge range so MeshCore telemetry no longer reports 0 V.
        switch (getBatteryLevelPercent()) {
        case 100:
            return 4135;
        case 75:
            return 3940;
        case 50:
            return 3680;
        case 25:
            return 3440;
        case 0:
            return 3200;
        default:
            return 0;
        }
    }

    const char *getManufacturerName() const override { return "M5Stack Core Basic + E22"; }
};
