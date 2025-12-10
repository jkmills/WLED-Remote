# ESP32 WLED Remote Control
### For DOIT ESP32 DEVKIT V1

Control your WLED lights using ESP32 with ultra-low power consumption. Choose between a simple touch sensor or a full 8-button remote control.

---

## Choose Your Version

### 🖐️ Touch Sensor Version
**Single capacitive touch pad to toggle lights on/off**

- **Code:** [esp32_devkit_v1.ino](esp32_devkit_v1.ino)
- **Documentation:** [TOUCH_VERSION_GUIDE.md](TOUCH_VERSION_GUIDE.md)
- **Best for:** Minimalist design, hidden installation, battery operation
- **Components:** ESP32 + touch pad (copper tape/foil)
- **Wiring:** Just 1 wire to GPIO4

### 🎛️ Multi-Button Version
**8 physical buttons for complete WLED control**

- **Code:** [esp32_multibutton.ino](esp32_multibutton.ino)
- **Documentation:** [MULTIBUTTON_GUIDE.md](MULTIBUTTON_GUIDE.md)
- **Quick Wiring:** [WIRING_GUIDE.md](WIRING_GUIDE.md)
- **Best for:** Full control, wall-mount remote, quick access
- **Components:** ESP32 + 8 push buttons
- **Functions:** ON/OFF, 4 Presets, Brightness±, Night Mode

---

## Quick Start

1. **Choose your version** (touch or multi-button)
2. **Read the documentation** for your chosen version
3. **Wire components** according to the guide
4. **Update configuration:**
   - WLED MAC address (line 37 for touch / line 70 for multi-button)
   - WiFi channel (line 10 in both)
5. **Upload code** to ESP32 using Arduino IDE
6. **Test** and enjoy!

---

## Features

### Both Versions Include:
- ✅ Ultra-low power deep sleep (~10µA)
- ✅ Instant wake on touch/button press
- ✅ ESP-NOW wireless communication (no WiFi network needed)
- ✅ State persistence through RTC memory
- ✅ Automatic retry on failed transmission
- ✅ Battery operation support (3-6 months on small battery)
- ✅ Built-in LED status indicator
- ✅ Detailed serial debugging output

### Multi-Button Exclusive:
- 8 programmable buttons
- Quick preset access
- Brightness control
- Night mode toggle
- Visual feedback on each button press

---

## Required Hardware

### For Touch Version:
- 1x ESP32 DEVKIT V1
- 1x Copper tape/foil (or any conductive material)
- 1-2x Jumper wires
- USB cable for programming

### For Multi-Button Version:
- 1x ESP32 DEVKIT V1
- 8x Momentary push buttons (normally open)
- 9x Jumper wires
- Breadboard (optional)
- USB cable for programming

### Optional (Both):
- LiPo battery 3.7V (500-2000mAh)
- Enclosure/case
- Power switch

**Total Cost:** $15-50 depending on components

---

## How It Works

1. **Deep Sleep:** ESP32 sleeps consuming only ~10µA
2. **Wake Up:** Touch sensor or button press wakes the device
3. **Initialize:** WiFi and ESP-NOW start up (~100ms)
4. **Send Command:** WLED command sent via ESP-NOW
5. **Confirm:** LED blinks, serial output confirms
6. **Sleep Again:** Returns to deep sleep immediately

**Battery Life:** Months of operation on a small battery!

---

## Configuration

### Find Your WLED MAC Address:
1. Open WLED web interface
2. Go to: **Config → WiFi Setup**
3. Note the **AP MAC** address
4. Format: `AA:BB:CC:DD:EE:FF` → `{0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}`

### Find Your WiFi Channel:
1. Check your router settings, or
2. Use a WiFi analyzer app on your phone
3. WLED uses the same channel as your WiFi

### Update Code:
```cpp
// Update these lines in the .ino file:
const uint8_t macAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // Your WLED MAC
#define CHANNEL 1  // Your WiFi channel (1-13)
```

---

## Wiring Summary

### Touch Version:
```
ESP32 GPIO4 ──────► Copper tape/touch pad
               (that's it!)
```

### Multi-Button Version:
```
ESP32 GPIO4  ──►  Button 1  ──►  GND
ESP32 GPIO16 ──►  Button 2  ──►  GND
ESP32 GPIO17 ──►  Button 3  ──►  GND
ESP32 GPIO18 ──►  Button 4  ──►  GND
ESP32 GPIO19 ──►  Button 5  ──►  GND
ESP32 GPIO21 ──►  Button 6  ──►  GND
ESP32 GPIO22 ──►  Button 7  ──►  GND
ESP32 GPIO23 ──►  Button 8  ──►  GND
```

*Internal pull-up resistors are used - no external resistors needed!*

---

## Troubleshooting

### Touch/Button Not Detected
- Check wire connections
- Verify GPIO pin numbers
- For touch: adjust threshold in code
- For buttons: verify normally-open type

### ESP-NOW Messages Fail
- Verify WLED MAC address is correct
- Ensure WiFi channel matches
- Check WLED is powered and in range
- Enable ESP-NOW in WLED settings

### Won't Wake from Sleep
- Check power supply (3.0-3.6V)
- Verify wake-up pin connections
- Ensure code uploaded successfully

### High Power Draw
- Disconnect USB during battery testing
- Verify board enters deep sleep (serial stops)
- Check for shorts or loose connections

**Full troubleshooting in version-specific guides**

---

## Project Files

| File | Description |
|------|-------------|
| [esp32_devkit_v1.ino](esp32_devkit_v1.ino) | Touch sensor version code |
| [esp32_multibutton.ino](esp32_multibutton.ino) | Multi-button version code |
| [TOUCH_VERSION_GUIDE.md](TOUCH_VERSION_GUIDE.md) | Complete touch version documentation |
| [MULTIBUTTON_GUIDE.md](MULTIBUTTON_GUIDE.md) | Complete multi-button documentation |
| [WIRING_GUIDE.md](WIRING_GUIDE.md) | Quick wiring reference for multi-button |
| [HARDWARE_SETUP.md](HARDWARE_SETUP.md) | General hardware information |
| [original.ino](original.ino) | Original reference code |

---

## Version Comparison

| Feature | Touch Version | Multi-Button |
|---------|--------------|--------------|
| **Wiring Complexity** | Very Simple (1 wire) | Moderate (8 buttons) |
| **Components** | Minimal | More components |
| **Cost** | $15-25 | $25-50 |
| **Functions** | ON/OFF only | 8 different functions |
| **Enclosure** | Easier (smaller) | Needs more space |
| **Battery Life** | Excellent | Excellent |
| **Aesthetics** | Minimalist | Control panel |
| **Best For** | Bedroom, hidden | Living room, wall-mount |

---

## Technical Specifications

### Power Consumption
- **Deep Sleep:** ~10µA
- **Active (transmitting):** ~80mA
- **Active Time:** 200-300ms per press
- **Battery Life (1000mAh):** 3-6 months typical use

### Wireless Communication
- **Protocol:** ESP-NOW (Espressif proprietary)
- **Range:** ~200m line-of-sight, 20-50m through walls
- **Latency:** < 100ms wake to transmit
- **No WiFi network required**

### Compatibility
- **Board:** DOIT ESP32 DEVKIT V1
- **Arduino Core:** 3.x (ESP-IDF 5.x)
- **WLED:** All recent versions with ESP-NOW support
- **Battery:** 3.0V - 3.6V (LiPo, AA with regulator)

---

## Arduino IDE Setup

1. **Install ESP32 Board Support:**
   - File → Preferences
   - Add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install

2. **Select Board:**
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module

3. **Configure Upload Settings:**
   - Upload Speed: 921600
   - Flash Frequency: 80MHz
   - Partition Scheme: Default

4. **Select Port:**
   - Tools → Port → (Your COM port)

5. **Upload:**
   - Click Upload button or Ctrl+U

---

## WLED Configuration

Enable ESP-NOW remote support in WLED:

1. Open WLED web interface
2. Go to **Config → Sync Interfaces**
3. Enable **"ESP-NOW Remote"**
4. Note the MAC address shown
5. Save settings

---

## Safety & Best Practices

- ✅ Use proper voltage (3.0V - 3.6V on GPIO pins)
- ✅ Add voltage regulator if using >3.6V battery
- ✅ Don't short GPIO pins
- ✅ Disconnect power when wiring
- ✅ LiPo batteries need careful handling
- ❌ Never use GPIO6-11 (flash pins)
- ❌ Don't exceed 40mA per GPIO pin

---

## Resources

- **ESP32 Documentation:** [Espressif Docs](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- **WLED Project:** [https://kno.wled.ge/](https://kno.wled.ge/)
- **ESP-NOW Protocol:** [ESP-NOW Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)
- **Arduino ESP32:** [https://github.com/espressif/arduino-esp32](https://github.com/espressif/arduino-esp32)

---

## License

This project is provided as-is for educational and personal use.

---

## Contributing

Found a bug or have an improvement? Please share your feedback!

---

## Changelog

### Version 1.0 (2025-12-10)
- Initial release
- Touch sensor version
- Multi-button version
- Complete documentation
- Compatible with ESP32 Arduino Core 3.x

---

**Ready to build? Pick your version above and follow the guide!** 🚀
