/**
 * ============================================================
 *  OLED.cpp — SSD1306 128×64 I²C Library Implementation
 *  Optimised for Seeed Studio XIAO RP2040
 * ============================================================
 */

#include "OLED.h"

/* ============================================================
 *  5×7 Bitmap Font  (ASCII 32 ' ' … 126 '~')
 *
 *  Layout: each entry = 5 column bytes.
 *  Bit 0 = top pixel, bit 6 = bottom pixel of that column.
 *  Stored as a plain const array — RP2040 XIP flash is
 *  byte-addressable, so no PROGMEM tricks are needed.
 * ============================================================ */
static const uint8_t FONT[95][5] = {
    { 0x00,0x00,0x00,0x00,0x00 }, // 32  (space)
    { 0x00,0x00,0x5F,0x00,0x00 }, // 33  !
    { 0x00,0x07,0x00,0x07,0x00 }, // 34  "
    { 0x14,0x7F,0x14,0x7F,0x14 }, // 35  #
    { 0x24,0x2A,0x7F,0x2A,0x12 }, // 36  $
    { 0x23,0x13,0x08,0x64,0x62 }, // 37  %
    { 0x36,0x49,0x55,0x22,0x50 }, // 38  &
    { 0x00,0x05,0x03,0x00,0x00 }, // 39  '
    { 0x00,0x1C,0x22,0x41,0x00 }, // 40  (
    { 0x00,0x41,0x22,0x1C,0x00 }, // 41  )
    { 0x08,0x2A,0x1C,0x2A,0x08 }, // 42  *
    { 0x08,0x08,0x3E,0x08,0x08 }, // 43  +
    { 0x00,0x50,0x30,0x00,0x00 }, // 44  ,
    { 0x08,0x08,0x08,0x08,0x08 }, // 45  -
    { 0x00,0x60,0x60,0x00,0x00 }, // 46  .
    { 0x20,0x10,0x08,0x04,0x02 }, // 47  /
    { 0x3E,0x51,0x49,0x45,0x3E }, // 48  0
    { 0x00,0x42,0x7F,0x40,0x00 }, // 49  1
    { 0x42,0x61,0x51,0x49,0x46 }, // 50  2
    { 0x21,0x41,0x45,0x4B,0x31 }, // 51  3
    { 0x18,0x14,0x12,0x7F,0x10 }, // 52  4
    { 0x27,0x45,0x45,0x45,0x39 }, // 53  5
    { 0x3C,0x4A,0x49,0x49,0x30 }, // 54  6
    { 0x01,0x71,0x09,0x05,0x03 }, // 55  7
    { 0x36,0x49,0x49,0x49,0x36 }, // 56  8
    { 0x06,0x49,0x49,0x29,0x1E }, // 57  9
    { 0x00,0x36,0x36,0x00,0x00 }, // 58  :
    { 0x00,0x56,0x36,0x00,0x00 }, // 59  ;
    { 0x08,0x14,0x22,0x41,0x00 }, // 60  <
    { 0x14,0x14,0x14,0x14,0x14 }, // 61  =
    { 0x00,0x41,0x22,0x14,0x08 }, // 62  >
    { 0x02,0x01,0x51,0x09,0x06 }, // 63  ?
    { 0x32,0x49,0x79,0x41,0x3E }, // 64  @
    { 0x7E,0x11,0x11,0x11,0x7E }, // 65  A
    { 0x7F,0x49,0x49,0x49,0x36 }, // 66  B
    { 0x3E,0x41,0x41,0x41,0x22 }, // 67  C
    { 0x7F,0x41,0x41,0x22,0x1C }, // 68  D
    { 0x7F,0x49,0x49,0x49,0x41 }, // 69  E
    { 0x7F,0x09,0x09,0x09,0x01 }, // 70  F
    { 0x3E,0x41,0x49,0x49,0x7A }, // 71  G
    { 0x7F,0x08,0x08,0x08,0x7F }, // 72  H
    { 0x00,0x41,0x7F,0x41,0x00 }, // 73  I
    { 0x20,0x40,0x41,0x3F,0x01 }, // 74  J
    { 0x7F,0x08,0x14,0x22,0x41 }, // 75  K
    { 0x7F,0x40,0x40,0x40,0x40 }, // 76  L
    { 0x7F,0x02,0x0C,0x02,0x7F }, // 77  M
    { 0x7F,0x04,0x08,0x10,0x7F }, // 78  N
    { 0x3E,0x41,0x41,0x41,0x3E }, // 79  O
    { 0x7F,0x09,0x09,0x09,0x06 }, // 80  P
    { 0x3E,0x41,0x51,0x21,0x5E }, // 81  Q
    { 0x7F,0x09,0x19,0x29,0x46 }, // 82  R
    { 0x46,0x49,0x49,0x49,0x31 }, // 83  S
    { 0x01,0x01,0x7F,0x01,0x01 }, // 84  T
    { 0x3F,0x40,0x40,0x40,0x3F }, // 85  U
    { 0x1F,0x20,0x40,0x20,0x1F }, // 86  V
    { 0x3F,0x40,0x38,0x40,0x3F }, // 87  W
    { 0x63,0x14,0x08,0x14,0x63 }, // 88  X
    { 0x07,0x08,0x70,0x08,0x07 }, // 89  Y
    { 0x61,0x51,0x49,0x45,0x43 }, // 90  Z
    { 0x00,0x7F,0x41,0x41,0x00 }, // 91  [
    { 0x02,0x04,0x08,0x10,0x20 }, // 92  backslash
    { 0x00,0x41,0x41,0x7F,0x00 }, // 93  ]
    { 0x04,0x02,0x01,0x02,0x04 }, // 94  ^
    { 0x40,0x40,0x40,0x40,0x40 }, // 95  _
    { 0x00,0x01,0x02,0x04,0x00 }, // 96  `
    { 0x20,0x54,0x54,0x54,0x78 }, // 97  a
    { 0x7F,0x48,0x44,0x44,0x38 }, // 98  b
    { 0x38,0x44,0x44,0x44,0x20 }, // 99  c
    { 0x38,0x44,0x44,0x48,0x7F }, // 100 d
    { 0x38,0x54,0x54,0x54,0x18 }, // 101 e
    { 0x08,0x7E,0x09,0x01,0x02 }, // 102 f
    { 0x0C,0x52,0x52,0x52,0x3E }, // 103 g
    { 0x7F,0x08,0x04,0x04,0x78 }, // 104 h
    { 0x00,0x44,0x7D,0x40,0x00 }, // 105 i
    { 0x20,0x40,0x44,0x3D,0x00 }, // 106 j
    { 0x7F,0x10,0x28,0x44,0x00 }, // 107 k
    { 0x00,0x41,0x7F,0x40,0x00 }, // 108 l
    { 0x7C,0x04,0x18,0x04,0x78 }, // 109 m
    { 0x7C,0x08,0x04,0x04,0x78 }, // 110 n
    { 0x38,0x44,0x44,0x44,0x38 }, // 111 o
    { 0x7C,0x14,0x14,0x14,0x08 }, // 112 p
    { 0x08,0x14,0x14,0x18,0x7C }, // 113 q
    { 0x7C,0x08,0x04,0x04,0x08 }, // 114 r
    { 0x48,0x54,0x54,0x54,0x20 }, // 115 s
    { 0x04,0x3F,0x44,0x40,0x20 }, // 116 t
    { 0x3C,0x40,0x40,0x20,0x7C }, // 117 u
    { 0x1C,0x20,0x40,0x20,0x1C }, // 118 v
    { 0x3C,0x40,0x30,0x40,0x3C }, // 119 w
    { 0x44,0x28,0x10,0x28,0x44 }, // 120 x
    { 0x0C,0x50,0x50,0x50,0x3C }, // 121 y
    { 0x44,0x64,0x54,0x4C,0x44 }, // 122 z
    { 0x00,0x08,0x36,0x41,0x00 }, // 123 {
    { 0x00,0x00,0x7F,0x00,0x00 }, // 124 |
    { 0x00,0x41,0x36,0x08,0x00 }, // 125 }
    { 0x10,0x08,0x08,0x10,0x08 }, // 126 ~
};


/* ============================================================
 *  SSD1306 Initialisation Sequence
 *
 *  Pairs: { command, argument }.  0xFF = no argument (send cmd only).
 * ============================================================ */
static const uint8_t INIT_SEQ[] = {
    0xAE, 0xFF,       // Display OFF
    0xD5, 0x80,       // Clock divide ratio / oscillator freq
    0xA8, 0x3F,       // Multiplex ratio: 64 rows (0–63)
    0xD3, 0x00,       // Display offset: none
    0x40, 0xFF,       // Start line: 0
    0x8D, 0x14,       // Charge pump: enable
    0x20, 0x00,       // Memory addressing: horizontal
    0xA1, 0xFF,       // Segment remap: col 127→SEG0
    0xC8, 0xFF,       // COM scan: remapped (top→bottom)
    0xDA, 0x12,       // COM pins hardware config
    0x81, 0xCF,       // Contrast: 0xCF (high)
    0xD9, 0xF1,       // Pre-charge period
    0xDB, 0x40,       // VCOMH deselect level
    0xA4, 0xFF,       // Entire display: follow RAM
    0xA6, 0xFF,       // Normal display (not inverted)
    0xAF, 0xFF,       // Display ON
};


/* ============================================================
 *  Private: I²C Transport
 * ============================================================ */

/** Send a single command byte (Co=0, D/C#=0). */
void oled::_cmd(uint8_t c) {
    Wire.beginTransmission(OLED_I2C_ADDR);
    Wire.write(0x00);   // Control byte: command stream
    Wire.write(c);
    Wire.endTransmission();
}

/** Convenience: send command + single argument. */
void oled::_cmd2(uint8_t c, uint8_t arg) {
    Wire.beginTransmission(OLED_I2C_ADDR);
    Wire.write(0x00);
    Wire.write(c);
    Wire.write(arg);
    Wire.endTransmission();
}

/**
 * Push raw data bytes to SSD1306 GDDRAM.
 * Splits into I2C_CHUNK-byte chunks to fit any Wire buffer.
 */
void oled::_data(const uint8_t* src, size_t len) {
    size_t offset = 0;
    while (offset < len) {
        // How many data bytes fit in this transaction?
        size_t chunk = len - offset;
        if (chunk > I2C_CHUNK) chunk = I2C_CHUNK;

        Wire.beginTransmission(OLED_I2C_ADDR);
        Wire.write(0x40);   // Control byte: data stream
        for (size_t i = 0; i < chunk; i++) {
            Wire.write(src[offset + i]);
        }
        Wire.endTransmission();

        offset += chunk;
    }
}


/* ============================================================
 *  Pin config & lazy initialisation
 * ============================================================ */

/** Store custom pins and (re)initialise immediately. */
void oled::pin(int sda, int scl) {
    _sda   = sda;
    _scl   = scl;
    _ready = false;   // force re-init with new pins
    _init();
}

/** Internal init — runs once on first use (or after pin()). */
void oled::_init() {
    Wire.setSDA(_sda);
    Wire.setSCL(_scl);
    Wire.begin();
    Wire.setClock(400000);   // Fast mode: 400 kHz

    // Send initialisation sequence
    for (size_t i = 0; i < sizeof(INIT_SEQ); i += 2) {
        uint8_t cmd = INIT_SEQ[i];
        uint8_t arg = INIT_SEQ[i + 1];

        if (arg == 0xFF) {
            _cmd(cmd);          // Single command, no argument
        } else {
            _cmd2(cmd, arg);    // Command + argument
        }
    }

    // Blank the frame buffer and push to display
    clear();
    display();
    _ready = true;   // mark as initialised
}

/** Erase frame buffer in RAM (does NOT push to display). */
void oled::clear() {
    memset(_buf, 0, sizeof(_buf));
}

/**
 * Push the entire 1024-byte frame buffer to the SSD1306.
 * Uses horizontal addressing, so the chip auto-increments X then Y.
 */
void oled::display() {
    // Reset pointer to top-left of GDDRAM
    _cmd2(0x21, 0);     // Column address: 0 …
    _cmd(127);           //                … 127
    _cmd2(0x22, 0);     // Page address:   0 …
    _cmd(7);             //                … 7

    // Send the full frame buffer
    _data(_buf, sizeof(_buf));
}


/* ============================================================
 *  Pixel & Line Primitives
 * ============================================================ */

/** Activate a single pixel and immediately push to the display. */
void oled::raw(int x, int y) {
    if (!_ready) _init();
    pixel(x, y, true);
    display();
}

/**
 *
 * The SSD1306 buffer is organised as 8 horizontal "pages" of
 * 128 bytes.  Within each page, bit 0 is the topmost row.
 *
 *  byte index = (y / 8) * 128 + x
 *  bit  index =  y % 8
 */
void oled::pixel(int x, int y, bool on) {
    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT) return;

    uint16_t idx = (uint16_t)(y / 8) * OLED_WIDTH + (uint16_t)x;
    uint8_t  bit = 1 << (y & 7);

    if (on) _buf[idx] |=  bit;
    else    _buf[idx] &= ~bit;
}

/**
 * Bresenham's line algorithm.
 * Works for any slope; no floating point required.
 */
void oled::line(int x0, int y0, int x1, int y1) {
    int dx =  abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    for (;;) {
        pixel(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = err * 2;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}


/* ============================================================
 *  High-Level Drawing
 * ============================================================ */

/** Draw a hollow axis-aligned rectangle. */
void oled::box(int x1, int y1, int x2, int y2) {
    if (!_ready) _init();
    // Normalise coordinates
    if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    if (y1 > y2) { int t = y1; y1 = y2; y2 = t; }

    line(x1, y1, x2, y1);   // top
    line(x1, y2, x2, y2);   // bottom
    line(x1, y1, x1, y2);   // left
    line(x2, y1, x2, y2);   // right

    display();
}

/* ── Circle helpers ───────────────────────────────────────── */

/** Plot the 8 symmetric points of a circle segment. */
void oled::_circlePoints(int cx, int cy, int x, int y) {
    pixel(cx + x, cy + y);
    pixel(cx - x, cy + y);
    pixel(cx + x, cy - y);
    pixel(cx - x, cy - y);
    pixel(cx + y, cy + x);
    pixel(cx - y, cy + x);
    pixel(cx + y, cy - x);
    pixel(cx - y, cy - x);
}

/**
 * Midpoint (Bresenham) circle algorithm.
 * Integer-only, no sqrt or trig.
 */
void oled::circle(int cx, int cy, int radius) {
    if (!_ready) _init();
    if (radius <= 0) return;

    int x = 0, y = radius;
    int d = 3 - 2 * radius;   // Initial decision variable

    _circlePoints(cx, cy, x, y);

    while (x <= y) {
        x++;
        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        _circlePoints(cx, cy, x, y);
    }

    display();
}

/** Draw a circle centred on the display. */
void oled::circle(int radius) {
    circle(OLED_WIDTH / 2, OLED_HEIGHT / 2, radius);
}

/** Map brightness 0–10 → SSD1306 contrast 0–255 and send. */
void oled::brightness(int level) {
    if (level < 0)  level = 0;
    if (level > 10) level = 10;

    // Linear map: 0→0x00, 10→0xFF
    uint8_t contrast = (uint8_t)((level * 255) / 10);
    _cmd2(0x81, contrast);
}

/**
 * Blit a 128×64 bitmap from a 1024-byte C array directly
 * into the frame buffer, then push to the display.
 *
 * Array format (row-major pages):
 *   byte[page * 128 + col], bit0 = topmost row of page.
 */
void oled::arr(const uint8_t* img) {
    if (!_ready) _init();
    memcpy(_buf, img, sizeof(_buf));
    display();
}


/* ============================================================
 *  Font Rendering
 * ============================================================ */

/**
 * Render a single ASCII character into the frame buffer.
 *
 * The 5×7 font stores column bytes: bit0 = top pixel.
 * We iterate over each column, then each bit, calling pixel().
 *
 * Characters that extend below row 63 or beyond col 127 are
 * clipped silently.
 */
void oled::_drawChar(int x, int y, char c) {
    // Only printable ASCII 32–126
    if (c < 32 || c > 126) return;

    const uint8_t* glyph = FONT[(uint8_t)c - 32];

    for (int col = 0; col < FONT_W; col++) {
        uint8_t colData = glyph[col];
        for (int row = 0; row < FONT_H; row++) {
            if (colData & (1 << row)) {
                pixel(x + col, y + row);
            }
        }
    }
    // The gap column is left blank (pixel() initialised to 0 by clear())
}

/**
 * Render a null-terminated string starting at pixel (x, y).
 * Characters are 6 px wide (5 px glyph + 1 px gap).
 * Long strings are clipped at the right/bottom edge.
 */
void oled::write(int x, int y, const char* str) {
    if (!_ready) _init();
    int curX = x;
    while (*str) {
        if (curX + CHAR_W > OLED_WIDTH) break;   // No room for more chars
        _drawChar(curX, y, *str);
        curX += CHAR_W;
        str++;
    }
    display();
}
