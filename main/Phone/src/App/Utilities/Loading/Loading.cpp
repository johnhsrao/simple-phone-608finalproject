#include "Arduino.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Phone.h"

Loading::Loading() {
    x = 64;
    y = 80;
    r = 15;
    timer = millis();
    angle = 0;
    period = 10;
    state_change = true;
}

void Loading::set_dimensions(uint8_t center_x, uint8_t center_y, uint8_t radius, uint32_t speed) {
    x = center_x;
    y = center_y;
    r = radius;
    timer = millis();
    angle = 0;
    period = speed;
    state_change = true;
}

void Loading::draw_screen() {
    // update the position of the spinner
    if (millis() - timer >= period) {
        // erase old spinner
        fillArc(6, r, r, 5, TFT_KEY_GRAY);
        if (angle < 359) {
            angle++;
        } else {
            angle = 0;
        }
        timer = millis();
    }

    // draw spinner background
    if (state_change) {
        fillArc(60, r, r, 5, TFT_KEY_GRAY);
        state_change = false;
    }
    // draw new spinner
    fillArc(6, r, r, 5, TFT_WHITE);
}

void Loading::fillArc(int seg_count, int rx, int ry, int w, unsigned int colour) {
    byte seg = 6; // Segments are 3 degrees wide = 120 segments for 360 degrees
    byte inc = 6; // Draw segments every 3 degrees, increase to 6 for segmented ring

    // Calculate first pair of coordinates for segment start
    float sx = cos((angle - 90) * 0.0174532925);
    float sy = sin((angle - 90) * 0.0174532925);
    uint16_t x0 = sx * (rx - w) + x;
    uint16_t y0 = sy * (ry - w) + y;
    uint16_t x1 = sx * rx + x;
    uint16_t y1 = sy * ry + y;

    // Draw colour blocks every inc degrees
    for (int i = angle; i < angle + seg * seg_count; i += inc) {
        // Calculate pair of coordinates for segment end
        float sx2 = cos((i + seg - 90) * 0.0174532925);
        float sy2 = sin((i + seg - 90) * 0.0174532925);
        int x2 = sx2 * (rx - w) + x;
        int y2 = sy2 * (ry - w) + y;
        int x3 = sx2 * rx + x;
        int y3 = sy2 * ry + y;

        tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
        tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);

        // Copy segment end to sgement start for next segment
        x0 = x2;
        y0 = y2;
        x1 = x3;
        y1 = y3;
    }
}