/**
 * @file lv_tslib.h
 * tslib input driver for LVGL 9.4
 */

#ifndef LV_TSLIB_H
#define LV_TSLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../indev/lv_indev.h"

#if LV_USE_TSLIB

/**
 * Create an LVGL input device backed by tslib.
 * @param dev_path  tslib device path, e.g. "/dev/input/event0".
 *                  If NULL, tslib uses TSLIB_TSDEVICE env var.
 * @return pointer to the created indev, or NULL on failure.
 */
lv_indev_t * lv_tslib_create(const char * dev_path);

/**
 * Delete a tslib input device and release resources.
 * @param indev  pointer returned by lv_tslib_create
 */
void lv_tslib_delete(lv_indev_t * indev);

#endif /* LV_USE_TSLIB */

#ifdef __cplusplus
}
#endif

#endif /* LV_TSLIB_H */
