# MBI5043_ESP32

**MBI5043_ESP32** is an ESP32-compatible library for controlling multiple MBI5043 LED driver chips.  
Developed for high-performance LED control on ESP32 platforms, this library simplifies interaction with the MBI5043 via SPI, supporting efficient multi-strip LED driving.

---

## ✨ Features
- Control multiple MBI5043 LED driver chips over SPI  
- Configurable output brightness, PWM, and channel updates  
- Designed for ESP32 environment with compatibility for Arduino and PlatformIO  
- Lightweight and easy to integrate into existing ESP32 projects  

---

## 📦 Installation
### Arduino IDE
1. Download the latest release as a `.zip` from the [releases page](https://github.com/ANLCRAW/MBI5043_ESP32/releases).  
2. In Arduino IDE:  
   **Sketch → Include Library → Add .ZIP Library...**  
   Select the downloaded archive to install.

### PlatformIO
Add the following line to your `platformio.ini` under the `[env:your_env]` section:

```ini
lib_deps = anlcraw/MBI5043_ESP32@^1.1.1
```

## 🔌 Hardware Wiring
The MBI5043 communicates via SPI. Below is a typical wiring example between the ESP32 and a single MBI5043 chip:

| ESP32 Pin | MBI5043 Pin | Description                |
| --------- | ----------- | -------------------------- |
| `MOSI`    | `SDI`       | Serial Data In             |
| `SCK`     | `CLK`       | SPI Clock                  |
| `CS`      | `LE`        | Latch Enable (chip select) |
| `GPIO`    | `OE`        | Output Enable (active low) |
| `GND`     | `GND`       | Ground                     |
| `3.3-4.5V`| `VDD`       | Power supply (logic)       |


**For multiple MBI5043 chips:**
- Connect all SDO (Serial Data Out) of one chip to the SDI of the next (daisy-chaining).
- Share CLK, LE, and OE across all chips.

⚠️ **Note:** Ensure your LED power supply is sized properly. The ESP32’s 3.3V logic is sufficient for the MBI5043 input pins, but the LED outputs require an external supply (typically 5V, 12V or 24V).

## 📂 Examples
**Check the examples/ directory for practical demos, including:**
- Driver initialization
- Multi-chip configuration
- Dynamic LED updates
- Basic animations and color patterns

## 📖 API Overview
| Method                         | Description                                     |
| ------------------------------ | ----------------------------------------------- |
| `begin()`                      | Initialize the MBI5043 driver and SPI interface |
| `setBrightness(chip, ch, val)` | Set PWM brightness for a specific LED channel   |
| `update()`                     | Write buffered values to the hardware           |
| `reset()`                      | Send clear/reset command to all channels        |

## 📝 To-Do
- Add more comprehensive documentation & usage guides
- Add calibration utilities
- Provide more stability
- Provide ESP-IDF (non-Arduino) compatibility
 
## 📄 License

This project is released under the BSD-3-Clause License.
See [LICENSE](https://github.com/ANLCRAW/MBI5043_ESP32/edit/main/LICENSE) for details.

## 🤝 Contributing
Contributions, bug reports, and feature requests are welcome!
Open an [issue](https://github.com/ANLCRAW/MBI5043_ESP32/issues) to report bugs or request features.
Submit a [pull request](https://github.com/ANLCRAW/MBI5043_ESP32/pulls) with improvements.
