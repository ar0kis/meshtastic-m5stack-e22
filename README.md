## About MeshCore

MeshCore is a lightweight, portable C++ library that enables multi-hop packet routing for embedded projects using LoRa and other packet radios. It is designed for developers who want to create resilient, decentralized communication networks that work without the internet.

## 🔍 What is MeshCore?

MeshCore now supports a range of LoRa devices, allowing for easy flashing without the need to compile firmware manually. Users can flash a pre-built binary using tools like Adafruit ESPTool and interact with the network through a serial console.
MeshCore provides the ability to create wireless mesh networks, similar to Meshtastic and Reticulum but with a focus on lightweight multi-hop packet routing for embedded projects. Unlike Meshtastic, which is tailored for casual LoRa communication, or Reticulum, which offers advanced networking, MeshCore balances simplicity with scalability, making it ideal for custom embedded solutions, where devices (nodes) can communicate over long distances by relaying messages through intermediate nodes. This is especially useful in off-grid, emergency, or tactical situations where traditional communication infrastructure is unavailable.

> **MQTT Observer Setup:** prebuilt observer firmware, docs, and a changelog are at
> [observer.gessaman.com](https://observer.gessaman.com/). See the
> [MQTT Implementation Guide](./MQTT_IMPLEMENTATION.md) for configuration, CLI commands, and
> troubleshooting.

## 🇷🇺 Подключение ESP32-C3 + E22-900M22S Observer к MeshCoreTel

Эта инструкция предназначена для устройства с **Observer MQTT firmware** из данной ветки. Настройка выполняется через USB-консоль в [MeshCore Config](https://config.meshcore.io/) без повторной прошивки. Подробное описание всех команд см. в [MQTT_IMPLEMENTATION.md](./MQTT_IMPLEMENTATION.md).

### 1. Wi-Fi и регион

Подключите ESP32-C3 по USB, откройте консоль и отправляйте **каждую команду отдельно**, заменив имя сети и пароль своими значениями (без кавычек):

```text
set wifi.ssid YourWiFiNetwork
set wifi.pwd YourWiFiPassword
set mqtt.iata TJM
```

`TJM` — пример IATA-кода для Тюмени; для другого региона укажите подходящий трёхбуквенный код.

### 2. Добавление MQTT-брокера MeshCoreTel

В примере используется **слот 3**; прошивка поддерживает слоты `mqtt1`–`mqtt6`. Не перезаписывайте занятый слот без необходимости.

```text
set mqtt3.preset custom
set mqtt3.server mqtt.meshcoretel.ru
set mqtt3.port 1883
set mqtt3.username meshcore
set mqtt3.password meshcore
set mqtt.rx on
set mqtt.packets on
reboot
```

Это пример параметров публичного брокера, опубликованных на [MeshCoreTel](https://meshcoretel.ru/ru#observer-setup): MQTT/TCP, порт 1883, без TLS. Не публикуйте собственные пароли Wi-Fi или индивидуальные секреты в логах и issue.

### 3. Проверка

После перезагрузки выполните:

```text
get link.status
get mqtt.status
get mqtt3.diag
get mqtt3.topic
get mqtt.stats
```

При успешном подключении `get mqtt.status` содержит запись вида `3: custom (ok)`, а `get mqtt3.diag` — `mqtt3: ok, no errors`. Это **подтверждает подключение к брокеру, но не доказывает получение пакетов или появление устройства на сайте**. Для проверки публикаций дождитесь приёма LoRa-пакетов и проверьте счётчики/логи и отображение в MeshCoreTel. Стандартный шаблон топика custom-слота — `meshcore/{iata}/{device}/{type}`; если сервис требует другой формат, проверьте `get mqtt3.topic` и документацию сервиса, прежде чем менять его.

### 4. Если `mqtt3: inactive, no error info`

У ESP32-C3 без PSRAM есть ограничение ресурсов: прошивка допускает не более **двух TLS-слотов** (`wss://`, `mqtts://`). По умолчанию они могут быть заняты `analyzer-us` и `analyzer-eu`. При этом параметры `mqtt3` сохраняются, но слот может оставаться неактивным.

Если оба TLS-слота активны, освободите **один** из них (например, `analyzer-us`):

```text
set mqtt1.preset none
reboot
get mqtt.status
get mqtt3.diag
```

Либо вместо первого отключите второй: `set mqtt2.preset none`. **Не отключайте оба без необходимости.** Ожидаемое состояние — один оставшийся Analyzer `(ok)` и `3: custom (ok)`. Изменение MQTT-слотов отключает передачу в соответствующий брокер, **но не отключает радиоретрансляцию MeshCore**. Если слот всё ещё не работает, проверьте `get mqtt.config.valid`, `get bridge.enabled` и `memory`.

---

## ⚡ Key Features

* Multi-Hop Packet Routing
  * Devices can forward messages across multiple nodes, extending range beyond a single radio's reach.
  * Supports up to a configurable number of hops to balance network efficiency and prevent excessive traffic.
  * Nodes use fixed roles where "Companion" nodes are not repeating messages at all to prevent adverse routing paths from being used.
* Supports LoRa Radios – Works with Heltec, RAK Wireless, and other LoRa-based hardware.
* Decentralized & Resilient – No central server or internet required; the network is self-healing.
* Low Power Consumption – Ideal for battery-powered or solar-powered devices.
* Simple to Deploy – Pre-built example applications make it easy to get started.

## 🎯 What Can You Use MeshCore For?

* Off-Grid Communication: Stay connected even in remote areas.
* Emergency Response & Disaster Recovery: Set up instant networks where infrastructure is down.
* Outdoor Activities: Hiking, camping, and adventure racing communication.
* Tactical & Security Applications: Military, law enforcement, and private security use cases.
* IoT & Sensor Networks: Collect data from remote sensors and relay it back to a central location.

## 🚀 How to Get Started

- Watch the [MeshCore QuickStart Playlist](https://www.youtube.com/watch?v=iaFltojJrAc&list=PLshzThxhw4O4WU_iZo3NmNZOv6KMrUuF9) by The Comms Channel
- Watch the [MeshCore Technical Presentation](https://www.youtube.com/watch?v=OwmkVkZQTf4) by Liam Cottle.
- Read through our [Frequently Asked Questions](./docs/faq.md) and [Documentation](https://docs.meshcore.io).
- Flash the MeshCore firmware on a supported device.
- Connect with a supported client.

For developers:

- Install [PlatformIO](https://docs.platformio.org) in [Visual Studio Code](https://code.visualstudio.com).
- Clone and open the MeshCore repository in Visual Studio Code.
- See the example applications you can modify and run:
  - [Companion Radio](./examples/companion_radio) - For use with an external chat app, over BLE, USB or Wi-Fi.
  - [KISS Modem](./examples/kiss_modem) - Serial KISS protocol bridge for host applications. ([protocol docs](./docs/kiss_modem_protocol.md))
  - [Simple Repeater](./examples/simple_repeater) - Extends network coverage by relaying messages.
  - [Simple Room Server](./examples/simple_room_server) - A simple BBS server for shared Posts.
  - [Simple Secure Chat](./examples/simple_secure_chat) - Secure terminal based text communication between devices.
  - [Simple Sensor](./examples/simple_sensor) - Remote sensor node with telemetry and alerting.

The Simple Secure Chat example can be interacted with through the Serial Monitor in Visual Studio Code, or with a Serial USB Terminal on Android.

## ⚡️ MeshCore Flasher

We have prebuilt firmware ready to flash on supported devices.

- Launch https://meshcore.io/flasher
- Select a supported device
- Flash one of the firmware types:
  - Companion, Repeater or Room Server
- Once flashing is complete, you can connect with one of the MeshCore clients below.

## 📱 MeshCore Clients

**Companion Firmware**

The companion firmware can be connected to via BLE, USB or Wi-Fi depending on the firmware type you flashed.

- Web: https://app.meshcore.nz
- Android: https://play.google.com/store/apps/details?id=com.liamcottle.meshcore.android
- iOS: https://apps.apple.com/us/app/meshcore/id6742354151?platform=iphone
- NodeJS: https://github.com/liamcottle/meshcore.js
- Python: https://github.com/fdlamotte/meshcore-cli

**Repeater and Room Server Firmware**

The repeater and room server firmware can be set up via USB in the web config tool.

- https://config.meshcore.io

They can also be managed via LoRa in the mobile app by using the Remote Management feature.

## 🛠 Hardware Compatibility

MeshCore is designed for devices listed in the [MeshCore Flasher](https://meshcore.io/flasher)

## 📜 License

MeshCore is open-source software released under the MIT License. You are free to use, modify, and distribute it for personal and commercial projects.

## Contributing

Please submit PR's using 'dev' as the base branch!
For minor changes just submit your PR and we'll try to review it, but for anything more 'impactful' please open an Issue first and start a discussion. It is better to sound out what it is you want to achieve first, and try to come to a consensus on what the best approach is, especially when it impacts the structure or architecture of this codebase.

Here are some general principles you should try to adhere to:
* Keep it simple. Please, don't think like a high-level lang programmer. Think embedded, and keep code concise, without any unnecessary layers.
* No dynamic memory allocation, except during setup/begin functions.
* Use the same brace and indenting style that's in the core source modules. (A .clang-format is probably going to be added soon, but please do NOT retroactively re-format existing code. This just creates unnecessary diffs that make finding problems harder)

Help us prioritize! Please react with thumbs-up to issues/PRs you care about most. We look at reaction counts when planning work.

### Running unit tests

To run unit tests, run the following command:

```bash
pio test --environment native --verbose
```

## Road-Map / To-Do

There are a number of fairly major features in the pipeline, with no particular time-frames attached yet. In very rough chronological order:
- [X] Companion radio: UI redesign
- [X] Repeater + Room Server: add ACL's (like Sensor Node has)
- [X] Standardise Bridge mode for repeaters
- [ ] Repeater/Bridge: Standardise the Transport Codes for zoning/filtering
- [X] Core + Repeater: enhanced zero-hop neighbour discovery
- [ ] Core: round-trip manual path support
- [ ] Companion + Apps: support for multiple sub-meshes (and 'off-grid' client repeat mode)
- [ ] Core + Apps: support for LZW message compression
- [ ] Core: dynamic CR (Coding Rate) for weak vs strong hops
- [ ] Core: new framework for hosting multiple virtual nodes on one physical device
- [ ] V2 protocol spec: discussion and consensus around V2 packet protocol, including path hashes, new encryption specs, etc

## 📞 Get Support

- Report bugs and request features on the [GitHub Issues](https://github.com/meshcore-dev/meshcore/issues) page.
- Find additional guides and components on [my site](https://buymeacoffee.com/ripplebiz).
- Join [MeshCore Discord](https://meshcore.gg) to chat with the developers and get help from the community.
