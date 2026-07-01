/**
 * ============================================================
 *  OLED.h — Minimal SSD1306 128×64 I²C Library
 *  Optimised for Seeed Studio XIAO RP2040
 * ============================================================
 *  Dependency: built-in Wire only.  No Adafruit, no U8g2.
 *
 *  Usage:
 *      oled o;
 *      o.pin(6, 7);           // optional — override SDA/SCL before begin()
 *      o.begin();
 *      o.write(0, 0, "Hello!");
 *      o.box(10, 10, 50, 30);
 *      o.circle(20);
 *      o.brightness(7);
 * ============================================================
 */

#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <Wire.h>

/* ── Hardware constants ─────────────────────────────────── */
#define OLED_I2C_ADDR   0x3C        // Default SSD1306 address (alt: 0x3D)
#define OLED_WIDTH      128
#define OLED_HEIGHT     64
#define OLED_PAGES      8           // 64 px / 8 bits per page

/* ── Font metrics ───────────────────────────────────────── */
#define FONT_W          5           // Glyph columns
#define FONT_H          7           // Glyph rows
#define FONT_GAP        1           // Horizontal gap between chars
#define CHAR_W          (FONT_W + FONT_GAP)   // 6 px per character cell
#define CHAR_H          8                      // 8 px tall (fits one page)

/* ── I²C chunk size ─────────────────────────────────────── */
// Wire buffer = 256 B on RP2040 (Arduino-Pico core).
// We keep chunks at 31 data bytes per transaction to stay
// compatible with 32-byte Wire buffers on other boards.
#define I2C_CHUNK       31


/* ============================================================
 *  oled  — main display class
 * ============================================================ */
class oled {
public:
    /* ── Pin config ─────────────────────────────────────── */

    /**
     * Override I²C pins. Call this before any drawing function.
     * If never called, defaults to D4 (SDA) and D5 (SCL).
     *
     *   o.pin(2, 3);      // custom SDA=2, SCL=3
     *   o.write(0,0,"Hi");
     */
    void pin(int sda, int scl);

    /** Erase the frame buffer (doesn't push to display). */
    void clear();

    /** Push frame buffer to the physical display. */
    void display();

    /* ── High-level drawing ─────────────────────────────── */

    /**
     * Render a null-terminated string.
     * @param x   Left edge in pixels (0–127)
     * @param y   Top  edge in pixels (0–63)
     * @param str The text to draw
     */
    void write(int x, int y, const char* str);

    /**
     * Draw a hollow rectangle.
     * @param x1,y1  Top-left corner
     * @param x2,y2  Bottom-right corner
     */
    void box(int x1, int y1, int x2, int y2);

    /**
     * Draw a hollow circle centred on the display.
     * @param radius  Radius in pixels
     */
    void circle(int radius);

    /**
     * Draw a hollow circle at an arbitrary centre.
     * @param cx,cy   Centre in pixels
     * @param radius  Radius in pixels
     */
    void circle(int cx, int cy, int radius);

    /**
     * Set display brightness.
     * @param level  0 (off) … 10 (max contrast)
     */
    void brightness(int level);

    /**
     * Blit a 128×64 monochrome bitmap from a C byte array.
     * Array layout: 1024 bytes, row-major pages (same as SSD1306
     * horizontal-addressing frame buffer).
     * @param img  Pointer to 1024-byte array
     */
    void arr(const uint8_t* img);

    /* ── Low-level primitives (public for power users) ───── */

    /**
     * Activate a single pixel at (x, y) and push to display.
     * @param x  0–127
     * @param y  0–63
     */
    void raw(int x, int y);

    /**
     * Set or clear one pixel in the frame buffer.
     * Call display() afterwards to push changes.
     * @param x,y  Pixel coordinate
     * @param on   true = white, false = black
     */
    void pixel(int x, int y, bool on = true);

    /**
     * Draw a Bresenham line between two points.
     */
    void line(int x0, int y0, int x1, int y1);

private:
    /* 128 × 8 pages = 1024 bytes — the entire frame buffer */
    uint8_t _buf[OLED_WIDTH * OLED_PAGES];

    /* ── Lazy-init state ────────────────────────────────── */
    bool _ready = false;   // flips true after first _init()
    int  _sda   = D4;      // default XIAO RP2040 SDA pin
    int  _scl   = D5;      // default XIAO RP2040 SCL pin

    /** Called automatically on first use (or after pin()). */
    void _init();

    /* ── SSD1306 transport ──────────────────────────────── */

    /** Send a single SSD1306 command byte. */
    void _cmd(uint8_t c);

    /** Send a two-byte command (command + argument). */
    void _cmd2(uint8_t c, uint8_t arg);

    /** Push raw data bytes to the display RAM. */
    void _data(const uint8_t* src, size_t len);

    /* ── Internal helpers ───────────────────────────────── */

    /** Draw one ASCII glyph from the built-in 5×7 font. */
    void _drawChar(int x, int y, char c);

    /** Midpoint-circle algorithm — plots 8-fold symmetry. */
    void _circlePoints(int cx, int cy, int x, int y);
};

#endif // OLED_H
