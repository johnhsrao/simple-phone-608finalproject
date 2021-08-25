/*
    Loading.h - Library for a loading animation.
*/
#ifndef Loading_h
#define Loading_h
#include "Arduino.h"

class Loading {
    private:
        uint8_t x;
        uint8_t y;
        uint8_t r;
        uint32_t timer;
        uint16_t angle;
        uint8_t period;
        bool state_change;
        void fillArc(int seg_count, int rx, int ry, int w, unsigned int colour);
    public:
        Loading();
        void draw_screen();
        void set_dimensions(uint8_t center_x, uint8_t center_y, uint8_t radius, uint32_t speed = 10);
};

#endif