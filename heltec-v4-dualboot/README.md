# Heltec V4: MeshCore Companion + Meshtastic

Готовый двойной образ хранит MeshCore и Meshtastic в разных разделах flash-памяти
одного Heltec WiFi LoRa 32 V4. Перепрошивать плату при смене системы не нужно.

> Обе системы находятся в памяти одновременно, но в каждый момент работает только
> одна из них. Радио MeshCore и Meshtastic не могут работать параллельно.

## Состав образа

| Компонент                         | Версия           | Раздел     | Адрес      |
| --------------------------------- | ---------------- | ---------- | ---------- |
| Mesh Loader                       | `04b1412`        | `loader`   | `0x20000`  |
| MeshCore BLE Companion            | `v1.15.0`        | `meshcore` | `0xA0000`  |
| Meshtastic с русским OLED-шрифтом | `2.7.23.04b1412` | `meshtas`  | `0x3A0000` |

Образ рассчитан на ESP32-S3 с flash-памятью `16 MB`, режимом `DIO 80 MHz` и
отдельными файловыми системами `core_fs` и `tastic_fs`. Он предназначен именно для
Heltec V4 и не подходит для Heltec V3, V4 R8 и плат с flash-памятью другого размера.

Исходный проект загрузчика:
[eliahreeves/mesh-loader](https://github.com/eliahreeves/mesh-loader/tree/04b1412ac081809b0979b7bac126c97d775fe402).

## Файлы

- [готовый BIN](firmware/firmware-merged-heltec_v4-rus-meshtastic2.7.23-meshcore1.15.0.bin);
- [тот же BIN в ZIP](firmware/firmware_merged_heltec_v4_rus_meshtastic2_7_23_meshcore1_15_0.zip).

Контрольные суммы SHA-256:

```text
2e0130e9270472055df2230f79a2e836a22cada1b233e158f4b7df7f1080b383  firmware-merged-heltec_v4-rus-meshtastic2.7.23-meshcore1.15.0.bin
4bafaa2a3a32a90ae4c8c3044f0a2a493e0b69570cb213905369007d5bd569ff  firmware_merged_heltec_v4_rus_meshtastic2_7_23_meshcore1_15_0.zip
```

## Переключение MeshCore и Meshtastic

1. Перезагрузите Heltec кнопкой `RST` или включите питание.
2. На экране на две секунды появится `Mesh Loader` и название выбранной системы.
3. Чтобы загрузить другую систему, один раз нажмите кнопку `PRG/BOOT` (`GPIO0`),
   пока виден экран загрузчика.
4. Если кнопку не нажимать, будет запущена система, выбранная в прошлый раз.

Выбор сохраняется. При следующем включении загрузчик снова даст две секунды для
его изменения.

## Прошивка через USB

Установите esptool в отдельное виртуальное окружение:

```bash
python3 -m venv .venv-flash
source .venv-flash/bin/activate
python -m pip install "esptool>=4.5,<5"
```

Найдите порт на macOS:

```bash
ls /dev/cu.usb*
```

Запишите полный образ с адреса `0x0`:

```bash
python -m esptool --chip esp32s3 --port /dev/cu.usbmodemXXXX --baud 460800 write_flash 0x0 heltec-v4-dualboot/firmware/firmware-merged-heltec_v4-rus-meshtastic2.7.23-meshcore1.15.0.bin
```

Замените `/dev/cu.usbmodemXXXX` на фактический порт. В Windows используйте номер
вида `COM5`, в Linux - `/dev/ttyACM0` или `/dev/ttyUSB0`.

Если соединение не начинается:

1. удерживайте `PRG/BOOT`;
2. кратко нажмите `RST`;
3. отпустите `RST`, затем `PRG/BOOT`;
4. повторите команду прошивки.

После завершения нажмите `RST`. Первая загрузка может занять немного больше
времени обычного.

## Важно

- Полная запись образа заменяет текущую разметку и настройки платы. Сохраните
  нужные ключи и конфигурацию заранее.
- Не запускайте OTA-обновление из MeshCore или Meshtastic: стандартное OTA может
  нарушить двойную разметку. Обновляйте устройство только полным объединённым
  образом через USB с адреса `0x0`.
- После перехода между системами может потребоваться удалить старое Bluetooth-
  сопряжение Heltec в настройках телефона и подключиться заново.
- Частота, LoRa-профиль и мощность настраиваются отдельно в каждой системе.
  Соблюдайте ограничения своего региона.

## Проверка образа

Проверены таблица разделов ESP32-S3, размер flash `16 MB`, контрольные суммы
приложений и validation hash. MeshCore содержит BLE-интерфейс компаньона и строку
версии `v1.15.0`; Meshtastic содержит строку версии `2.7.23.04b1412`.
