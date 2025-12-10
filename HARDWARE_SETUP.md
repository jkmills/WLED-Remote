# ESP32 DEVKIT V1 - WLED Touch Remote
## Hardware Setup and Pinout Guide

---

## Table of Contents
1. [Required Components](#required-components)
2. [ESP32 DEVKIT V1 Pinout Reference](#esp32-devkit-v1-pinout-reference)
3. [Touch Sensor Setup](#touch-sensor-setup)
4. [Wiring Diagrams](#wiring-diagrams)
5. [Power Supply Options](#power-supply-options)
6. [Testing and Troubleshooting](#testing-and-troubleshooting)

---

## Required Components

### For Single Touch Version (esp32_devkit_v1.ino)
**Essential Components:**
- **1x DOIT ESP32 DEVKIT V1** - Main microcontroller board
- **1x USB Cable (Micro-USB)** - For programming and power
- **Touch sensor or conductive material** - For touch input (see options below)
- **Jumper wires** - For connections

### For Multi-Button Version (esp32_multibutton.ino)
**Essential Components:**
- **1x DOIT ESP32 DEVKIT V1** - Main microcontroller board
- **1x USB Cable (Micro-USB)** - For programming and power
- **8x Momentary Push Buttons** (normally open, SPST)
  - Recommended: 12mm tactile switches or arcade-style buttons
  - Voltage rating: Any (we use 3.3V logic)
  - Current rating: Minimal (10mA is plenty)
- **Jumper wires** - For connections (9 wires minimum: 8 buttons + 1 ground)
- **Breadboard or PCB** - For prototyping/permanent installation

**Button Options:**
- **12mm Tactile Switches** - Compact, PCB-mountable (~$0.10 each)
- **6mm Tactile Switches** - Very compact (~$0.05 each)
- **16mm Illuminated Arcade Buttons** - Large, satisfying click (~$2-3 each)
- **Membrane Keypad 4x4** - Pre-wired matrix (you can use 8 buttons from it)

### Optional Components (Both Versions)
- **1x Battery holder** (for portable operation)
  - LiPo battery (3.7V, 500mAh - 2000mAh recommended)
  - Or 3x AA battery holder (4.5V) with voltage regulator
- **1x Slide switch** - For power on/off
- **1x 10kΩ resistor** - For pull-up if needed (usually not required)
- **Enclosure/Case** - For finished project
- **Copper tape or aluminum foil** - DIY touch pad option

---

## ESP32 DEVKIT V1 Pinout Reference

### Board Layout
```
                    ESP32 DEVKIT V1
                   ┌─────────────┐
                   │   USB Port  │
                   └─────────────┘

 Left Side                              Right Side
 ┌──────────────┐                  ┌──────────────┐
 │ 3V3          │ Power Out        │         GND  │ Ground
 │ EN           │ Reset            │         23   │ GPIO23
 │ VP (GPIO36)  │ Input Only       │         22   │ GPIO22
 │ VN (GPIO39)  │ Input Only       │         TX   │ GPIO1
 │ 34           │ Input Only       │         RX   │ GPIO3
 │ 35           │ Input Only       │         21   │ GPIO21
 │ 32           │ GPIO32 (T9)      │         19   │ GPIO19
 │ 33           │ GPIO33 (T8)      │         18   │ GPIO18
 │ 25           │ GPIO25           │         5    │ GPIO5
 │ 26           │ GPIO26           │         17   │ GPIO17
 │ 27           │ GPIO27 (T7)      │         16   │ GPIO16
 │ 14           │ GPIO14 (T6)      │         4    │ GPIO4 (T0) ⭐
 │ 12           │ GPIO12 (T5)      │         0    │ GPIO0 (T1)
 │ GND          │ Ground           │         2    │ GPIO2 (T2/LED)
 │ 13           │ GPIO13 (T4)      │         15   │ GPIO15 (T3)
 │ D2 (GPIO9)   │ Flash (avoid)    │         D1   │ GPIO8 (Flash)
 │ D3 (GPIO10)  │ Flash (avoid)    │         D0   │ GPIO7 (Flash)
 │ CMD (GPIO11) │ Flash (avoid)    │         CLK  │ GPIO6 (Flash)
 │ 5V           │ 5V In/Out        │         GND  │ Ground
 └──────────────┘                  └──────────────┘

⭐ = Pin used in this project
```

### Touch-Capable GPIO Pins
The ESP32 has 10 touch-sensitive pins:
- **T0 = GPIO4** ⭐ (Used in this project)
- **T1 = GPIO0** (Connected to BOOT button)
- **T2 = GPIO2** (Built-in LED)
- **T3 = GPIO15**
- **T4 = GPIO13**
- **T5 = GPIO12**
- **T6 = GPIO14**
- **T7 = GPIO27**
- **T8 = GPIO33**
- **T9 = GPIO32**

### Special Pin Notes
- **GPIO2**: Built-in LED (used for status indication)
- **GPIO0**: BOOT button (entering flash mode when LOW during startup)
- **GPIO1 (TX), GPIO3 (RX)**: Serial communication (avoid if using Serial Monitor)
- **GPIO6-11**: Connected to flash memory (DO NOT USE)
- **GPIO34-39**: Input only, no internal pull-up/pull-down resistors

---

## Touch Sensor Setup

### Option 1: Direct Touch Pad (Recommended for Low Power)
This is the simplest option and consumes the least power.

**Components:**
- Copper tape, aluminum foil, or conductive fabric
- Tape or adhesive

**Wiring:**
```
ESP32 DEVKIT V1          Touch Pad
┌─────────────┐          ┌─────────┐
│             │          │         │
│ GPIO4 (T0)  ├──────────┤  Touch  │ (Copper tape/foil)
│             │          │   Pad   │
│             │          │         │
│ GND         │ (no connection needed)
└─────────────┘          └─────────┘
```

**Setup Instructions:**
1. Cut a piece of copper tape or aluminum foil (approximately 2cm x 2cm or larger)
2. Connect a jumper wire to GPIO4
3. Attach the other end of the wire to the conductive material
4. Secure with tape or adhesive
5. Optionally, cover with non-conductive decorative material (paper, plastic)

**Sensitivity Adjustment:**
- Increase `TOUCH_THRESHOLD` value (currently 40) for less sensitive
- Decrease value for more sensitive
- Test by monitoring Serial output to see touch values

### Option 2: TTP223 Capacitive Touch Sensor Module
A ready-made touch sensor module with better noise immunity.

**Module Specifications:**
- Operating Voltage: 2.0V - 5.5V
- Operating Current: 1.5µA (standby)
- Response Time: 60ms - 220ms

**Wiring:**
```
ESP32 DEVKIT V1          TTP223 Module
┌─────────────┐          ┌─────────┐
│             │          │         │
│ 3V3         ├──────────┤  VCC    │
│             │          │         │
│ GPIO4       ├──────────┤  I/O    │ (Signal)
│             │          │         │
│ GND         ├──────────┤  GND    │
│             │          │         │
└─────────────┘          └─────────┘
```

**Note:** TTP223 modules output HIGH when touched, so you'll need to modify the code to use digital input instead of touch sensor.

### Option 3: DIY Metal Button/Plate
Any conductive metal object can work as a touch sensor.

**Examples:**
- Metal pushpin
- Coin taped down
- Metal screw/bolt
- Copper wire coil

Simply connect GPIO4 to any exposed metal surface.

---

## Wiring Diagrams

### Multi-Button Setup (8 Physical Buttons)

For the multi-button version ([esp32_multibutton.ino](c:\Local\esp32-remote\esp32_multibutton.ino)), use momentary push buttons (normally open):

```
              ESP32 DEVKIT V1
           ┌───────────────────┐
           │                   │
  [BTN1]───┤ GPIO4  (ON/OFF)   │
  [BTN2]───┤ GPIO16 (Preset 1) │
  [BTN3]───┤ GPIO17 (Preset 2) │
  [BTN4]───┤ GPIO18 (Preset 3) │
  [BTN5]───┤ GPIO19 (Preset 4) │
  [BTN6]───┤ GPIO21 (Bright +)  │
  [BTN7]───┤ GPIO22 (Bright -)  │
  [BTN8]───┤ GPIO23 (Night)     │
           │                   │
      GND──┤ GND               │
           │                   │
           │ GPIO2 (LED) ●     │
           │                   │
           └───────────────────┘

All buttons connect: One side to GPIO, other side to GND
Internal pull-up resistors are enabled in software
```

**Detailed Button Wiring:**
```
Each button uses this configuration:

    GPIO Pin ────┬──── [Button] ──── GND
                 │
              (10kΩ pull-up
               internal to ESP32)

When button is pressed: GPIO reads LOW (0V)
When button is released: GPIO reads HIGH (3.3V via pull-up)
```

**Physical Layout Example:**
```
  ┌─────────────────────────────────────┐
  │        WLED Remote Control          │
  │                                     │
  │   ┌────────┐         ┌────────┐    │
  │   │ON/OFF  │         │Preset 1│    │
  │   └────────┘         └────────┘    │
  │                                     │
  │   ┌────────┐         ┌────────┐    │
  │   │Preset 2│         │Preset 3│    │
  │   └────────┘         └────────┘    │
  │                                     │
  │   ┌────────┐         ┌────────┐    │
  │   │Preset 4│         │ NIGHT  │    │
  │   └────────┘         └────────┘    │
  │                                     │
  │      ┌────┐            ┌────┐      │
  │      │ +  │            │ -  │      │
  │      └────┘            └────┘      │
  │     Brightness       Brightness    │
  │                                     │
  │               [●]  Status LED      │
  │                                     │
  └─────────────────────────────────────┘
```

### Minimal Setup (USB Powered - Single Touch)
```
┌─────────────────────────────────────┐
│         ESP32 DEVKIT V1             │
│                                     │
│  GPIO4 (T0) ──┬─── [Touch Pad]     │
│               │                     │
│  GPIO2 ───────┼─── (Built-in LED)  │
│               │                     │
│  USB Port ────┴─── [USB Cable]     │
│                    to Computer      │
└─────────────────────────────────────┘
```

### Battery Powered Setup (LiPo)
```
                    ┌─────────────┐
                    │   LiPo      │
                    │  Battery    │
                    │   3.7V      │
                    └──┬──────┬───┘
                       │      │
                    [Switch]  │
                       │      │
                       │      │
┌──────────────────────┼──────┼───────┐
│  ESP32 DEVKIT V1     │      │       │
│                      │      │       │
│  3V3 ────────────────┘      │       │
│                             │       │
│  GND ────────────────────────┘       │
│                                     │
│  GPIO4 ──── [Touch Pad]             │
│                                     │
│  GPIO2 ──── (Built-in LED)          │
│                                     │
└─────────────────────────────────────┘
```

**Important:**
- Use 3V3 pin (not 5V) when powered by 3.7V LiPo
- Maximum recommended current: 500mA
- Deep sleep current: ~10µA (battery can last months)

### Battery Powered Setup (AA Batteries)
```
   ┌─────────────┐
   │   3x AA     │
   │  Batteries  │
   │   4.5V      │
   └──┬──────┬───┘
      │      │
   [Switch]  │
      │      │
      │   ┌──▼───────────┐
      │   │ AMS1117-3.3  │ (Voltage Regulator)
      │   │  or similar  │
      │   └──┬───────────┘
      │      │
      │      │
┌─────┼──────┼─────────────────┐
│     │      │                 │
│  5V │      │                 │
│     │   3V3◄─ (Or use this) │
│     │      │                 │
│  GND├──────┘                 │
│                              │
│  GPIO4 ──── [Touch Pad]      │
│                              │
│  GPIO2 ──── (Built-in LED)   │
│                              │
└──────────────────────────────┘
```

---

## Power Supply Options

### USB Power (Development/Testing)
- **Voltage:** 5V from USB
- **Current:** Up to 500mA available
- **Use Case:** Programming, debugging, testing
- **Connection:** Micro-USB cable to computer or USB charger

### LiPo Battery (Recommended for Portable)
- **Voltage:** 3.7V nominal (3.0V - 4.2V range)
- **Capacity:** 500mAh - 2000mAh recommended
- **Runtime:** Months in deep sleep mode (wakes only on touch)
- **Connection:** Battery + to 3V3, Battery - to GND
- **Charging:** Remove battery and charge externally, or add TP4056 charging module

### AA Batteries
- **Configuration:** 3x AA in series (4.5V) with voltage regulator
- **Capacity:** ~2000mAh typical
- **Runtime:** Several months in deep sleep
- **Voltage Regulator:** AMS1117-3.3 or LM1117-3.3 recommended

### Power Consumption
- **Active Mode:** ~80mA (WiFi on, transmitting)
- **Deep Sleep:** ~10µA (minimal power draw)
- **Active Time:** ~200ms per touch (then back to sleep)
- **Estimated Battery Life:**
  - With 1000mAh battery and 10 touches per day: ~6-12 months

---

## Testing and Troubleshooting

### Initial Testing Steps

1. **Upload Code**
   - Connect ESP32 via USB
   - Open Arduino IDE
   - Select Board: "ESP32 Dev Module"
   - Select Port: Your COM port
   - Upload the sketch

2. **Open Serial Monitor**
   - Baud Rate: 115200
   - You should see: "=== ESP32 DEVKIT V1 WLED Remote ==="
   - MAC address will be displayed

3. **Test Touch Sensor**
   - Touch the connected pad/wire on GPIO4
   - Board should wake up
   - Serial monitor shows activity
   - Built-in LED (GPIO2) lights up or turns off
   - Board enters deep sleep after 200ms

4. **Verify ESP-NOW Communication**
   - Check serial output for "Delivery Status"
   - Should show "Success" if WLED device is reachable
   - Or "Fail. Retrying" if not connected

### Troubleshooting Guide

#### Problem: Touch not detected
**Solutions:**
- Increase touch pad size (larger = more sensitive)
- Lower `TOUCH_THRESHOLD` value in code (line 7)
- Check wire connection to GPIO4
- Try touching with more skin contact area
- Monitor actual touch values via serial to calibrate threshold

#### Problem: False triggers (activates without touch)
**Solutions:**
- Increase `TOUCH_THRESHOLD` value
- Shield touch pad from electromagnetic interference
- Add ground plane around touch sensor
- Use shorter wire connections

#### Problem: ESP-NOW messages fail
**Solutions:**
- Verify WLED MAC address is correct (line 36)
- Ensure WiFi channel matches WLED (line 10)
- Check distance to WLED device (ESP-NOW range: ~200m open air, less through walls)
- Verify WLED device has ESP-NOW remote enabled

#### Problem: Won't wake from deep sleep
**Solutions:**
- Verify touch sensor is connected to GPIO4 (T0)
- Check that `touchSleepWakeUpEnable()` is called in setup
- Ensure power supply is stable
- Try pressing/holding BOOT button, then touching sensor

#### Problem: High power consumption
**Solutions:**
- Verify board enters deep sleep (serial output should stop)
- Disconnect USB during battery testing (USB keeps chip awake)
- Check for loose connections or shorts
- Ensure no other peripherals are drawing power

#### Problem: Board resets or crashes
**Solutions:**
- Check power supply is adequate (minimum 250mA capability)
- Add 100µF capacitor between 3V3 and GND
- Verify GPIO6-11 are not used (flash pins)
- Check for proper grounding

### Touch Threshold Calibration

To find the optimal threshold value:

1. Add this to `setup()` before the touch sleep enable:
```cpp
Serial.print("Touch value: ");
Serial.println(touchRead(TOUCH_PIN));
```

2. Monitor serial output:
   - Note the value when **not touching** (e.g., 80)
   - Touch the pad and note the value (e.g., 20)
   - Set `TOUCH_THRESHOLD` to midpoint (e.g., 50)

3. Lower threshold = more sensitive (may trigger accidentally)
4. Higher threshold = less sensitive (may require firm press)

---

## Configuration Checklist

Before deploying your ESP32 WLED remote:

- [ ] WLED MAC address updated (line 36)
- [ ] WiFi channel matches WLED (line 10)
- [ ] Touch threshold calibrated (line 7)
- [ ] Touch pad connected to GPIO4
- [ ] Power supply connected and tested
- [ ] Serial monitor confirms successful operation
- [ ] ESP-NOW messages being delivered successfully
- [ ] Deep sleep working (board powers down after touch)
- [ ] Touch wake-up functioning correctly
- [ ] LED status indicator working (if desired)

---

## Pin Usage Summary

### Single Touch Button Version (esp32_devkit_v1.ino)
| Pin | Function | Connection |
|-----|----------|------------|
| GPIO4 (T0) | Touch Input | Touch pad/sensor |
| GPIO2 | Status LED | Built-in LED (onboard) |
| 3V3 | Power Output | To sensors (if needed) |
| GND | Ground | Common ground |
| USB | Programming/Power | Micro-USB cable |

### Multi-Button Version (esp32_multibutton.ino)
| Pin | Function | Button Label | WLED Command |
|-----|----------|--------------|--------------|
| GPIO4 | Button Input | ON/OFF | Toggle lights |
| GPIO15 | Button Input | Preset 1 | Activate Preset 1 |
| GPIO13 | Button Input | Preset 2 | Activate Preset 2 |
| GPIO12 | Button Input | Preset 3 | Activate Preset 3 |
| GPIO14 | Button Input | Preset 4 | Activate Preset 4 |
| GPIO27 | Button Input | Brightness + | Increase brightness |
| GPIO26 | Button Input | Brightness - | Decrease brightness |
| GPIO25 | Button Input | Night Mode | Activate night mode |
| GPIO2 | Status LED | Built-in LED (onboard) | Visual feedback |
| 3V3 | Power Output | To buttons (if needed) | Button power |
| GND | Ground | Common ground | All buttons |
| USB | Programming/Power | Micro-USB cable | Development |

**Note:** Only RTC GPIO pins (0, 2, 4, 12-15, 25-27, 32-33) can wake ESP32 from deep sleep.

---

## Safety Notes

- Never exceed 3.6V on GPIO pins
- Use voltage regulator if battery voltage > 3.6V
- Avoid shorting pins
- Disconnect power when making wiring changes
- LiPo batteries require careful handling and charging
- Don't use GPIO6-11 (internal flash pins)
- Ensure proper polarity when connecting batteries

---

## Additional Resources

- [ESP32 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)
- [WLED Documentation](https://kno.wled.ge/)
- [ESP-NOW Protocol Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)
- [Touch Sensor Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/touch_pad.html)

---

## Project Photos

*Add your own photos here showing:*
- Assembled hardware
- Touch pad placement
- Enclosure design
- Final installation

---

*Document Version: 1.0*
*Last Updated: 2025-12-10*
*Compatible with: ESP32 Arduino Core 3.x (ESP-IDF 5.x)*
