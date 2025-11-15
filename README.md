# Pixie Oval LED Clock

**Oval LED analog clock for the Axiometa Pixie M1, using a 289-pixel 17×21 WS2812 matrix fitted inside a custom 3D-printed case. Features ultra-dim hour markers, smooth seconds ping-pong motion, pulsing minute/hour dots, oval-corrected mapping, and Wi-Fi NTP sync. Includes firmware, mapping, STL files, and build guides.**

---

## Features

- 289-pixel WS2812B circular/oval LED matrix (17×21)
- Ultra-dim 12 hour markers in yellow-green
- Smooth seconds ping-pong sweep animation
- Pulsing minute and hour dots
- Oval coordinate correction for true round display
- Wi-Fi NTP time synchronization
- Custom 3D-printed case with Pixie M1 integration
- Full firmware and build instructions included

---

## Hardware

- **Controller:** Axiometa Pixie M1 (ESP32-S3 based)
- **LEDs:** 289 WS2812B individually addressable
- **Matrix layout:** Oval 17×21 pixels
- **Power:** 5V, sufficient for full brightness
- **3D Case:** Custom printed enclosure with internal Pixie M1 mounting

---

## Wiring

| Component           | Pin / Connection                  |
|--------------------|---------------------------------|
| Pixie M1 DATA       | GPIO 6 → WS2812 DI              |
| 5V Power            | 5V → LED matrix + Pixie M1      |
| GND                 | GND → LED matrix + Pixie M1     |
| Wi-Fi               | Optional: configure SSID/Pass   |

> Ensure power supply can handle the peak LED current.

---

## Software / Firmware

- ESP32-S3 Arduino framework
- FastLED library for LED control
- NTP time synchronization via Wi-Fi
- Custom XY mapping for oval matrix
- Adjustable brightness for ultra-dim hour markers
- Seconds, minutes, hours animated with smooth pulse/trail effects

---

## 3D Case

- Designed to fit the 289-pixel oval matrix and Pixie M1
- STL files included in `3D_case/` directory
- Snap-fit or screw mounting options
- Cable channels for power and data line

---

## Installation

1. Install **Arduino IDE** with ESP32-S3 support
2. Install **FastLED library**
3. Open `PixieOvalClock.ino`
4. Configure Wi-Fi SSID/password if needed
5. Upload to Pixie M1
6. Assemble matrix into 3D-printed case
7. Power up and enjoy smooth oval LED clock

---

## Notes

- Brightness of hour markers is configurable in code (`nscale8_video`)
- Minute hand color is soft pink; hour hand is blue; seconds are orange
- Oval mapping ensures true circular display even with 17×21 pixel matrix
- Adjust `maxR` and `cx/cy` if you change matrix size or shape

---

## License

MIT License – feel free to use and modify for personal or educational projects.
