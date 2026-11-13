# MeshCore Repeater: ESP32-C3 Super Mini + E22-900M22S

Готовая проверенная прошивка **MeshCore Repeater v1.18.0** для ESP32-C3 Super Mini и радиомодуля **EBYTE E22-900M22S (SX1262, 22 dBm)**.

> Важно: это вариант именно для **E22-900M22S** размером 20 × 14 мм. Он не предназначен для E22-900MM22S с другой распиновкой или E220-900M22S на LLCC68.

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

DIO2 не подключается: RXEN и TXEN управляются отдельными GPIO. TCXO радиомодуля управляется через DIO3 с напряжением 1,8 В — это учтено в прошивке.

## Радиопрофиль

| Параметр | Значение |
|---|---|
| Частота | 869,050 МГц |
| Полоса | 62,5 кГц |
| Spreading Factor | SF8 |
| Coding Rate | CR7 |
| Мощность | 22 dBm |
| Имя репитера | `E22REP` |
| Начальный пароль администратора | `password` |

После первого запуска смените стандартный пароль.

## Питание и монтаж

- Питание радиомодуля: только 3,3 В.
- Источник должен уверенно отдавать не менее 200 мА.
- Возле VCC установите конденсаторы 100 нФ и 47–100 мкФ.
- SPI-провода желательно делать короче 10 см.
- Не включайте передачу без антенны диапазона 868/915 МГц.

## Файлы

- [Полная инструкция и исходная конфигурация](meshcore-esp32c3-e22-900m22s/)
- [PlatformIO-конфигурация](meshcore-esp32c3-e22-900m22s/platformio.ini)
- [Готовая прошивка и скрипт сборки](meshcore-esp32c3-e22-900m22s/firmware/)
- [Изменение FSPI для MeshCore](meshcore-esp32c3-e22-900m22s/target-cpp-change.txt)

## Подготовка готового бинарника

Перейдите в каталог с прошивкой и объедините три части:

```bash
cd meshcore-esp32c3-e22-900m22s/firmware
python3 assemble_firmware.py
```

Скрипт создаст файл:

```text
meshcore_esp32c3_e22-900m22s_repeater_v1.18.0_merged.bin
```

и проверит его SHA-256:

```text
89a147090a6c334ac8b641944f1c5a93869d69fbc8cd0f9bf878365289cec398
```

## Прошивка ESP32-C3

```bash
esptool --chip esp32c3 --port PORT write-flash 0x0 meshcore_esp32c3_e22-900m22s_repeater_v1.18.0_merged.bin
```

После загрузки откройте последовательный порт на скорости 115200 бод. Исправный запуск содержит обнаружение SX126x и `device_errors=0x0000`.

Полезные команды: `get radio`, `get name`, `clock`, `stats-radio`, `stats-packets`, `advert`.

## Сборка из исходников MeshCore

База: MeshCore v1.18.0, commit `81dbae592ffad40b4212bc0b37e82b38da08f4bc`.

```bash
pio run -e supermini_c3_e22_m22s_repeater
pio run -e supermini_c3_e22_m22s_repeater -t mergebin
```
