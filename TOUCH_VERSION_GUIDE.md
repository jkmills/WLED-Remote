# ESP32 DEVKIT V1 - WLED Touch Remote
## Single Touch Sensor Version - Complete Guide

---

## Overview

This version uses a single capacitive touch sensor to toggle your WLED lights on and off. The ESP32 stays in deep sleep mode (consuming only ~10µA) and wakes up when you touch the sensor, making it perfect for battery-powered operation.

**Code File:** [esp32_devkit_v1.ino](c:\Local\esp32-remote\esp32_devkit_v1.ino)

---

## Table of Contents
1. [Required Components](#required-components)
2. [ESP32 Pinout Reference](#esp32-pinout-reference)
3. [Touch Sensor Options](#touch-sensor-options)
4. [Wiring Diagrams](#wiring-diagrams)
5. [Power Supply Options](#power-supply-options)
6. [Configuration](#configuration)
7. [Testing and Troubleshooting](#testing-and-troubleshooting)

---

## Required Components

### Essential Components
- **1x DOIT ESP32 DEVKIT V1** - Main microcontroller board
- **1x USB Cable (Micro-USB)** - For programming and power
- **Touch sensor or conductive material** - For touch input (see options below)
- **Jumper wires** - For connections (2-3 wires)

### Optional Components
- **1x LiPo Battery** (3.7V, 500mAh - 2000mAh) - For portable operation
- **1x Slide switch** - For power on/off
- **Enclosure/Case** - For finished project
- **Copper tape or aluminum foil** - DIY touch pad option
- **3x AA battery holder** with voltage regulator - Alternative power source

---

## ESP32 Pinout Reference

### DOIT ESP32 DEVKIT V1 Layout
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
 │ GND          │ Ground           │         2    │ GPIO2 (LED)
 │ 13           │ GPIO13 (T4)      │         15   │ GPIO15 (T3)
 │ D2 (GPIO9)   │ Flash (avoid)    │         D1   │ GPIO8 (Flash)
 │ D3 (GPIO10)  │ Flash (avoid)    │         D0   │ GPIO7 (Flash)
 │ CMD (GPIO11) │ Flash (avoid)    │         CLK  │ GPIO6 (Flash)
 │ 5V           │ 5V In/Out        │         GND  │ Ground
 └──────────────┘                  └──────────────┘

⭐ = GPIO4 (T0) - Used for touch sensor in this project
```

### Touch-Capable GPIO Pins
The ESP32 has 10 capacitive touch-sensitive pins. This project uses **T0 (GPIO4)**:

| Touch Pin | GPIO | Notes |
|-----------|------|-------|
| **T0** | **GPIO4** | **Used in this project** ⭐ |
| T1 | GPIO0 | Connected to BOOT button |
| T2 | GPIO2 | Built-in LED |
| T3 | GPIO15 | Available |
| T4 | GPIO13 | Available |
| T5 | GPIO12 | Available |
| T6 | GPIO14 | Available |
| T7 | GPIO27 | Available |
| T8 | GPIO33 | Available |
| T9 | GPIO32 | Available |

**Why GPIO4?** It's easily accessible on the right side of the board and has no special boot/startup requirements.

---

## Touch Sensor Options

### Option 1: Direct Touch Pad (Recommended)
**Simplest and lowest power consumption**

**Materials:**
- Copper tape, aluminum foil, or conductive fabric
- One jumper wire
- Tape or adhesive

**Setup:**
1. Cut conductive material to desired size (2cm x 2cm minimum, larger = more sensitive)
2. Connect one jumper wire from GPIO4 to the conductive material
3. Secure with tape or adhesive
4. Optionally, cover with thin non-conductive material (paper, thin plastic) for aesthetics

**Wiring:**
```
ESP32 DEVKIT V1          Touch Pad
┌─────────────┐          ┌─────────┐
│             │          │         │
│ GPIO4 (T0)  ├──────────┤  Copper │
│             │          │   Tape  │
│             │          │         │
└─────────────┘          └─────────┘

No connection to GND needed!
Capacitive touch works through body capacitance
```

**Sensitivity Adjustment:**
- **Increase threshold** (line 7 in code) → Less sensitive, requires firmer touch
- **Decrease threshold** → More sensitive, may trigger accidentally
- **Default:** 40 (good starting point)

---

### Option 2: TTP223 Capacitive Touch Module
**Pre-made module with better noise immunity**

**Specifications:**
- Operating Voltage: 2.0V - 5.5V
- Standby Current: 1.5µA
- Response Time: 60ms - 220ms
- Includes built-in touch pad

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

**Note:** TTP223 outputs HIGH when touched, so you'll need to modify the code to use `digitalRead()` instead of `touchRead()` and adjust the wake-up method.

---

### Option 3: DIY Metal Touch Surface
**Use any conductive metal object**

**Examples:**
- Metal pushpin or thumbtack
- Coin taped/glued down
- Metal screw or bolt
- Bare copper wire (coiled or straight)
- Metal plate or washer

**Setup:**
Simply connect a jumper wire from GPIO4 to any exposed metal surface. The ESP32's capacitive touch sensor will detect when you touch the metal.

---

## Wiring Diagrams

### Minimal Setup (USB Powered)
```
┌─────────────────────────────────────┐
│         ESP32 DEVKIT V1             │
│                                     │
│  GPIO4 (T0) ──── [Touch Pad]       │
│                                     │
│  GPIO2 ──────── (Built-in LED)     │
│                                     │
│  USB Port ───── [USB Cable]        │
│                 to Computer/Charger │
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

**LiPo Battery Notes:**
- Use 3V3 pin (not 5V) when powered by 3.7V LiPo
- Voltage range: 3.0V (empty) to 4.2V (full)
- Deep sleep current: ~10µA
- Battery life: Months with occasional use
- No built-in charging circuit - charge externally or add TP4056 module

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
      │   └──┬───────────┘
      │      │
┌─────┼──────┼─────────────────┐
│     │      │                 │
│  5V │   3V3◄─ (Use this)     │
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
- **Pros:** Unlimited power, easy development
- **Cons:** Requires cable, not portable

### LiPo Battery (Recommended for Portable)
- **Voltage:** 3.7V nominal (3.0V - 4.2V range)
- **Capacity:** 500mAh - 2000mAh recommended
- **Runtime:** 3-12 months depending on usage
- **Connection:** Battery + to 3V3, Battery - to GND
- **Charging:** External charger or add TP4056 module
- **Pros:** Compact, lightweight, long runtime
- **Cons:** Requires external charging

### AA Batteries
- **Configuration:** 3x AA in series (4.5V) with voltage regulator
- **Capacity:** ~2000mAh typical
- **Runtime:** 6-12 months with occasional use
- **Voltage Regulator:** AMS1117-3.3 or LM1117-3.3
- **Pros:** Easy to replace, widely available
- **Cons:** Bulkier than LiPo

### Power Consumption
- **Active Mode:** ~80mA (WiFi on, transmitting)
- **Deep Sleep:** ~10µA (99.9% of the time)
- **Active Time per Touch:** ~200ms
- **Estimated Battery Life (1000mAh battery, 10 touches/day):**
  - Daily consumption: ~10µA × 24h + (80mA × 0.2s × 10) ≈ 0.24mAh
  - Battery life: 1000mAh / 0.24mAh ≈ 4,000 days (theoretical)
  - Realistic: 3-6 months (accounting for self-discharge)

---

## Configuration

### Step 1: Update WLED MAC Address
Open [esp32_devkit_v1.ino](c:\Local\esp32-remote\esp32_devkit_v1.ino) and modify line 37:

```cpp
// Find your WLED device's MAC address in WLED settings
const uint8_t macAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
```

**How to find WLED MAC address:**
1. Open WLED web interface
2. Go to Config → WiFi Setup
3. Look for "AP MAC" or "STA MAC"
4. Format: AA:BB:CC:DD:EE:FF → {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}

### Step 2: Set WiFi Channel
Modify line 10 to match your WLED device's WiFi channel:

```cpp
#define CHANNEL 1  // Change to match WLED WiFi channel (1-13)
```

**How to find WiFi channel:**
1. Check your router settings
2. Or use WiFi analyzer app on phone
3. WLED typically uses the same channel as your WiFi router

### Step 3: Adjust Touch Threshold (Optional)
Modify line 7 if needed:

```cpp
#define TOUCH_THRESHOLD 40  // Lower = more sensitive, Higher = less sensitive
```

**Calibration:**
1. Upload code and open Serial Monitor (115200 baud)
2. Add this line in `setup()` before `touchSleepWakeUpEnable()`:
   ```cpp
   Serial.println(touchRead(TOUCH_PIN));
   ```
3. Note the value when NOT touching (e.g., 80)
4. Touch and note the value (e.g., 20)
5. Set threshold to midpoint (e.g., 50)

---

## Testing and Troubleshooting

### Initial Upload and Testing

**Step 1: Upload Code**
1. Connect ESP32 via USB
2. Open Arduino IDE
3. Select Board: "ESP32 Dev Module"
4. Select Port: Your COM port
5. Upload [esp32_devkit_v1.ino](c:\Local\esp32-remote\esp32_devkit_v1.ino)

**Step 2: Open Serial Monitor**
1. Set baud rate to 115200
2. You should see:
   ```
   === ESP32 DEVKIT V1 WLED Remote ===
   MAC Address: XX:XX:XX:XX:XX:XX
   ESP-NOW Initialized Successfully
   Current light state: OFF
   Sending: Light ON
   Entering deep sleep...
   ```

**Step 3: Test Touch Sensor**
1. Touch the sensor pad on GPIO4
2. Board wakes up immediately
3. Serial output shows activity
4. Built-in LED lights up or turns off
5. Board returns to deep sleep after 200ms

**Step 4: Verify ESP-NOW Communication**
1. Check serial output for "Delivery Status: Success"
2. If you see "Fail. Retrying", check MAC address and channel
3. Ensure WLED device is powered on and in range

---

### Common Issues and Solutions

#### Problem: Touch not detected
**Solutions:**
- ✓ Increase touch pad size (larger = more sensitive)
- ✓ Lower `TOUCH_THRESHOLD` value in code
- ✓ Check wire connection to GPIO4
- ✓ Touch with more skin contact area (use finger, not fingernail)
- ✓ Remove any thick insulation over touch pad

**Diagnosis:**
Add to code and monitor touch values:
```cpp
Serial.print("Touch value: ");
Serial.println(touchRead(TOUCH_PIN));
```

---

#### Problem: False triggers (activates without touch)
**Solutions:**
- ✓ Increase `TOUCH_THRESHOLD` value (try 50, 60, 70...)
- ✓ Shield touch pad from electromagnetic interference
- ✓ Add ground plane around touch sensor
- ✓ Use shorter wire connections (< 30cm)
- ✓ Keep away from power supplies and motors

---

#### Problem: ESP-NOW messages fail
**Solutions:**
- ✓ Verify WLED MAC address is correct
- ✓ Ensure WiFi channel matches WLED
- ✓ Check distance to WLED device (max ~200m open air, less through walls)
- ✓ Verify WLED has ESP-NOW remote enabled
- ✓ Restart WLED device

**Check WLED ESP-NOW settings:**
1. WLED Web Interface → Config → Sync Interfaces
2. Enable "ESP-NOW Remote"

---

#### Problem: Won't wake from deep sleep
**Solutions:**
- ✓ Verify touch sensor connected to GPIO4 (T0)
- ✓ Check that `touchSleepWakeUpEnable()` is called
- ✓ Ensure power supply is stable (3.0V - 3.6V)
- ✓ Try pressing BOOT button, then touch sensor
- ✓ Check touch threshold isn't too high

---

#### Problem: High power consumption
**Solutions:**
- ✓ Verify board enters deep sleep (serial output stops)
- ✓ Disconnect USB during battery testing (USB keeps chip partially awake)
- ✓ Check for loose connections or shorts
- ✓ Ensure no other peripherals drawing power
- ✓ Measure current: should be ~10µA in deep sleep

---

#### Problem: Board resets or crashes
**Solutions:**
- ✓ Check power supply provides adequate current (250mA minimum)
- ✓ Add 100µF capacitor between 3V3 and GND
- ✓ Verify not using GPIO6-11 (flash pins)
- ✓ Check for proper grounding
- ✓ Ensure stable power during WiFi transmission

---

### Touch Threshold Calibration Procedure

**Add this code to `setup()` for calibration:**

```cpp
// Add before touchSleepWakeUpEnable()
Serial.println("\n=== Touch Calibration ===");
Serial.println("Reading touch values for 10 seconds...");
Serial.println("First 5 sec: DON'T touch");
Serial.println("Last 5 sec: TOUCH the sensor");

for (int i = 0; i < 100; i++) {
  Serial.print("Touch value: ");
  Serial.println(touchRead(TOUCH_PIN));
  delay(100);
}

Serial.println("=== Calibration Complete ===");
Serial.println("Set TOUCH_THRESHOLD to a value between the min and max observed");
```

**Results interpretation:**
- Not touching: Values around 60-90 (varies by sensor size)
- Touching: Values around 10-30
- Set threshold: Midpoint (e.g., 40-50)

---

## Pin Usage Summary

| Pin | Function | Connection | Notes |
|-----|----------|------------|-------|
| GPIO4 (T0) | Touch Input | Touch pad/sensor | Capacitive touch |
| GPIO2 | Status LED | Built-in LED (onboard) | Visual feedback |
| 3V3 | Power Output | Battery + (if used) | 3.3V regulated |
| GND | Ground | Battery - / Common ground | |
| USB | Programming/Power | Micro-USB cable | 5V input |

---

## Configuration Checklist

Before deploying:

- [ ] WLED MAC address updated (line 37)
- [ ] WiFi channel matches WLED (line 10)
- [ ] Touch threshold calibrated (line 7)
- [ ] Touch pad connected to GPIO4
- [ ] Power supply connected and tested
- [ ] Serial monitor confirms successful operation
- [ ] ESP-NOW messages delivered successfully
- [ ] Deep sleep working (board powers down)
- [ ] Touch wake-up functioning correctly
- [ ] Battery life tested (if portable)

---

## Code Features

### Deep Sleep Mode
- Consumes only ~10µA when sleeping
- Wakes instantly on touch (< 100ms)
- RTC memory preserves ON/OFF state
- Sequence number persists across sleep cycles

### State Tracking
- Remembers if lights are ON or OFF
- Toggles state on each touch
- State preserved through deep sleep
- Built-in LED reflects current state

### Retry Mechanism
- Automatically retries failed transmissions
- Up to 25 retries per message
- Detailed error reporting via serial

### Status Indicators
- Built-in LED shows light state
- Serial output for debugging
- ESP-NOW delivery confirmation
- Detailed error messages

---

## Enclosure Ideas

### 3D Printed Case
- Design with opening for touch pad
- Battery compartment if portable
- USB port access for programming

### Project Box
- Plastic enclosure (5cm x 5cm recommended)
- Drill hole for touch wire/pad
- Mount ESP32 with standoffs
- Add switch for power

### Minimal Design
- Just ESP32 + touch pad
- Use decorative top plate
- Hide battery underneath
- Mount with double-sided tape

---

## Advanced Modifications

### Add Multiple Touch Sensors
Modify code to use different touch pins (T1-T9) for different functions.

### Add Button Functions
Combine touch sensor with traditional buttons for more control options.

### LED Brightness Indicator
Use external LEDs to show different states or brightness levels.

### Battery Level Monitoring
Add code to read battery voltage and indicate low battery.

---

## Safety Notes

- Never exceed 3.6V on GPIO pins
- Use voltage regulator if battery voltage > 3.6V
- Avoid shorting pins
- Disconnect power when making wiring changes
- LiPo batteries require careful handling
- Don't use GPIO6-11 (internal flash pins)
- Ensure proper polarity when connecting batteries

---

## Shopping List

- [ ] ESP32 DEVKIT V1 (~$6-10)
- [ ] USB cable Micro-USB (~$3)
- [ ] Copper tape or aluminum foil (~$3-5)
- [ ] Jumper wires (~$2-5)
- [ ] Optional: LiPo battery 3.7V 500-2000mAh (~$5-10)
- [ ] Optional: Battery holder/connector (~$2)
- [ ] Optional: Slide switch (~$1)
- [ ] Optional: Enclosure (~$5-15)

**Total Cost:** ~$20-50 depending on options

---

## Resources

- [ESP32 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)
- [WLED Documentation](https://kno.wled.ge/)
- [ESP-NOW Protocol](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)
- [Touch Sensor Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/touch_pad.html)

---

*Document Version: 1.0*
*Last Updated: 2025-12-10*
*Code File: esp32_devkit_v1.ino*
*Compatible with: ESP32 Arduino Core 3.x (ESP-IDF 5.x)*
