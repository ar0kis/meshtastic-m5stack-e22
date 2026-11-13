# ESP32-C3 + E22-900M22S Repeater Observer Design

## Intent

Build a standalone MeshCore repeater plus Wi-Fi/MQTT observer for the existing
ESP32-C3 Super Mini and EBYTE E22-900M22S node. Preserve the node identity and
name `SNodeREP-1`; do not adopt any MeshCoreTel radio or MQTT settings.

## Requirements

- Radio wiring: SCK GPIO0, MISO GPIO3, MOSI GPIO2, NSS GPIO1, BUSY GPIO4,
  DIO1 GPIO21, NRST GPIO10, RXEN GPIO6, TXEN GPIO7.
- Radio profile: 869.050 MHz, BW 62.5 kHz, SF8, CR7, TX 22 dBm.
- Role: `simple_repeater` with `WITH_MQTT_BRIDGE=1`.
- Board: ESP32-C3, 4 MB flash, no display, no PSRAM.
- Time: observer NTP support supplies wall clock after Wi-Fi connects.
- Secrets: Wi-Fi and MQTT credentials are configured at runtime and never
  committed.
- Identity: save `get prv.key` before destructive flashing, restore it after
  flashing, and verify the full public Repeater ID is unchanged.

## Approaches

The selected approach ports the existing Flex `repeater_observer_mqtt`
environment to a dedicated board variant. A USB-hosted observer was rejected
because the node must operate autonomously. Reimplementing MQTT from upstream
MeshCore was rejected because the selected fork already supplies the tested
observer bridge, NTP, broker configuration, and web/CLI configuration.

## Safety and verification

Build and static configuration checks happen before touching the device.
Before flashing, save both the private key and a full flash backup locally.
After flashing, restore the private key and name, then verify boot, SX1262,
radio parameters, LoRa transmit/receive counters, MQTT configuration surface,
NTP clock, and identity persistence after reboot.
