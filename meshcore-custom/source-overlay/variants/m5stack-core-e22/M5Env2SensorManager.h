#pragma once

#include <Adafruit_BMP280.h>
#include <Adafruit_SHT31.h>
#include <Arduino.h>
#include <helpers/SensorManager.h>

class M5Env2SensorManager : public SensorManager
{
    static constexpr uint8_t kSht30Address = 0x44;
    static constexpr uint8_t kBmp280Address = 0x76;
    static constexpr uint32_t kSampleIntervalMillis = 2000;
    static constexpr float kSeaLevelPressureHpa = 1013.25f;

    Adafruit_SHT31 sht30;
    Adafruit_BMP280 bmp280;
    bool sht30_available = false;
    bool bmp280_available = false;
    bool sample_valid = false;
    uint32_t last_sample_millis = 0;
    float temperature_c = NAN;
    float humidity_percent = NAN;
    float pressure_hpa = NAN;
    float altitude_m = NAN;
    float dew_point_c = NAN;

    void sample(bool force = false);

  public:
    M5Env2SensorManager() : bmp280(&Wire) {}

    bool begin() override;
    bool querySensors(uint8_t requester_permissions, CayenneLPP &telemetry) override;
    void loop() override;

    bool isAvailable() const { return sht30_available || bmp280_available; }
    bool hasValidSample() const { return sample_valid; }
    bool hasSht30() const { return sht30_available; }
    bool hasBmp280() const { return bmp280_available; }
    float getTemperatureC() const { return temperature_c; }
    float getHumidityPercent() const { return humidity_percent; }
    float getPressureHpa() const { return pressure_hpa; }
    float getAltitudeM() const { return altitude_m; }
    float getDewPointC() const { return dew_point_c; }
};
