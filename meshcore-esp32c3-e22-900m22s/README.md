# MeshCore Repeater: ESP32-C3 Super Mini + E22-900M22S

Готовый вариант MeshCore Repeater v1.18.0 для ESP32-C3 Super Mini и радиомодуля EBYTE E22-900M22S на SX1262.

> Это именно **E22-900M22S** (20 × 14 мм, SX1262), не E22-900MM22S и не E220-900M22S на LLCC68.

## Схема подключения

| ESP32-C3 Super Mini | E22-900M22S | Контакт E22 | Направление |
|---|---|---:|---|
| GPIO0 | SCK | 18 | ESP → E22 |
| GPIO3 | MISO | 16 | ESP ← E22 |
| GPIO2 | MOSI | 17 | ESP → E22 |
| GPIO1 | NSS | 19 | ESP → E22 |
| GPIO4 | BUSY | 14 | ESP ← E22 |
| GPIO21 | DIO1 | 13 | ESP ← E22 |
| GPIO10 | NRST | 15 | ESP → E22 |
| GPIO6 | RXEN | 6 | ESP → E22 |
| GPIO7 | TXEN | 7 | ESP → E22 |
| 3V3 | VCC | 9 | питание |
| GND | GND | 1–5, 10–12, 20 | земля |

```text
ESP32-C3 Super Mini                       E22-900M22S

3V3  ---------------------------------->  VCC   pin 9
GND  ---------------------------------->  GND   pins 1-5, 10-12, 20

GPIO0  -------------------------------->  SCK   pin 18
GPIO3  <--------------------------------  MISO  pin 16
GPIO2  -------------------------------->  MOSI  pin 17
GPIO1  -------------------------------->  NSS   pin 19
GPIO4  <--------------------------------  BUSY  pin 14
GPIO21 <--------------------------------  DIO1  pin 13
GPIO10 -------------------------------->  NRST  pin 15
GPIO6  -------------------------------->  RXEN  pin 6
GPIO7  -------------------------------->  TXEN  pin 7
```

DIO2 не подключается: RXEN и TXEN управляются отдельными GPIO. TCXO модуля управляется через DIO3 с напряжением 1,8 В — это учтено в конфигурации.

## Радиопрофиль

- 869,050 МГц
- BW 62,5 кГц
- SF8
- CR7
- TX 22 dBm
- имя: `E22REP`
- пароль администратора по умолчанию: `password` (смените после первого запуска)

## Питание

Подавайте только 3,3 В от источника не менее 200 мА. Возле VCC установите 100 нФ и 47–100 мкФ. SPI-провода желательно короче 10 см. Не передавайте без антенны 868/915 МГц.

## Как применить к MeshCore

Исходная база: MeshCore v1.18.0, commit `81dbae592ffad40b4212bc0b37e82b38da08f4bc`.

1. Скопируйте `platformio.ini` в `variants/supermini_c3_e22_m22s/platformio.ini`.
2. В `variants/xiao_c3/target.cpp` замените `static SPIClass spi;` на `static SPIClass spi(FSPI);`.
3. Выполните:

```bash
pio run -e supermini_c3_e22_m22s_repeater
pio run -e supermini_c3_e22_m22s_repeater -t mergebin
```

## Прошивка

```bash
esptool --chip esp32c3 --port PORT write-flash 0x0 firmware/meshcore_esp32c3_e22-900m22s_repeater_v1.18.0_merged.bin
```

SHA-256 бинарника:

```text
89a147090a6c334ac8b641944f1c5a93869d69fbc8cd0f9bf878365289cec398
```

После запуска откройте порт на 115200 бод. Нормальный журнал содержит обнаружение SX126x и `device_errors=0x0000`. Полезные команды: `get radio`, `get name`, `clock`, `stats-radio`, `stats-packets`, `advert`.
