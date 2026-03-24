/*******************************************************************
 *
 * main.c - LVGL application for GNU/Linux
 *
 * Supports two display backends:
 *   - SDL2:  for Ubuntu desktop simulation (LV_USE_SDL=1)
 *   - fbdev: for embedded Linux (LV_USE_LINUX_FBDEV=1)
 *
 ******************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "lvgl/lvgl.h"

int main(void)
{
    lv_init();

#if LV_USE_SDL
    lv_sdl_window_create(800, 480);
    lv_sdl_mouse_create();
#endif

#if LV_USE_LINUX_FBDEV
    lv_display_t *disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, "/dev/fb0");
#endif

#if LV_USE_EVDEV
    lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event1");
#endif

    /* Create a Demo */
    // lv_demo_widgets();
    // lv_demo_widgets_start_slideshow();

    /* Main loop */
    while(1) {
        uint32_t time_till_next = lv_timer_handler();
        usleep(time_till_next * 1000);
    }

    return 0;
}
