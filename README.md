# XIAO SSD1306 OLED Library

A **minimal, fully self-contained** SSD1306 128×64 I²C OLED library
optimised for the **Seeed Studio XIAO RP2040**.

No Adafruit_GFX. No U8g2. **Wire only.**

---

## Features

| Feature | Details |
|---|---|
| Protocol | I²C (Wire), 400 kHz |
| Resolution | 128 × 64 px |
| Font | Built-in 5×7 bitmap (ASCII 32–126) |
| Graphics | Pixel, Line (Bresenham), Box, Circle (midpoint algorithm) |
| Bitmap | Direct 1024-byte array blit (`arr()`) |
| Brightness | 0–10 scale → mapped to SSD1306 contrast 0–255 |
| RAM use | 1024-byte frame buffer + small stack |
| Dependencies | `Wire` (built-in) only |

---

## Wiring (XIAO RP2040 → SSD1306)

```
XIAO RP2040   SSD1306 Module
───────────   ──────────────
3.3V      →   VCC
GND       →   GND
D4 (SDA)  →   SDA
D5 (SCL)  →   SCL
```

> The XIAO RP2040's default I²C pins are **D4 = GPIO 6** (SDA)
> and **D5 = GPIO 7** (SCL). `Wire.begin()` selects them automatically.

---

## Installation

### Option A — Arduino IDE Library Manager (manual ZIP install)

1. Zip the `XIAO_SSD1306` folder:
   ```
   XIAO_SSD1306/
   ├── OLED.h
   ├── OLED.cpp
   ├── library.properties
   ├── README.md
   └── examples/
       └── basic_demo/
           └── basic_demo.ino
   ```
2. In Arduino IDE: **Sketch → Include Library → Add .ZIP Library…**
3. Select `XIAO_SSD1306.zip` → click Open.

### Option B — Manual copy

Copy the `XIAO_SSD1306` folder to your Arduino libraries directory:
- Windows: `Documents\Arduino\libraries\`
- macOS / Linux: `~/Arduino/libraries/`

Restart the IDE.

---

## Quick Start

```cpp
#include <OLED.h>

oled o;

void setup() {
  o.begin();               // Initialise I²C + SSD1306

  o.write(0, 0, "Hello"); // Text at pixel (0, 0)
  o.box(10, 10, 50, 30);  // Hollow rectangle
  o.circle(20);            // Circle r=20, centred on display
  o.brightness(7);         // Brightness 0–10
}

void loop() {}
```

---

## API Reference

### `o.begin()`
Initialise Wire at 400 kHz, send SSD1306 startup sequence, clear display.

### `o.clear()`
Erase the frame buffer in RAM. Does **not** push to display — call `o.display()` after.

### `o.display()`
Push the 1024-byte frame buffer to the physical display via I²C.
All drawing functions call this automatically.

### `o.write(x, y, text)`
Render a null-terminated string at pixel position `(x, y)`.
- Each character: **6 px wide** (5 px glyph + 1 px gap), **8 px tall**.
- Characters outside the display boundary are silently clipped.

### `o.box(x1, y1, x2, y2)`
Draw a hollow axis-aligned rectangle. Coordinates are normalised internally.

### `o.circle(radius)`
Draw a hollow circle **centred on the display** with the given radius.

### `o.circle(cx, cy, radius)`
Draw a hollow circle centred at pixel `(cx, cy)`.

### `o.brightness(level)`
Set display contrast. `level` is **0** (off) … **10** (maximum).
Mapped linearly to the SSD1306 contrast register (0x00–0xFF).

### `o.arr(img)`
Blit a **1024-byte** `const uint8_t` array directly to the display.
Array layout matches SSD1306 horizontal-addressing GDDRAM:
```
byte[page * 128 + col], bit 0 = topmost row of the page
```

### `o.pixel(x, y, on)`
Set (on=true) or clear (on=false) one pixel. Call `o.display()` manually.

### `o.line(x0, y0, x1, y1)`
Bresenham line. Call `o.display()` manually.

---

## Generating Bitmaps for `arr()`

1. Draw a 128×64 monochrome image in any editor.
2. Export as **BMP 1-bit** or use an online LCD image converter
   (e.g. **image2cpp** — select "Plain bytes", "Horizontal" layout).
3. Paste the resulting array and pass its pointer to `o.arr()`.

---

## Board Settings (Arduino IDE)

| Setting | Value |
|---|---|
| Board | Seeed XIAO RP2040 |
| Upload Speed | Default |
| USB Stack | Pico SDK (default) |

Install the **Seeed SAMD Boards** or **Arduino-Pico** core for RP2040 support.

---

## License

MIT — use freely, attribution appreciated.
