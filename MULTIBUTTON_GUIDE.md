# ESP32 DEVKIT V1 - WLED Multi-Button Remote
## 8 Physical Buttons Version - Complete Guide

---

## Overview

This version uses 8 physical push buttons to control your WLED lights with different functions. Each button triggers a specific WLED command (ON/OFF, presets, brightness, night mode). The ESP32 stays in deep sleep and wakes when any button is pressed, providing excellent battery life.

**Code File:** [esp32_multibutton.ino](c:\Local\esp32-remote\esp32_multibutton.ino)
**Quick Wiring Reference:** [WIRING_GUIDE.md](c:\Local\esp32-remote\WIRING_GUIDE.md)

---

## Table of Contents
1. [Required Components](#required-components)
2. [Button Functions](#button-functions)
3. [ESP32 Pinout Reference](#esp32-pinout-reference)
4. [Wiring Instructions](#wiring-instructions)
5. [Power Supply Options](#power-supply-options)
6. [Configuration](#configuration)
7. [Testing and Troubleshooting](#testing-and-troubleshooting)
8. [Enclosure Ideas](#enclosure-ideas)

---

## Required Components

### Essential Components
- **1x DOIT ESP32 DEVKIT V1** - Main microcontroller board (~$6-10)
- **8x Momentary Push Buttons** - Normally open (SPST) switches
- **1x USB Cable (Micro-USB)** - For programming and power (~$3)
- **Jumper wires** - For connections (9+ wires: 8 buttons + ground) (~$2-5)
- **Breadboard or PCB** - For prototyping or permanent installation (~$2-5)

### Button Options

**Option 1: 12mm Tactile Switches (Recommended)**
- Cost: ~$0.10-0.20 each ($1-2 for 8)
- PCB mountable
- Compact size
- Good tactile feedback
- Widely available

**Option 2: 6mm Tactile Switches**
- Cost: ~$0.05-0.10 each ($0.50-1 for 8)
- Very compact
- PCB mountable
- Lower profile
- Good for tight spaces

**Option 3: 16mm Arcade Buttons**
- Cost: ~$2-3 each ($16-24 for 8)
- Large and satisfying click
- Panel mountable
- Optional illumination
- Professional feel

**Option 4: Membrane Keypad 4x4**
- Cost: ~$2-5 for 16-button matrix
- Pre-wired, compact
- Use 8 buttons from matrix
- Thin profile
- Easy integration

### Optional Components
- **1x LiPo Battery** (3.7V, 1000-2000mAh) - For portable operation (~$5-10)
- **1x Battery holder/JST connector** (~$2)
- **1x Slide switch** - For power on/off (~$1)
- **1x Project enclosure** - For finished project (~$5-15)
- **Button caps/labels** - For easy identification (~$2-5)
- **TP4056 charging module** - For LiPo charging (~$1-2)

---

## Button Functions

### Default Button Mapping

| Button # | GPIO Pin | Function | WLED Command | Description |
|----------|----------|----------|--------------|-------------|
| 1 | GPIO4  | ON/OFF | Toggle | Turns lights on/off (remembers state) |
| 2 | GPIO15 | Preset 1 | Activate | Loads WLED Preset 1 |
| 3 | GPIO13 | Preset 2 | Activate | Loads WLED Preset 2 |
| 4 | GPIO12 | Preset 3 | Activate | Loads WLED Preset 3 |
| 5 | GPIO14 | Preset 4 | Activate | Loads WLED Preset 4 |
| 6 | GPIO27 | Brightness + | Increase | Increases brightness one step |
| 7 | GPIO26 | Brightness - | Decrease | Decreases brightness one step |
| 8 | GPIO25 | Night Mode | Activate | Activates night mode (dim, warm light) |

### WLED Command Codes

These are the standard WLED remote control codes (from `wled00/remote.cpp`):

```cpp
#define ON 1            // Turn lights on
#define OFF 2           // Turn lights off
#define NIGHT 3         // Night mode
#define BRIGHT_DOWN 8   // Decrease brightness
#define BRIGHT_UP 9     // Increase brightness
#define PRESET_ONE 16   // Load preset 1
#define PRESET_TWO 17   // Load preset 2
#define PRESET_THREE 18 // Load preset 3
#define PRESET_FOUR 19  // Load preset 4
```

**Customization:** You can modify these in the code to suit your needs.

---

## ESP32 Pinout Reference

### DOIT ESP32 DEVKIT V1 - Right Side (Buttons Side)
```
                    ESP32 DEVKIT V1
                   ┌─────────────┐
                   │   USB Port  │
                   └─────────────┘

                               Right Side (Main Button Pins)
                               ┌──────────────┐
                               │ GND          │◄── All buttons
                               │ 23     ◄─────┼── Button 8 (Night)
                               │ 22     ◄─────┼── Button 7 (Bright-)
                               │ TX           │
                               │ RX           │
                               │ 21     ◄─────┼── Button 6 (Bright+)
                               │ 19     ◄─────┼── Button 5 (Preset 4)
                               │ 18     ◄─────┼── Button 4 (Preset 3)
                               │ 5            │
                               │ 17     ◄─────┼── Button 3 (Preset 2)
                               │ 16     ◄─────┼── Button 2 (Preset 1)
                               │ 4      ◄─────┼── Button 1 (ON/OFF)
                               │ 0            │
                               │ 2            │ (Built-in LED)
                               │ 15           │
                               └──────────────┘
```

### Pin Selection Rationale

The pins were chosen because they:
- ✅ Are safe to use (no boot/flash conflicts)
- ✅ Support internal pull-up resistors
- ✅ Support EXT1 wake from deep sleep
- ✅ Are conveniently located on right side
- ✅ Don't interfere with USB/Serial
- ✅ Have no special startup requirements

**Pins to AVOID:**
- ❌ GPIO0 - Boot button (can prevent startup if held LOW)
- ❌ GPIO1, 3 - Serial TX/RX (interferes with Serial Monitor)
- ❌ GPIO6-11 - Flash memory pins (WILL BRICK BOARD!)
- ❌ GPIO34-39 - Input only (no internal pull-ups)

---

## Wiring Instructions

### Basic Wiring Concept

Each button uses a simple configuration:

```
    GPIO Pin ────┬──── [Button] ──── GND
                 │
              (10kΩ pull-up
               internal to ESP32)

When button is pressed: GPIO reads LOW (0V)
When button is released: GPIO reads HIGH (3.3V)
```

**Key Points:**
- ✅ Use internal pull-up resistors (no external resistors needed!)
- ✅ All buttons share common ground
- ✅ Momentary buttons only (normally open)
- ✅ Button polarity doesn't matter (non-polarized)

---

### Step-by-Step Wiring

#### Step 1: Prepare Components
1. Place ESP32 DEVKIT V1 in breadboard center (or prepare for direct wiring)
2. Arrange 8 push buttons near the ESP32
3. Identify GPIO pins on right side of ESP32

#### Step 2: Connect Each Button

**Button 1 (ON/OFF) - GPIO4:**
```
[Button Pin 1] ────→ GPIO4 (right side, row 4)
[Button Pin 2] ────→ GND (top of right side)
```

**Button 2 (Preset 1) - GPIO16:**
```
[Button Pin 1] ────→ GPIO16
[Button Pin 2] ────→ GND
```

**Button 3 (Preset 2) - GPIO17:**
```
[Button Pin 1] ────→ GPIO17
[Button Pin 2] ────→ GND
```

**Button 4 (Preset 3) - GPIO18:**
```
[Button Pin 1] ────→ GPIO18
[Button Pin 2] ────→ GND
```

**Button 5 (Preset 4) - GPIO19:**
```
[Button Pin 1] ────→ GPIO19
[Button Pin 2] ────→ GND
```

**Button 6 (Brightness +) - GPIO21:**
```
[Button Pin 1] ────→ GPIO21
[Button Pin 2] ────→ GND
```

**Button 7 (Brightness -) - GPIO22:**
```
[Button Pin 1] ────→ GPIO22
[Button Pin 2] ────→ GND
```

**Button 8 (Night Mode) - GPIO23:**
```
[Button Pin 1] ────→ GPIO23
[Button Pin 2] ────→ GND
```

#### Step 3: Common Ground (Recommended)

Instead of 8 separate wires to GND:

1. Connect all button ground pins together with one wire
2. Run a single wire from this common point to ESP32 GND
3. This keeps wiring much cleaner

```
              ESP32
         ┌─────────────┐
[BTN1]───┤ 4           │
[BTN2]───┤ 16          │
[BTN3]───┤ 17          │
[BTN4]───┤ 18          │
[BTN5]───┤ 19          │
[BTN6]───┤ 21          │
[BTN7]───┤ 22          │
[BTN8]───┤ 23          │
         │             │
    ┌────┤ GND         │
    │    └─────────────┘
    │
    │ Common Ground Bus
    └─┬─┬─┬─┬─┬─┬─┬─
      │ │ │ │ │ │ │ │
     BTN1-8 (other side)
```

---

### Breadboard Layout

```
                    ESP32 DEVKIT V1
                   (mounted in center)
                         ┃  ┃
    ┏━━━━━━━━━━━━━━━━━━━╋━━╋━━━━━━━━━━━━━━━━━━━┓
    ┃ GROUND RAIL  ━━━━━╋━━╋━━━━━━━━━━━━━━━━━━┫ GND
    ┣━━━━━━━━━━━━━━━━━━━┻━━┻━━━━━━━━━━━━━━━━━━┫
    ┃                                           ┃
    ┃  [BTN1] [BTN2] [BTN3] [BTN4]            ┃
    ┃    4     16     17     18                ┃
    ┃                                           ┃
    ┃  [BTN5] [BTN6] [BTN7] [BTN8]            ┃
    ┃   19     21     22     23                ┃
    ┃                                           ┃
    ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

Top button pins → GPIO pins
Bottom button pins → Ground rail
```

---

### Physical Button Layout Example

For a nice user interface, arrange buttons like this:

```
  ┌─────────────────────────────────────┐
  │        WLED Remote Control          │
  │                                     │
  │   ┌────────┐         ┌────────┐    │
  │   │ON/OFF  │         │Preset 1│    │
  │   │  (4)   │         │  (16)  │    │
  │   └────────┘         └────────┘    │
  │                                     │
  │   ┌────────┐         ┌────────┐    │
  │   │Preset 2│         │Preset 3│    │
  │   │  (17)  │         │  (18)  │    │
  │   └────────┘         └────────┘    │
  │                                     │
  │   ┌────────┐         ┌────────┐    │
  │   │Preset 4│         │ NIGHT  │    │
  │   │  (19)  │         │  (23)  │    │
  │   └────────┘         └────────┘    │
  │                                     │
  │      ┌────┐            ┌────┐      │
  │      │ +  │            │ -  │      │
  │      │(21)│            │(22)│      │
  │      └────┘            └────┘      │
  │     Brightness       Brightness    │
  │                                     │
  │               [●]  Status LED      │
  │                                     │
  └─────────────────────────────────────┘
```

---

## Power Supply Options

### USB Power (Development)
- **Voltage:** 5V from USB
- **Current:** Up to 500mA
- **Use Case:** Programming, debugging, permanent installation
- **Pros:** Unlimited power, no battery maintenance
- **Cons:** Requires cable, not portable

### LiPo Battery (Portable)
- **Voltage:** 3.7V nominal (3.0V - 4.2V range)
- **Capacity:** 1000-2000mAh recommended for multi-button
- **Runtime:** 2-6 months depending on usage
- **Connection:** Battery + to 3V3, Battery - to GND
- **Pros:** Compact, rechargeable, good capacity
- **Cons:** Requires external charging (or add TP4056)

**Wiring:**
```
    LiPo Battery 3.7V
    ┌──────┐
    │  +   │──[Switch]──→ ESP32 3V3
    │  -   │─────────────→ ESP32 GND
    └──────┘
```

### AA Batteries
- **Configuration:** 3x AA in series (4.5V) + voltage regulator
- **Capacity:** ~2000mAh
- **Runtime:** 3-6 months
- **Regulator:** AMS1117-3.3 or LM1117-3.3
- **Pros:** Easy to replace, widely available
- **Cons:** Bulkier, requires voltage regulator

**Wiring:**
```
    3x AA (4.5V)
    ┌──────┐
    │  +   │──[Switch]──→ Regulator IN
    │  -   │─────────────→ Regulator GND
    └──────┘                    │
                                ↓
                         Regulator OUT ──→ ESP32 3V3
                         Regulator GND ──→ ESP32 GND
```

### Power Consumption Estimate

**With 20 button presses per day:**
- Deep sleep: ~10µA × 24h = 0.24mAh/day
- Active (20 presses): 80mA × 0.2s × 20 = 0.09mAh/day
- **Total:** ~0.33mAh/day

**Battery life with 1000mAh battery:** ~3000 days (theoretical)
**Realistic:** 3-6 months (self-discharge, temperature effects)

---

## Configuration

### Step 1: Update WLED MAC Address

Open [esp32_multibutton.ino](c:\Local\esp32-remote\esp32_multibutton.ino) and modify line 70:

```cpp
// Get MAC address from WLED: Config → WiFi Setup → AP MAC
const uint8_t macAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
```

**Format:** AA:BB:CC:DD:EE:FF → `{0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}`

---

### Step 2: Set WiFi Channel

Modify line 10:

```cpp
#define CHANNEL 1  // Match your WLED WiFi channel (1-13)
```

---

### Step 3: Customize Button Functions (Optional)

To change what each button does, modify lines 19-26:

```cpp
ButtonConfig buttons[] = {
  {BTN_ON_OFF,    0,            "ON/OFF",       true},
  {BTN_PRESET1,   PRESET_ONE,   "Preset 1",     false},
  {BTN_PRESET2,   PRESET_TWO,   "Preset 2",     false},
  // Modify these as needed...
};
```

**Example: Change Button 8 to Preset 5:**
```cpp
{BTN_NIGHT,     20,  "Preset 5",  false}  // 20 = PRESET_FIVE
```

---

### Step 4: Remap Pins (Optional)

To use different GPIO pins, modify lines 14-21:

```cpp
#define BTN_ON_OFF    4   // Change to different GPIO
#define BTN_PRESET1   16  // Change as needed
// etc...
```

**Remember:** Avoid GPIO 0, 1, 3, 6-11, 34-39

---

## Testing and Troubleshooting

### Initial Testing

**Step 1: Upload Code**
1. Connect ESP32 via USB
2. Open Arduino IDE
3. Board: "ESP32 Dev Module"
4. Port: Your COM port
5. Upload [esp32_multibutton.ino](c:\Local\esp32-remote\esp32_multibutton.ino)

**Step 2: Open Serial Monitor (115200 baud)**

You should see:
```
=== ESP32 DEVKIT V1 WLED Multi-Button Remote ===
MAC Address: XX:XX:XX:XX:XX:XX

Initializing buttons:
  - GPIO4: ON/OFF
  - GPIO16: Preset 1
  - GPIO17: Preset 2
  ...
ESP-NOW Initialized Successfully

Waiting for button press...
```

**Step 3: Test Each Button**

Press each button and verify:
- Serial shows: "Button Pressed: [Name]"
- Serial shows: "Sending command: [#]"
- LED blinks twice
- "Delivery Status: Success" (if WLED connected)
- "Button released"
- "Entering deep sleep..."

---

### Troubleshooting Guide

#### Problem: No button detected
**Check:**
- ✓ Buttons properly connected to GPIO pins?
- ✓ GND connected to ESP32 GND?
- ✓ Using normally-open (NO) buttons?
- ✓ Buttons not damaged/stuck?

**Test:**
Add to code after button initialization:
```cpp
for (int i = 0; i < NUM_BUTTONS; i++) {
  Serial.print("GPIO");
  Serial.print(buttons[i].pin);
  Serial.print(": ");
  Serial.println(digitalRead(buttons[i].pin) ? "HIGH" : "LOW");
}
```
All should show HIGH when not pressed.

---

#### Problem: Wrong button detected
**Check:**
- ✓ GPIO pin numbers match code?
- ✓ Wires not crossed?

**Solution:**
Either rewire buttons or change pin definitions in code.

---

#### Problem: Multiple buttons trigger at once
**Check:**
- ✓ Wires not touching/shorting?
- ✓ Buttons not mechanically linked?
- ✓ Breadboard connections solid?

**Solution:**
- Check for shorts
- Verify button quality
- Add debounce delay if needed (line 7)

---

#### Problem: Button doesn't wake from sleep
**Check:**
- ✓ All button pins in the wake-up mask?
- ✓ Button connected to valid GPIO (not 34-39)?

**Verify wake-up mask:**
Code automatically includes all buttons in the mask (lines 287-294).

---

#### Problem: ESP-NOW messages fail
**Solutions:**
- ✓ Verify WLED MAC address
- ✓ Check WiFi channel match
- ✓ Ensure WLED in range (~50m indoors)
- ✓ Enable ESP-NOW in WLED settings
- ✓ Restart WLED device

---

#### Problem: High power consumption
**Check:**
- ✓ Board enters deep sleep?
- ✓ Disconnect USB when testing battery
- ✓ No LEDs or other peripherals drawing power
- ✓ Measure: should be ~10µA in sleep

---

#### Problem: Buttons trigger randomly
**Check:**
- ✓ Loose connections
- ✓ Long wire runs (keep under 30cm)
- ✓ Electromagnetic interference

**Solution:**
- Shorten wires
- Add 0.1µF capacitor across each button
- Increase debounce delay

---

## Enclosure Ideas

### 3D Printed Enclosure
- Design with 8 button holes
- Battery compartment underneath
- USB port access on side
- Label button functions
- STL files available online

### Project Box Conversion
- Plastic project box (10cm x 15cm)
- Drill 8 button holes
- Mount ESP32 with standoffs
- Label with permanent marker or stickers
- Add rubber feet

### Professional Panel Mount
- Metal or acrylic front panel
- 16mm arcade buttons (panel mount)
- Enclosure for electronics
- Printed labels or engraving
- Optional: add button illumination

### Minimal Breadboard Build
- Keep on breadboard
- 3D print button caps with labels
- Acrylic base plate
- Clear acrylic top cover
- Quick and functional

---

## Advanced Customization

### Add More Buttons
1. Choose available GPIO pins (not 0, 1, 3, 6-11, 34-39)
2. Add to button configuration array
3. Compile and upload

### Change Button Functions
Modify the `ButtonConfig buttons[]` array to assign different WLED commands.

### Add Button Combinations
Detect multiple buttons pressed simultaneously for additional functions.

### LED Indicators
- Use GPIO2 (built-in LED) for status
- Add external LEDs for button state indication
- Illuminate arcade buttons

### Battery Monitoring
Add code to read battery voltage and warn when low:
```cpp
int batteryVoltage = analogRead(35) * 2; // Example for voltage divider
```

---

## Pin Usage Summary

| GPIO | Function | WLED Command | Notes |
|------|----------|--------------|-------|
| 4 | Button 1 | ON/OFF Toggle | State tracked |
| 15 | Button 2 | Preset 1 | RTC GPIO |
| 13 | Button 3 | Preset 2 | RTC GPIO |
| 12 | Button 4 | Preset 3 | RTC GPIO (caution) |
| 14 | Button 5 | Preset 4 | RTC GPIO |
| 27 | Button 6 | Brightness + | RTC GPIO |
| 26 | Button 7 | Brightness - | RTC GPIO |
| 25 | Button 8 | Night Mode | RTC GPIO |
| 2 | Status LED | Built-in | Visual feedback |
| GND | Ground | All buttons | Common ground |

---

## Shopping List

- [ ] ESP32 DEVKIT V1 (~$6-10)
- [ ] 8x Momentary buttons (~$2-24 depending on type)
- [ ] Breadboard or PCB (~$2-5)
- [ ] Jumper wires pack (~$2-5)
- [ ] USB cable Micro-USB (~$3)
- [ ] Optional: LiPo 1000-2000mAh (~$5-10)
- [ ] Optional: TP4056 charging module (~$1-2)
- [ ] Optional: Enclosure (~$5-15)
- [ ] Optional: Button labels/caps (~$2-5)

**Total Cost:** ~$25-75 depending on components

---

## Quick Reference Card

```
┌────────────────────────────────────────┐
│   ESP32 WLED Multi-Button Remote      │
├────────────────────────────────────────┤
│ GPIO4  → BTN1 → ON/OFF                │
│ GPIO16 → BTN2 → Preset 1              │
│ GPIO17 → BTN3 → Preset 2              │
│ GPIO18 → BTN4 → Preset 3              │
│ GPIO19 → BTN5 → Preset 4              │
│ GPIO21 → BTN6 → Brightness +          │
│ GPIO22 → BTN7 → Brightness -          │
│ GPIO23 → BTN8 → Night Mode            │
│ GND    → All buttons (other side)     │
├────────────────────────────────────────┤
│ Internal pull-ups enabled             │
│ Active LOW (0V when pressed)          │
│ Wakes from deep sleep on any button   │
└────────────────────────────────────────┘
```

---

## Resources

- [ESP32 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)
- [WLED Documentation](https://kno.wled.ge/)
- [WLED Remote Control Codes](https://github.com/Aircoookie/WLED/blob/main/wled00/remote.cpp)
- [ESP-NOW Protocol](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)

---

*Document Version: 1.0*
*Last Updated: 2025-12-10*
*Code File: esp32_multibutton.ino*
*Quick Wiring: WIRING_GUIDE.md*
*Compatible with: ESP32 Arduino Core 3.x (ESP-IDF 5.x)*
