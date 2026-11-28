# Toothbrushing STC8G

Firmware for a children's toothbrushing trainer built around an STC8G1K08A microcontroller.

## Table of Contents

- [Toothbrushing STC8G](#toothbrushing-stc8g)
  - [Table of Contents](#table-of-contents)
  - [About The Project](#about-the-project)
  - [Features](#features)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Usage](#usage)
  - [Configuration](#configuration)
    - [Timing](#timing)
    - [Pins](#pins)
  - [Contributing](#contributing)
  - [License](#license)
  - [Contact](#contact)
  - [Acknowledgments](#acknowledgments)

## About The Project

Brushing for the recommended two minutes is difficult for a child to judge unaided. This project drives a jaw model whose teeth are LEDs grouped into six mouth regions. The firmware highlights one region at a time, so the child can follow the light and know when to move on.

The board walks through the six groups in sequence, 20 seconds each (120 seconds total), then turns the LEDs off and enters power-down mode. Pressing the wake button starts a fresh session.

Intended users are children learning to brush and the hobbyists or parents building and flashing the trainer.

## Features

- Guided two-minute session: six mouth regions, 20 seconds per region
- Active region blinks at 500 ms intervals to show it is the current target
- Six LED groups driven through a 74HC595-style shift register; unused register outputs stay off
- Automatic power-down after the last group; a wake button restarts the session
- 10 ms system tick from Timer 0, with the per-region duration configurable in one header file

## Getting Started

### Prerequisites

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/) or the PlatformIO IDE extension
- An STC8G1K08A target and its programming interface
- The jaw hardware with the shift-register LED groups described under [Configuration](#configuration)

The project declares no external libraries; the register definitions ship in `include/stc8g.h`.

## Installation

```bash
git clone https://github.com/capcom6-hardware/toothbrushing-stc8g.git
cd toothbrushing-stc8g
pio run
```

The build for the `STC8G1K08A` environment produces `.pio/build/STC8G1K08A/firmware.hex`.

## Usage

Flash the firmware:

```bash
pio run -t upload
```

If the target is already powered, the uploader waits for a power cycle — switch the target power off and back on when prompted so programming can proceed.

On power-up or after a wake-button press, the firmware lights the first group and blinks it for 20 seconds, then moves to the next group. After the sixth group it clears all outputs and sleeps. Pressing the wake button starts the sequence again from the first group.

The group order equals the shift-register bit order:

| Bit | Group             |
| --- | ----------------- |
| 0   | Bottom front (BF) |
| 1   | Bottom left (BL)  |
| 2   | Bottom right (BR) |
| 3   | Top front (TF)    |
| 4   | Top left (TL)     |
| 5   | Top right (TR)    |

Each output drives a cluster of LEDs representing the teeth in that group.

## Configuration

### Timing

| File             | Setting          | Default                                 | Meaning                         |
| ---------------- | ---------------- | --------------------------------------- | ------------------------------- |
| `src/settings.h` | `TOOTH_DURATION` | `20`                                    | Seconds each group stays active |
| `src/main.c`     | `SECOND_COUNTER` | `100`                                   | Timer 0 ticks per second        |
| `src/main.c`     | `SECOND_TIMER`   | `65536 - (F_CPU / 12 / SECOND_COUNTER)` | Timer 0 reload for a 10 ms tick |

The PlatformIO environment defines `F_CPU=11059200L`.

### Pins

| Signal  | MCU pin     | Purpose                    |
| ------- | ----------- | -------------------------- |
| `SER`   | P5.5        | Shift register serial data |
| `SRCLK` | P5.4        | Shift register shift clock |
| `RCLK`  | P3.2        | Shift register latch clock |
| `WAKE`  | P3.3 (INT1) | Wake button, falling edge  |

Pins P3.2, P5.4 and P5.5 are configured as outputs; P3.3 is a high-Z input with the INT1 falling-edge interrupt enabled.


## Contributing

Fork the repository, create a branch for your change, verify it with `pio run`, and open a pull request. Keep hardware-specific pin and timing changes documented in `src/settings.h` and this README.

## License

Apache License 2.0. See [LICENSE](LICENSE).

## Contact

Repository: https://github.com/capcom6-hardware/toothbrushing-stc8g

## Acknowledgments

- Vincent DEFERT for the STC8G register definitions in `include/stc8g.h` (copyright 2022)
- PlatformIO and the SDCC toolchain for the MCS-51 build
