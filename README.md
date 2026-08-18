<div align="center" markdown="1">

<img src=".github/meshtastic_logo.png" alt="Meshtastic Logo" width="80"/>
<h1>Meshtastic Firmware</h1>

![GitHub release downloads](https://img.shields.io/github/downloads/meshtastic/firmware/total)
[![CI](https://img.shields.io/github/actions/workflow/status/meshtastic/firmware/main_matrix.yml?branch=master&label=actions&logo=github&color=yellow)](https://github.com/meshtastic/firmware/actions/workflows/ci.yml)
[![CLA assistant](https://cla-assistant.io/readme/badge/meshtastic/firmware)](https://cla-assistant.io/meshtastic/firmware)
[![Fiscal Contributors](https://opencollective.com/meshtastic/tiers/badge.svg?label=Fiscal%20Contributors&color=deeppink)](https://opencollective.com/meshtastic/)
[![Vercel](https://img.shields.io/static/v1?label=Powered%20by&message=Vercel&style=flat&logo=vercel&color=000000)](https://vercel.com?utm_source=meshtastic&utm_campaign=oss)

<a href="https://trendshift.io/repositories/5524" target="_blank"><img src="https://trendshift.io/api/badge/repositories/5524" alt="meshtastic%2Ffirmware | Trendshift" style="width: 250px; height: 55px;" width="250" height="55"/></a>

</div>

</div>

<div align="center">
	<a href="https://meshtastic.org">Website</a>
	-
	<a href="https://meshtastic.org/docs/">Documentation</a>
</div>

## Overview

This repository contains the official device firmware for Meshtastic, an open-source LoRa mesh networking project designed for long-range, low-power communication without relying on internet or cellular infrastructure. The firmware supports various hardware platforms, including ESP32, nRF52, RP2040/RP2350, and Linux-based devices.

Meshtastic enables text messaging, location sharing, and telemetry over a decentralized mesh network, making it ideal for outdoor adventures, emergency preparedness, and remote operations.

### Get Started

- 🔧 **[Building Instructions](https://meshtastic.org/docs/development/firmware/build)** - Learn how to compile the firmware from source.
- ⚡ **[Flashing Instructions](https://meshtastic.org/docs/getting-started/flashing-firmware/)** - Install or update the firmware on your device.

Join our community and help improve Meshtastic! 🚀

## Custom MeshCore firmware

Ниже приведена русская инструкция по сборке и прошивке устройств MeshCore на
базе E22-900M30S. Готовые бинарные файлы, исходный overlay и расширенное описание
находятся в каталоге [`meshcore-custom`](meshcore-custom/README.md).

### Варианты устройств

| Плата               | Роль прошивки                          | Файл                                                                                                                                                          |
| ------------------- | -------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| M5Stack Core ESP32  | BLE-компаньон с кириллицей             | [`meshcore_m5stack_core_e22_companion_ble_rus_tjm_tx22_merged.bin`](meshcore-custom/firmware/meshcore_m5stack_core_e22_companion_ble_rus_tjm_tx22_merged.bin) |
| M5Stack Core ESP32  | Наблюдатель MQTT, ENV II, погодный бот | [`meshcore_m5stack_core_e22_observer_mqtt_tjm_tx22_merged.bin`](meshcore-custom/firmware/meshcore_m5stack_core_e22_observer_mqtt_tjm_tx22_merged.bin)         |
| ESP32-C3 Super Mini | Репитер                                | [`meshcore_esp32c3_supermini_e22_repeater_tjm_tx22_merged.bin`](meshcore-custom/firmware/meshcore_esp32c3_supermini_e22_repeater_tjm_tx22_merged.bin)         |

Все три прошивки используют один радиопрофиль: `869.05 MHz`, `BW 62.5 kHz`,
`SF7`, `CR 4/7`, мощность SX1262 `22` и TCXO `1.8 V`. Перед использованием
проверьте допустимые частоты и мощность для своего региона.

### Что потребуется

- M5Stack Core ESP32 или ESP32-C3 Super Mini;
- радиомодуль E22-900M30S на SX1262;
- согласованная антенна диапазона 868-869 MHz;
- источник 5 V с запасом тока не менее 1 A;
- электролитический конденсатор `470-1000 µF`;
- керамические конденсаторы `10-47 µF` и `0.1 µF`;
- короткие провода, паяльник и мультиметр;
- USB-кабель с линиями данных.

Не включайте E22 без подключённой антенны. Радиомодуль нельзя питать от контакта
`3.3V` микроконтроллера.

### Подключение M5Stack Core к E22-900M30S

| M5Stack Core |   Контакт E22 | Назначение |
| -----------: | ------------: | ---------- |
|       GPIO26 |            19 | NSS / CS   |
|       GPIO18 |            18 | SCK        |
|       GPIO23 |            17 | MOSI       |
|       GPIO19 |            16 | MISO       |
|       GPIO25 |            15 | NRST       |
|       GPIO35 |            14 | BUSY       |
|       GPIO36 |            13 | DIO1       |
|        GPIO5 |             7 | TXEN       |
|        GPIO2 |             6 | RXEN       |
|          5 V |        9 и 10 | Питание    |
|          GND | 11, 12 или 20 | Земля      |

Контакт E22 pin 8 (`DIO2`) не подключается. GPIO логики работают с уровнем
`3.3 V`, преобразователь уровней для SPI не требуется.

### Подключение M5Stack ENV II к наблюдателю

ENV II подключается штатным Grove-кабелем в разъём M5Stack `Port A (I2C)`.

| Цвет провода | M5Stack | ENV II | Назначение |
| ------------ | ------- | ------ | ---------- |
| чёрный       | GND     | GND    | земля      |
| красный      | 5 V     | 5V     | питание    |
| жёлтый       | GPIO21  | SDA    | данные I2C |
| белый        | GPIO22  | SCL    | такт I2C   |

Подключайте датчик при выключенном питании и затем перезагрузите наблюдатель.
Прошивка автоматически ищет SHT30 по адресу `0x44` и BMP280 по адресу `0x76`.
На экране отображаются температура, влажность, давление, точка росы и расчётная
барометрическая высота. Эти же исходные показания доступны по удалённому запросу
телеметрии MeshCore; MQTT status содержит объект `environment`.

### Наблюдатель MQTT и погодный бот

Прошивка наблюдателя предназначена для постоянно работающего узла M5Stack Core +
E22-900M30S. Узел принимает пакеты MeshCore по LoRa, работает с MQTT и публикует
телеметрию подключённого ENV II. На дисплее доступны погодная панель и отдельная
страница состояния радио, датчика, Wi-Fi и MQTT.

Наблюдатель не является BLE-компаньоном и не появляется в списке Bluetooth.
В приложении MeshCore он становится виден как удалённая нода после того, как его
объявление примет подключённый компаньон. Для связи у наблюдателя и компаньона
должны совпадать частота, полоса, `SF` и `CR`.

В прошивку встроен автономный погодный бот. В стандартном групповом канале
MeshCore `Public` отправьте отдельное сообщение:

```text
/погода
```

Также поддерживается команда `/weather`. Наблюдатель ответит в тот же канал,
например:

```text
M5Stack2 E22 Observer: Погода 18.08.2026 10:05 | Темп. 25.0°C | Влажн. 57.1% | Давл. 995.7 гПа | Роса 15.9°C
```

Ответ содержит дату и время запроса в часовом поясе Тюмени (`UTC+5`), температуру,
влажность, атмосферное давление и точку росы. Высота и заряд аккумулятора в ответ
не включаются. Между ответами действует защитный интервал 30 секунд.

Бот отвечает только в стандартном канале `Public`: пользовательские приватные
каналы он не расшифровывает. Если ответа нет, сначала убедитесь, что наблюдатель
появился среди удалённых нод, принимает сообщения и видит ENV II. Расширенное
описание находится в разделе [«Погодный бот»](meshcore-custom/README.md#погодный-бот).

### Подключение ESP32-C3 Super Mini к E22-900M30S

| ESP32-C3 |   Контакт E22 | Назначение |
| -------: | ------------: | ---------- |
|   GPIO10 |            19 | NSS / CS   |
|    GPIO4 |            18 | SCK        |
|    GPIO6 |            17 | MOSI       |
|    GPIO5 |            16 | MISO       |
|    GPIO0 |            15 | NRST       |
|    GPIO1 |            14 | BUSY       |
|    GPIO3 |            13 | DIO1       |
|   GPIO20 |             7 | TXEN       |
|   GPIO21 |             6 | RXEN       |
|       5V |        9 и 10 | Питание    |
|      GND | 11, 12 или 20 | Земля      |

### Сборка железа

1. Полностью отключите USB и внешнее питание.
2. Соедините линии SPI, `NRST`, `BUSY`, `DIO1`, `TXEN` и `RXEN` по таблице для
   выбранной платы.
3. Соедините землю платы с землёй E22. Общая земля обязательна даже при отдельном
   источнике питания радиомодуля.
4. Подайте стабильные `5 V` на оба контакта питания E22, контакты 9 и 10. Используйте
   короткие и достаточно толстые провода.
5. Установите конденсатор `470-1000 µF` между VCC и GND как можно ближе к E22.
   Параллельно установите `10-47 µF` и `0.1 µF`.
6. Подключите центральную жилу антенного кабеля к площадке `ANT` E22, pin 21, а
   экран кабеля к соседней земле.
7. Мультиметром убедитесь, что между 5 V и GND нет короткого замыкания.
8. Подключите антенну и только после этого включите питание.

При питании ESP32-C3 от USB контакт `5V` платы можно использовать для E22 только
если USB-источник, дорожки платы и проводка выдерживают импульсный ток передатчика.
Надёжнее использовать отдельную стабилизированную линию 5 V с общей землёй. Не
подавайте внешние 5 V одновременно с USB, если схема не защищена от обратного тока.

После загрузки типичные уровни такие: E22 `NRST` около `3.3 V`, `NSS` около
`3.3 V`, `BUSY` около `0 V`. Они помогают обнаружить ошибку монтажа, но не заменяют
проверку всех соединений.

### Установка esptool

Откройте терминал в корне репозитория.

macOS или Linux:

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

### Поиск последовательного порта

macOS:

```bash
ls /dev/cu.usb*
```

Linux:

```bash
ls /dev/ttyUSB* /dev/ttyACM*
```

В Windows номер порта `COM` указан в диспетчере устройств. Если порт не появился,
проверьте, что USB-кабель поддерживает передачу данных.

### Прошивка M5Stack Core

BLE-компаньон:

```bash
python -m esptool --chip esp32 --port /dev/cu.usbserial-XXXX --baud 460800 write_flash 0x0 meshcore-custom/firmware/meshcore_m5stack_core_e22_companion_ble_rus_tjm_tx22_merged.bin
```

Наблюдатель MQTT:

```bash
python -m esptool --chip esp32 --port /dev/cu.usbserial-XXXX --baud 460800 write_flash 0x0 meshcore-custom/firmware/meshcore_m5stack_core_e22_observer_mqtt_tjm_tx22_merged.bin
```

Замените `/dev/cu.usbserial-XXXX` на найденный порт. В Windows укажите, например,
`COM5`. Если M5Stack не переходит в загрузчик автоматически, кратко нажмите RESET
и повторите команду.

### Прошивка ESP32-C3 Super Mini

```bash
python -m esptool --chip esp32c3 --port /dev/cu.usbmodemXXXX --baud 460800 write_flash 0x0 meshcore-custom/firmware/meshcore_esp32c3_supermini_e22_repeater_tjm_tx22_merged.bin
```

Если соединение не начинается:

1. удерживайте кнопку `BOOT`;
2. кратко нажмите `RESET`;
3. отпустите `RESET`, затем `BOOT`;
4. повторите команду прошивки.

Команда `erase_flash` для обычного обновления не нужна: она удаляет настройки и
идентификатор ноды.

### Первый запуск и проверка

1. Перезагрузите устройство после прошивки.
2. Для M5Stack-компаньона откройте приложение MeshCore, подключитесь по BLE и
   используйте PIN `123456`. Средняя кнопка `B` включает экран и меняет страницы.
3. Репитер ESP32-C3 не отображается как отдельное BLE-устройство. Его объявление
   принимается по LoRa и появляется через подключённый компаньон.
4. Убедитесь, что на всех устройствах совпадают частота, BW, SF и CR.
5. Сначала проверьте приём и передачу на небольшом расстоянии, затем переходите к
   внешней антенне и дальнему ретранслятору.

Если устройство принимает сообщения, но не передаёт, в первую очередь измерьте
напряжение 5 V непосредственно на E22 во время передачи, проверьте общую землю,
линии `TXEN`/`RXEN`, антенный разъём и КСВ антенны.

## Stats

![Alt](https://repobeats.axiom.co/api/embed/8025e56c482ec63541593cc5bd322c19d5c0bdcf.svg "Repobeats analytics image")
