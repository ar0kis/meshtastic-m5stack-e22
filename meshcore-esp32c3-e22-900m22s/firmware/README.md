# Готовая прошивка

GitHub API ограничивает размер двоичного файла, поэтому проверенный объединённый
образ сохранён тремя частями. Перед прошивкой соберите его одной командой:

```bash
python3 assemble_firmware.py
```

Скрипт создаст:

```text
meshcore_esp32c3_e22-900m22s_repeater_v1.18.0_merged.bin
```

и автоматически проверит SHA-256:

```text
89a147090a6c334ac8b641944f1c5a93869d69fbc8cd0f9bf878365289cec398
```

После этого прошейте файл с адреса `0x0`:

```bash
esptool --chip esp32c3 --port PORT write-flash 0x0 meshcore_esp32c3_e22-900m22s_repeater_v1.18.0_merged.bin
```

Размер итогового файла: 1 282 416 байт.
