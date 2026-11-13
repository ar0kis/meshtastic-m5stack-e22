# ESP32-C3 Super Mini + E22-900M22S Observer Repeater

This variant builds MeshCore as a repeater with the MQTT observer bridge for an
ESP32-C3 Super Mini connected to an Ebyte E22-900M22S (SX1262).

## Wiring

| ESP32-C3 Super Mini | Direction | E22-900M22S | Module pin |
|---|---:|---|---:|
| 3V3 | -> | VCC | 9 |
| GND | — | GND | 1–5, 10–12, 20 |
| GPIO0 | -> | SCK | 18 |
| GPIO3 | <- | MISO | 16 |
| GPIO2 | -> | MOSI | 17 |
| GPIO1 | -> | NSS | 19 |
| GPIO4 | <- | BUSY | 14 |
| GPIO21 | <- | DIO1 | 13 |
| GPIO10 | -> | NRST | 15 |
| GPIO6 | -> | RXEN | 6 |
| GPIO7 | -> | TXEN | 7 |

GPIO21 is available on the ESP32-C3 used by this build and was verified on the
connected board. This table is for the larger **E22-900M22S**, not the smaller
E22-900MM22S; their module pin numbers are different.

## Defaults

- Environment: `supermini_c3_e22_m22s_repeater_observer_mqtt`
- Role: MeshCore Repeater + MQTT Observer
- Name: `SNodeREP-1`
- Frequency: 869.050 MHz
- Bandwidth: 62.5 kHz
- Spreading factor: 8
- Coding rate: 7
- TX power: 22 dBm
- Repeating: enabled

Wi-Fi and MQTT broker credentials are deliberately not compiled in. No
MeshCoreTel settings are included.

## Build

From the repository root:

```sh
pio run -e supermini_c3_e22_m22s_repeater_observer_mqtt
pio run -e supermini_c3_e22_m22s_repeater_observer_mqtt -t mergebin
```

The merged image is written to:

```text
.pio/build/supermini_c3_e22_m22s_repeater_observer_mqtt/firmware-merged.bin
```

Verified artifact:

```text
Size:   1,750,880 bytes
SHA256: cadaef403c2370e83c56ac95e797ac0402b9e3ab24eb1a018a6585fa4290c4c1
```

## Flash while preserving the node identity

Before a full-chip or merged-image flash, save the output of `get prv.key` to a
private local file. Never commit it or paste it into an issue. Also record the
full node ID. After flashing, restore the key over the physical USB serial
console and reboot:

```text
set prv.key <64-hex-character-private-key>
reboot
```

Confirm that the full node ID is identical to the recorded value. A firmware-only
update may preserve preferences, but an identity backup is still required.

## Runtime Wi-Fi and MQTT setup

Configure only the network and broker you intend to use. These placeholders are
not real credentials:

```text
set wifi.ssid <YOUR_WIFI_SSID>
set wifi.pwd <YOUR_WIFI_PASSWORD>
get mqtt.presets
set mqtt1.preset <CHOSEN_PRESET>
set mqtt.iata <YOUR_SITE_CODE>
reboot
```

For a custom broker, use the `mqtt1.*` commands reported by `help` instead of a
preset. Until Wi-Fi and a valid MQTT slot are configured, the repeater continues
to work over LoRa but `get mqtt.status` reports that the bridge is not running.
NTP time synchronization also requires a working Wi-Fi connection.

## Verification

Use the physical USB serial console:

```text
get name
get radio
get tx
get repeat
get mqtt.rx
get mqtt.tx
get mqtt.ntp
get mqtt.config.valid
get mqtt.status
stats
advert
stats
```

Expected core values are `SNodeREP-1`, 869.050 MHz / 62.5 kHz / SF8 / CR7,
22 dBm, and repeating enabled. After `advert`, the sent/flood TX counters should
increase. Received counters increasing confirms LoRa RX activity.
