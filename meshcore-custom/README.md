# MeshCore для M5Stack Core + E22 и ESP32-C3 Super Mini + E22

Готовый комплект MeshCore для сети с единым радиопрофилем:

- частота: `869.05 MHz`;
- полоса: `62.5 kHz`;
- spreading factor: `SF7`;
- coding rate: `CR 4/7`;
- программная мощность SX1262: `22` (максимальная настройка для E22-900M30S);
- TCXO: `1.8 V`;
- управление усилителем E22: отдельные линии `TXEN` и `RXEN`.

Используйте частоту и мощность только там, где это разрешено местными правилами.

## Готовые прошивки

Все файлы в каталоге [`firmware`](firmware/) являются цельными `merged.bin` и прошиваются с адреса `0x0`.

| Файл                                                              | Назначение                                            | Платформа                 |
| ----------------------------------------------------------------- | ----------------------------------------------------- | ------------------------- |
| `meshcore_m5stack_core_e22_observer_mqtt_tjm_tx22_merged.bin`     | Наблюдатель с MQTT-мостом и дисплеем                  | M5Stack Core ESP32 + E22  |
| `meshcore_m5stack_core_e22_companion_ble_rus_tjm_tx22_merged.bin` | BLE-компаньон, кириллица, заряд штатного аккумулятора | M5Stack Core ESP32 + E22  |
| `meshcore_esp32c3_supermini_e22_repeater_tjm_tx22_merged.bin`     | Репитер `C3 E22 Repeater TJM`                         | ESP32-C3 Super Mini + E22 |

Компаньон использует BLE PIN `123456`. Средняя кнопка `B` на M5Stack включает экран и переключает страницы. Репитер не появляется отдельным BLE-устройством: его объявление принимается по LoRa через подключённый компаньон.

## Подключение M5Stack Core к E22-900M30S

| M5Stack GPIO |       E22 pin | Сигнал   |
| -----------: | ------------: | -------- |
|       GPIO26 |            19 | NSS / CS |
|       GPIO18 |            18 | SCK      |
|       GPIO23 |            17 | MOSI     |
|       GPIO19 |            16 | MISO     |
|       GPIO25 |            15 | NRST     |
|       GPIO35 |            14 | BUSY     |
|       GPIO36 |            13 | DIO1     |
|        GPIO5 |             7 | TXEN     |
|        GPIO2 |             6 | RXEN     |
|          5 V |        9 и 10 | VCC      |
|          GND | 11, 12 или 20 | GND      |

E22 pin 8 (`DIO2`) не подключается. Антенна подключается к RF-площадке `ANT` (pin 21) с соседней землёй. Не включайте передачу без согласованной антенны.

## Подключение ESP32-C3 Super Mini к E22-900M30S

| ESP32-C3 |       E22 pin | Сигнал   |
| -------: | ------------: | -------- |
|   GPIO10 |            19 | NSS / CS |
|    GPIO4 |            18 | SCK      |
|    GPIO6 |            17 | MOSI     |
|    GPIO5 |            16 | MISO     |
|    GPIO0 |            15 | NRST     |
|    GPIO1 |            14 | BUSY     |
|    GPIO3 |            13 | DIO1     |
|   GPIO20 |             7 | TXEN     |
|   GPIO21 |             6 | RXEN     |
|       5V |        9 и 10 | VCC      |
|      GND | 11, 12 или 20 | GND      |

Прошивка использует контроллер `FSPI`. Стандартный `HSPI` на ESP32-C3 недоступен и приводит к ошибке `radio init failed: -2`.

## Питание E22

E22-900M30S при передаче требует отдельного устойчивого питания `5 V` с запасом не менее `1 A`. Контакт `3.3V` микроконтроллера для питания E22 не используйте.

Рекомендуется:

- общий источник 5 V для платы и E22 либо отдельная линия 5 V с общей землёй;
- электролитический конденсатор `470-1000 µF` возле E22;
- керамические конденсаторы `10-47 µF` и `0.1 µF` возле VCC/GND модуля;
- короткие толстые провода питания;
- не подавать внешние 5 V одновременно с USB, если схема питания не защищена от обратного тока.

Логические входы E22 работают с уровнями `3.3 V`; преобразователи уровней на SPI не требуются.

## Антенна 868 MHz

В каталог [`hardware/antenna`](hardware/antenna/) добавлен исходный архив
[`868_MHz_6el_Yagi_Thingiverse_5437281.zip`](hardware/antenna/868_MHz_6el_Yagi_Thingiverse_5437281.zip)
с моделями шестиэлементной Yagi-антенны. Внутри находятся семь STL-файлов,
изображения, исходный README и лицензия Creative Commons Attribution.

Автор модели - `rk3att`, исходная публикация:
[Thingiverse #5437281](https://www.thingiverse.com/thing:5437281). По описанию автора,
элементы выполняются из латунной трубки диаметром 2 мм, а комплект включает
крышку с креплением на трубу диаметром 20 мм.

Перед передачей проверьте сборку анализатором или КСВ-метром на рабочей частоте.
Заявленное автором значение КСВ `1.1-1.2` не гарантируется без проверки конкретно
изготовленной антенны. Контрольная сумма архива записана в
[`hardware/antenna/SHA256SUMS`](hardware/antenna/SHA256SUMS).

## Прошивка готового merged.bin

### 1. Установка esptool

macOS/Linux:

```bash
python3 -m venv .venv-flash
source .venv-flash/bin/activate
python -m pip install "esptool>=4.5,<5"
```

Windows PowerShell:

```powershell
py -m venv .venv-flash
.\.venv-flash\Scripts\Activate.ps1
python -m pip install "esptool>=4.5,<5"
```

### 2. Поиск USB-порта

macOS:

```bash
ls /dev/cu.usb*
```

В Windows посмотрите номер `COM` в диспетчере устройств.

### 3. Запись M5Stack Core

```bash
python -m esptool --chip esp32 --port /dev/cu.usbserial-XXXX --baud 460800 write_flash 0x0 firmware/meshcore_m5stack_core_e22_companion_ble_rus_tjm_tx22_merged.bin
```

Для наблюдателя замените имя файла на:

```text
firmware/meshcore_m5stack_core_e22_observer_mqtt_tjm_tx22_merged.bin
```

В Windows вместо `/dev/cu.usbserial-XXXX` укажите, например, `COM5`. Обычно M5Stack автоматически входит в загрузчик. Если соединение не начинается, нажмите RESET и повторите команду.

### 4. Запись ESP32-C3 Super Mini

```bash
python -m esptool --chip esp32c3 --port /dev/cu.usbmodemXXXX --baud 460800 write_flash 0x0 firmware/meshcore_esp32c3_supermini_e22_repeater_tjm_tx22_merged.bin
```

Если C3 не входит в загрузчик:

1. удерживайте `BOOT`;
2. кратко нажмите `RESET`;
3. отпустите `RESET`, затем `BOOT`;
4. повторите запись.

Команда `erase_flash` здесь не нужна. Она стирает настройки и идентификатор ноды, поэтому используйте её только для намеренной чистой установки.

## Сборка из исходников

Overlay рассчитан на ветку `mqtt-bridge-implementation` репозитория MeshCore и базовый commit `460d254`.

```bash
git clone --branch mqtt-bridge-implementation https://github.com/agessaman/MeshCore.git
cd MeshCore
git checkout 460d254
cp -R /path/to/meshtastic-m5stack-e22/meshcore-custom/source-overlay/. .
pio run -e m5stack_core_e22_observer_mqtt
pio run -e m5stack_core_e22_companion_ble_rus
pio run -e supermini_c3_e22_repeater_tjm
```

PlatformIO берёт пользовательские окружения из `platformio.local.ini`. Исходный overlay содержит:

- аппаратный вариант M5Stack Core + E22 и драйвер ILI9341 с кириллицей;
- русский интерфейс компаньона и отображение заряда через штатный IP5306;
- однократное применение радиопрофиля после установки компаньона;
- исправление выбора `FSPI` для ESP32-C3;
- окружения наблюдателя, компаньона и репитера.

## Контрольные суммы SHA-256

```text
88800a3536b9bee6aced46faa2770012bf834f3ce5688072c292a60274d1d893  meshcore_esp32c3_supermini_e22_repeater_tjm_tx22_merged.bin
2ded5ed8244cbab7e78393c1cd37e1782574d3b56c5f2f96421739c3c85ff395  meshcore_m5stack_core_e22_companion_ble_rus_tjm_tx22_merged.bin
0fbab0eaa93cd5bd100abe2fef7dc1861faaf341b21d65f27487237ca9661750  meshcore_m5stack_core_e22_observer_mqtt_tjm_tx22_merged.bin
```
