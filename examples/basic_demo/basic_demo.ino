/**
 * ============================================================
 *  basic_demo.ino — XIAO SSD1306 Library Demo
 *  Board: Seeed Studio XIAO RP2040
 * ============================================================
 *
 *  Wiring (XIAO RP2040  →  SSD1306 module):
 *    3.3V  →  VCC
 *    GND   →  GND
 *    D4    →  SDA   (GPIO 6)
 *    D5    →  SCL   (GPIO 7)
 *
 *  Install: copy the XIAO_SSD1306 folder to your Arduino
 *           libraries directory, then restart the IDE.
 * ============================================================
 */

#include <OLED.h>

oled o;   // Single display object — the entire API lives here

/* ── 128×64 bitmap: smiley face ───────────────────────────── */
// Byte-counted: exactly 1024 bytes (8 pages x 128 cols).
// Encodes a face: circle outline, two eyes, smile arc.
static const uint8_t SMILEY[1024] = {
    /* page 0 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,128,128,64,64,32,32,32,32,16,16,16,16,
    16,16,16,16,16,32,32,32,32,64,64,128,128,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    /* page 1 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,128,64,32,16,8,4,
    4,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,
    4,4,8,16,32,64,128,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    /* page 2 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,192,48,12,2,1,0,0,0,0,0,
    0,0,192,224,240,248,220,12,6,6,3,3,1,1,1,1,
    1,1,1,1,1,3,3,6,6,12,220,248,240,224,192,0,
    0,0,0,0,0,0,1,2,12,48,192,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    /* page 3 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,240,15,0,0,0,0,0,0,0,0,0,0,
    4,7,7,7,7,7,7,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,7,7,7,7,7,7,
    4,0,0,0,0,0,0,0,0,0,0,15,240,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    /* page 4 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,31,224,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,224,31,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    /* page 5 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,1,6,24,96,128,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,128,96,24,6,1,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    /* page 6 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,3,4,8,16,32,64,
    64,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,
    64,64,32,16,8,4,3,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    /* page 7 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,1,2,2,4,4,8,8,8,8,16,16,16,16,
    16,16,16,16,16,8,8,8,8,4,4,2,2,1,1,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/* ── Demo state machine ───────────────────────────────────── */
int  demoStep    = 0;
bool fadingUp    = true;
int  brightness  = 5;

void setup() {
    // No begin() needed — display auto-inits on first use (D4=SDA, D5=SCL)
    // To use custom pins: o.pin(2, 3);  ← call this first, then draw

    // ── Step 0: Hello world text ───────────────────────────
    o.write(0,  0, "XIAO RP2040");
    o.write(0,  8, "SSD1306 Demo");
    o.write(0, 16, "No libraries!");
    o.write(0, 24, "Wire only :)");
    delay(2500);
}

void loop() {
    // Cycle through a set of demos

    switch (demoStep % 6) {

        /* ── Demo 1: Basic text at various positions ───── */
        case 0:
            o.clear();
            o.write( 0,  0, "Hello, World!");
            o.write( 0,  8, "OLED.h library");
            o.write( 0, 16, "5x7 bitmap font");
            o.write( 0, 24, "128 x 64 pixels");
            o.write( 0, 40, "Step 1: Text");
            delay(2000);
            break;

        /* ── Demo 2: Rectangle (box) ────────────────────── */
        case 1:
            o.clear();
            o.write(30, 0, "Box demo");
            o.box( 5,  12, 122, 55);   // Outer border
            o.box(20,  20, 107, 47);   // Inner box
            o.write(35, 25, "Nested!");
            delay(2000);
            break;

        /* ── Demo 3: Circles ────────────────────────────── */
        case 2:
            o.clear();
            o.write(30, 0, "Circle demo");
            o.circle(64, 38, 22);      // Centre of lower area
            o.circle(64, 38, 14);
            o.circle(64, 38,  6);
            delay(2000);
            break;

        /* ── Demo 4: Line art ───────────────────────────── */
        case 3: {
            o.clear();
            o.write(28, 0, "Line demo");
            // Starburst from centre
            int cx = 64, cy = 38;
            for (int angle = 0; angle < 360; angle += 30) {
                float rad = angle * 3.14159f / 180.0f;
                int ex = cx + (int)(28 * cos(rad));
                int ey = cy + (int)(18 * sin(rad));
                o.line(cx, cy, ex, ey);
            }
            o.display();
            delay(2000);
            break;
        }

        /* ── Demo 5: Brightness sweep ───────────────────── */
        case 4:
            o.clear();
            o.write(10,  0, "Brightness sweep");
            o.write(10, 16, "Fading 0 -> 10");
            o.box(5, 30, 122, 45);
            // Animate brightness bar
            for (int b = 0; b <= 10; b++) {
                o.brightness(b);

                // Draw fill bar proportional to brightness
                o.clear();
                o.write(10,  0, "Brightness sweep");
                o.write(10, 16, "Fading 0 -> 10");
                o.box(5, 30, 122, 45);
                if (b > 0) {
                    int fillX = 5 + (int)((117.0f * b) / 10);
                    o.box(5, 30, fillX, 45);
                    // Fill interior
                    for (int fy = 31; fy < 45; fy++) {
                        o.line(6, fy, fillX - 1, fy);
                    }
                    o.display();
                }
                delay(120);
            }
            for (int b = 10; b >= 0; b--) {
                o.brightness(b);
                delay(80);
            }
            o.brightness(5);   // Reset to mid
            delay(500);
            break;

        /* ── Demo 6: Bitmap from array ──────────────────── */
        case 5:
            o.arr(SMILEY);                        // Blank bitmap (see above)
            o.write(10,  4, "arr() bitmap");
            o.write(10, 24, "128x64 array");
            o.write(10, 44, "direct blit!");
            o.display();
            delay(2000);
            break;
    }

    demoStep++;
}
