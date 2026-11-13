#!/usr/bin/env python3
from hashlib import sha256
from pathlib import Path

folder = Path(__file__).resolve().parent
stem = "meshcore_esp32c3_e22-900m22s_repeater_v1.18.0_merged.bin"
parts = sorted(folder.glob(stem + ".part*"))
if not parts:
    raise SystemExit("Firmware parts not found")

output = folder / stem
with output.open("wb") as destination:
    for part in parts:
        destination.write(part.read_bytes())

digest = sha256(output.read_bytes()).hexdigest()
expected = "89a147090a6c334ac8b641944f1c5a93869d69fbc8cd0f9bf878365289cec398"
if digest != expected:
    output.unlink(missing_ok=True)
    raise SystemExit(f"SHA-256 mismatch: {digest}")

print(f"Created: {output}")
print(f"SHA-256: {digest}")
