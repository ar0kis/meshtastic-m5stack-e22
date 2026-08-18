#include "M5Env2SensorManager.h"

#include <Arduino.h>
#include <cmath>

bool M5Env2SensorManager::begin()
{
    sht30_available = sht30.begin(kSht30Address);
    if (sht30_available) {
        sht30.heater(false);
        MESH_DEBUG_PRINTLN("ENV II: SHT30 found at I2C address 0x44");
    } else {
        MESH_DEBUG_PRINTLN("ENV II: SHT30 not found at I2C address 0x44");
    }

    bmp280_available = bmp280.begin(kBmp280Address);
    if (bmp280_available) {
        bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2, Adafruit_BMP280::SAMPLING_X16,
                           Adafruit_BMP280::FILTER_X16, Adafruit_BMP280::STANDBY_MS_500);
        MESH_DEBUG_PRINTLN("ENV II: BMP280 found at I2C address 0x76");
    } else {
        MESH_DEBUG_PRINTLN("ENV II: BMP280 not found at I2C address 0x76");
    }

    sample(true);
    return isAvailable();
}

void M5Env2SensorManager::sample(bool force)
{
    const uint32_t now = millis();
    if (!force && static_cast<uint32_t>(now - last_sample_millis) < kSampleIntervalMillis)
        return;

    last_sample_millis = now;

    if (sht30_available) {
        const float measured_temperature = sht30.readTemperature();
        const float measured_humidity = sht30.readHumidity();
        if (!std::isnan(measured_temperature))
            temperature_c = measured_temperature;
        if (!std::isnan(measured_humidity))
            humidity_percent = measured_humidity;
    }

    if (bmp280_available) {
        const float measured_pressure = bmp280.readPressure() / 100.0f;
        if (!std::isnan(measured_pressure) && measured_pressure > 0.0f) {
            pressure_hpa = measured_pressure;
            altitude_m = bmp280.readAltitude(kSeaLevelPressureHpa);
        }
        if (!sht30_available) {
            const float measured_temperature = bmp280.readTemperature();
            if (!std::isnan(measured_temperature))
                temperature_c = measured_temperature;
        }
    }

    if (!std::isnan(temperature_c) && !std::isnan(humidity_percent) && humidity_percent > 0.0f) {
        constexpr float a = 17.62f;
        constexpr float b = 243.12f;
        const float gamma = logf(humidity_percent / 100.0f) + (a * temperature_c) / (b + temperature_c);
        dew_point_c = (b * gamma) / (a - gamma);
    }

    sample_valid = !std::isnan(temperature_c) || !std::isnan(humidity_percent) || !std::isnan(pressure_hpa);
}

bool M5Env2SensorManager::querySensors(uint8_t requester_permissions, CayenneLPP &telemetry)
{
    if (!(requester_permissions & TELEM_PERM_ENVIRONMENT))
        return true;

    sample(true);
    constexpr uint8_t channel = TELEM_CHANNEL_SELF + 1;
    if (!std::isnan(temperature_c))
        telemetry.addTemperature(channel, temperature_c);
    if (!std::isnan(humidity_percent))
        telemetry.addRelativeHumidity(channel, humidity_percent);
    if (!std::isnan(pressure_hpa))
        telemetry.addBarometricPressure(channel, pressure_hpa);
    if (!std::isnan(altitude_m))
        telemetry.addAltitude(channel, altitude_m);
    return true;
}

void M5Env2SensorManager::loop()
{
    sample();
}
