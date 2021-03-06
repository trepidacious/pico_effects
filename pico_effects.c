/*

MIT No Attribution

Copyright (c) 2021 Mika Tuupola

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-cut-

SPDX-License-Identifier: MIT-0

*/

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <pico/stdlib.h>

#include <sys/time.h>

#include <hagl_hal.h>
#include <hagl.h>
#include <font6x9.h>
#include <fps.h>
#include <aps.h>

#include "metaballs.h"
#include "plasma.h"
#include "rotozoom.h"

static uint8_t effect = 1;
volatile bool fps_flag = false;
volatile bool switch_flag = true;
static float effect_fps;

static bitmap_t *bb;
wchar_t message[32];

static char demo[3][32] = {
    "METABALLS",
    "PLASMA",
    "ROTOZOOM",
};

bool switch_timer_callback(struct repeating_timer *t) {
    switch_flag = true;
    return true;
}

bool fps_timer_callback(struct repeating_timer *t) {
    fps_flag = true;
    return true;
}

int main()
{
    struct repeating_timer switch_timer;
    struct repeating_timer fps_timer;

    color_t red = hagl_color(255, 0, 0);
    color_t green = hagl_color(0, 255, 0);
    color_t blue = hagl_color(0, 0, 255);

    stdio_init_all();

    bb = hagl_init();
    if (bb) {
        printf("Back buffer: %dx%dx%d\r\n", bb->width, bb->height, bb->depth);
    } else {
        printf("No back buffer\r\n");
    }

    hagl_clear_screen();
    hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);

    add_repeating_timer_ms(10000, switch_timer_callback, NULL, &switch_timer);
    add_repeating_timer_ms(1000, fps_timer_callback, NULL, &fps_timer);

    while (1) {

        switch(effect) {
        case 0:
            metaballs_animate();
            metaballs_render();
            break;
        case 1:
            plasma_render();
            break;
        case 2:
            rotozoom_animate();
            rotozoom_render();
            break;
        }

        hagl_flush();
        effect_fps = aps(1);

        if (switch_flag) {
            switch_flag = false;
            effect = (effect + 1) % 3;

            switch(effect) {
            case 0:
                metaballs_init();
                break;
            case 1:
                plasma_init();
                break;
            case 2:
                rotozoom_init();
                break;
            }

            /* Print the message on top left corner. */
            swprintf(message, sizeof(message), L"%s    ", demo[effect]);
            hagl_set_clip_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
            hagl_put_text(message, 4, 4, green, font6x9);
            hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);

            /* Print the message in console. */
            printf("%s at %d FPS\r\n", demo[effect], (uint32_t)effect_fps);

            aps(APS_RESET);
        }

        if (fps_flag) {
            fps_flag = 0;

            /* Print the message on lower right corner. */
            swprintf(message, sizeof(message), L"%.*f FPS  ", 0, effect_fps);
            hagl_set_clip_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
            hagl_put_text(message, DISPLAY_WIDTH - 40, DISPLAY_HEIGHT - 14, green, font6x9);
            hagl_set_clip_window(0, 20, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 21);
        }

    };

    return 0;
}
