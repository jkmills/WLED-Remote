# ESP32 Multi-Button WLED Remote - Quick Wiring Guide

## Button Connections (All 8 Buttons)

Each button connects between a GPIO pin and GND. The ESP32's internal pull-up resistors are used (no external resistors needed).

### Wiring Table

| Button # | GPIO Pin | WLED Function | Wire 1 (Button) | Wire 2 (Button) |
|----------|----------|---------------|-----------------|-----------------|
| 1 | GPIO4  | ON/OFF Toggle    | To GPIO4  | To GND |
| 2 | GPIO15 | Preset 1         | To GPIO15 | To GND |
| 3 | GPIO13 | Preset 2         | To GPIO13 | To GND |
| 4 | GPIO12 | Preset 3         | To GPIO12 | To GND |
| 5 | GPIO14 | Preset 4         | To GPIO14 | To GND |
| 6 | GPIO27 | Brightness Up    | To GPIO27 | To GND |
| 7 | GPIO26 | Brightness Down  | To GPIO26 | To GND |
| 8 | GPIO25 | Night Mode       | To GPIO25 | To GND |

### Common Ground Connection

All buttons share the same ground. You can:
- **Option 1:** Run a separate wire from each button to GND pin
- **Option 2:** Connect all button grounds together, then one wire to ESP32 GND
- **Option 3:** Use a breadboard ground rail for easy connection

## Visual Wiring Diagram

```
ESP32 DEVKIT V1 Pinout:

Left Side:                      Right Side:
┌────────────┐                  ┌────────────┐
│ 3V3        │                  │ GND        │◄─── All buttons connect here
│ EN         │                  │ 23   ◄─────┼─── Button 8 (Night)
│ VP         │                  │ 22   ◄─────┼─── Button 7 (Bright -)
│ VN         │                  │ TX         │
│ 34         │                  │ RX         │
│ 35         │                  │ 21   ◄─────┼─── Button 6 (Bright +)
│ 32         │                  │ 19   ◄─────┼─── Button 5 (Preset 4)
│ 33         │                  │ 18   ◄─────┼─── Button 4 (Preset 3)
│ 25         │                  │ 5          │
│ 26         │                  │ 17   ◄─────┼─── Button 3 (Preset 2)
│ 27         │                  │ 16   ◄─────┼─── Button 2 (Preset 1)
│ 14         │                  │ 4    ◄─────┼─── Button 1 (ON/OFF)
│ 12         │                  │ 0          │
│ GND        │◄───┐             │ 2          │ (Built-in LED)
│ 13         │    │             │ 15         │
│ D2         │    │             │ D1         │
│ D3         │    └─ Common Ground Rail
│ CMD        │
│ 5V         │
└────────────┘
```

## Breadboard Layout Example

```
                    ESP32 DEVKIT V1
                   (mounted in center)
                         ┃  ┃
    ┏━━━━━━━━━━━━━━━━━━━╋━━╋━━━━━━━━━━━━━━━━━━━┓
    ┃ GROUND RAIL  ━━━━━╋━━╋━━━━━━━━━━━━━━━━━━┫ GND
    ┣━━━━━━━━━━━━━━━━━━━┻━━┻━━━━━━━━━━━━━━━━━━┫
    ┃                                           ┃
    ┃  [BTN1] [BTN2] [BTN3] [BTN4]            ┃
    ┃    │      │      │      │                ┃
    ┃    4     16     17     18                ┃
    ┃                                           ┃
    ┃  [BTN5] [BTN6] [BTN7] [BTN8]            ┃
    ┃    │      │      │      │                ┃
    ┃   19     21     22     23                ┃
    ┃                                           ┃
    ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

All button bottom pins connect to ground rail
All button top pins connect to their respective GPIO
```

## Step-by-Step Wiring Instructions

### Step 1: Prepare the ESP32
1. Place ESP32 DEVKIT V1 in the center of breadboard (or have it ready for direct wiring)
2. Ensure it's properly seated with pins accessible

### Step 2: Identify the Pins
Look at your ESP32 and locate:
- **Right side:** GPIO23, 22, 21, 19, 18, 17, 16, 4, and GND (top pin)
- These are the pins we'll use

### Step 3: Connect Buttons One by One

**Button 1 (ON/OFF):**
1. Take a momentary push button
2. Connect one side to GPIO4 (right side of board)
3. Connect other side to GND

**Button 2 (Preset 1):**
1. Connect one side to GPIO16
2. Connect other side to GND

**Button 3 (Preset 2):**
1. Connect one side to GPIO17
2. Connect other side to GND

**Button 4 (Preset 3):**
1. Connect one side to GPIO18
2. Connect other side to GND

**Button 5 (Preset 4):**
1. Connect one side to GPIO19
2. Connect other side to GND

**Button 6 (Brightness Up):**
1. Connect one side to GPIO21
2. Connect other side to GND

**Button 7 (Brightness Down):**
1. Connect one side to GPIO22
2. Connect other side to GND

**Button 8 (Night Mode):**
1. Connect one side to GPIO23
2. Connect other side to GND

### Step 4: Common Ground (Recommended)
Instead of 8 separate wires to GND:
1. Connect all button ground pins together with a wire
2. Run ONE wire from this common point to ESP32 GND
3. This keeps wiring cleaner

### Step 5: Power Connection
- Connect USB cable to ESP32 for power and programming
- OR connect battery to 3V3 and GND for portable operation

## Simplified Wiring (Common Ground Bus)

```
                ESP32
           ┌─────────────┐
[BTN1]─────┤ 4           │
[BTN2]─────┤ 16          │
[BTN3]─────┤ 17          │
[BTN4]─────┤ 18          │
[BTN5]─────┤ 19          │
[BTN6]─────┤ 21          │
[BTN7]─────┤ 22          │
[BTN8]─────┤ 23          │
           │             │
  ┌────────┤ GND         │
  │        └─────────────┘
  │
  │  Common Ground Bus
  └──┬──┬──┬──┬──┬──┬──┬──
     │  │  │  │  │  │  │  │
    BTN1 BTN2 ... BTN8
  (other side of each button)
```

## Testing Each Button

After wiring, upload the code and open Serial Monitor (115200 baud):

1. **Press Button 1 (GPIO4):** Should see "Button Pressed: ON/OFF"
2. **Press Button 2 (GPIO16):** Should see "Button Pressed: Preset 1"
3. **Press Button 3 (GPIO17):** Should see "Button Pressed: Preset 2"
4. Continue testing all buttons...

If a button doesn't work:
- Check the connection to the GPIO pin
- Check the ground connection
- Ensure button is normally-open (NO) type
- Try pressing firmly

## Troubleshooting

### No buttons detected
- **Check:** All buttons properly connected?
- **Check:** Is GND connected?
- **Check:** Using normally-open (NO) buttons?

### Wrong button detected
- **Check:** GPIO pin numbers match the code
- **Solution:** Either rewire or change pin numbers in code

### Multiple buttons trigger at once
- **Check:** Wires not touching/shorting
- **Check:** Buttons not mechanically stuck
- **Solution:** Add small delays, check debounce setting

### Button triggers randomly
- **Check:** Loose connections
- **Check:** Long wire runs (keep under 30cm if possible)
- **Solution:** Add 0.1µF capacitor across button if needed

## Advanced: Using Dupont Connectors

For a cleaner permanent installation:

1. **Strip and crimp:** Use Dupont connectors on wires
2. **Label wires:** Use tape or labels (GPIO4, GPIO16, etc.)
3. **Bundle ground wires:** Use a small terminal block for common ground
4. **Secure connections:** Hot glue or electrical tape for strain relief

## Pin Selection Notes

The pins were chosen because they:
- ✅ Are safe to use (no boot/flash conflicts)
- ✅ Have no special startup requirements
- ✅ Support EXT1 wake from deep sleep
- ✅ Are conveniently located on the right side
- ✅ Don't interfere with USB/Serial

**Pins to AVOID (not used in this project):**
- ❌ GPIO0 - Boot button, can interfere with startup
- ❌ GPIO1, 3 - Serial TX/RX
- ❌ GPIO6-11 - Flash memory pins (will brick board!)
- ❌ GPIO34-39 - Input only, can't use internal pull-ups

## Shopping List

For a complete build you need:

- [ ] 1x ESP32 DEVKIT V1 (~$6-10)
- [ ] 8x Momentary push buttons (~$2-8 depending on style)
- [ ] 1x Breadboard OR custom PCB (~$2-5)
- [ ] Jumper wires pack (~$5)
- [ ] USB cable Micro-USB (~$3)
- [ ] Optional: Enclosure/case (~$5-15)
- [ ] Optional: Battery + holder (~$5-10)

**Total Cost:** ~$25-50 depending on components chosen

## Quick Reference Card

Print this and keep near your workspace:

```
┌────────────────────────────────────────┐
│   ESP32 WLED Remote - Quick Ref       │
├────────────────────────────────────────┤
│ GPIO4  → Button 1 → ON/OFF            │
│ GPIO15 → Button 2 → Preset 1          │
│ GPIO13 → Button 3 → Preset 2          │
│ GPIO12 → Button 4 → Preset 3          │
│ GPIO14 → Button 5 → Preset 4          │
│ GPIO27 → Button 6 → Brightness Up     │
│ GPIO26 → Button 7 → Brightness Down   │
│ GPIO25 → Button 8 → Night Mode        │
│ GND    → All buttons (other side)     │
├────────────────────────────────────────┤
│ All buttons: One side to GPIO,        │
│              Other side to GND         │
│ Pull-ups: Internal (no resistor)      │
│ Active: LOW (0V when pressed)         │
│ NOTE: Only RTC GPIO pins support      │
│       deep sleep wake-up!              │
└────────────────────────────────────────┘
```

---

**Document Version:** 1.0
**Last Updated:** 2025-12-10
**Compatible with:** esp32_multibutton.ino
