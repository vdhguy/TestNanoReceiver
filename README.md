# TestNanoReceiver

Arduino Nano firmware to receive wireless data from an HC-12 module and forward it to a PC over USB Serial. Built with PlatformIO.

## Hardware

| Component | Description |
|-----------|-------------|
| Arduino Nano (new bootloader) | Main controller |
| HC-12 wireless module | 433 MHz serial transceiver |
| LED | Activity indicator on received message |

### Wiring

| Arduino Pin | Connects to |
|-------------|-------------|
| D2 (RX) | HC-12 TX |
| D3 (TX) | HC-12 RX |
| D4 | HC-12 SET |
| D5 | LED (+ resistor to GND) |

> The HC-12 SET pin must be pulled LOW to enter AT command mode. It is held HIGH during normal operation.

## Usage

Open a Serial Monitor at **9600 baud**. On startup the firmware immediately begins receiving HC-12 data:

```
HC-12 active. Press SPACE for menu.
```

Press **SPACE** at any time to pause reception and open the menu:

```
=== HC-12 Menu ===
1) Read HC-12 and output to Serial USB
2) Stop reading
3) Configure HC-12
==================
Choice:
```

| Key | Action |
|-----|--------|
| `1` | Resume HC-12 reception, exit menu |
| `2` | Stop reception, exit menu |
| `3` | Open HC-12 configuration submenu |

### Configure HC-12 (option 3)

```
--- HC-12 Configuration ---
1) Set channel
2) Read current channel
Choice:
```

**Set channel** — prompts for a channel number (1–127), then sends `AT+C` to the HC-12 via its SET pin and prints the module's response.

**Read current channel** — sends `AT+RC` to the HC-12 and prints the currently stored channel (e.g. `+RC:001`).

## Build & Flash

Requires [PlatformIO](https://platformio.org/).

```bash
# Build
pio run

# Upload
pio run --target upload

# Open serial monitor
pio device monitor --baud 9600
```

## Project Structure

```
TestNanoReceiver/
├── src/
│   └── main.cpp       # Firmware source
├── platformio.ini     # PlatformIO configuration
└── README.md
```
