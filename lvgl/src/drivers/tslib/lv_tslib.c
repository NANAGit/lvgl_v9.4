/**
 * @file lv_tslib.c
 * tslib input driver for LVGL 9.4
 *
 * Uses tslib to read calibrated touch data and feed it into the
 * LVGL indev subsystem as a POINTER device.
 */

#include "lv_tslib.h"

#if LV_USE_TSLIB

#include <tslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../misc/lv_log.h"
#include "../../stdlib/lv_mem.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    struct tsdev * ts;
    lv_indev_state_t last_state;
    int last_x;
    int last_y;
} lv_tslib_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void tslib_read_cb(lv_indev_t * indev, lv_indev_data_t * data);
static void tslib_delete_cb(lv_event_t * e);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_tslib_create(const char * dev_path)
{
    struct tsdev * ts = NULL;

    if(dev_path != NULL) {
        ts = ts_open(dev_path, 1);  /* 1 = non-blocking */
    }
    else {
        /* Falls back to TSLIB_TSDEVICE environment variable */
        ts = ts_open(getenv("TSLIB_TSDEVICE"), 1);
    }

    if(ts == NULL) {
        LV_LOG_ERROR("ts_open failed");
        return NULL;
    }

    if(ts_config(ts) != 0) {
        LV_LOG_ERROR("ts_config failed");
        ts_close(ts);
        return NULL;
    }

    lv_tslib_data_t * dsc = lv_malloc_zeroed(sizeof(lv_tslib_data_t));
    if(dsc == NULL) {
        LV_LOG_ERROR("lv_malloc failed");
        ts_close(ts);
        return NULL;
    }
    dsc->ts = ts;
    dsc->last_state = LV_INDEV_STATE_RELEASED;

    lv_indev_t * indev = lv_indev_create();
    if(indev == NULL) {
        lv_free(dsc);
        ts_close(ts);
        return NULL;
    }

    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, tslib_read_cb);
    lv_indev_set_driver_data(indev, dsc);
    lv_indev_add_event_cb(indev, tslib_delete_cb, LV_EVENT_DELETE, NULL);

    LV_LOG_INFO("tslib indev created (%s)", dev_path ? dev_path : "env");
    return indev;
}

void lv_tslib_delete(lv_indev_t * indev)
{
    lv_indev_delete(indev);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void tslib_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_tslib_data_t * dsc = lv_indev_get_driver_data(indev);

    struct ts_sample samp;
    int ret = ts_read(dsc->ts, &samp, 1);

    if(ret > 0) {
        if(samp.pressure > 0) {
            dsc->last_x = samp.x;
            dsc->last_y = samp.y;
            dsc->last_state = LV_INDEV_STATE_PRESSED;
        }
        else {
            dsc->last_state = LV_INDEV_STATE_RELEASED;
        }
    }

    data->point.x = dsc->last_x;
    data->point.y = dsc->last_y;
    data->state = dsc->last_state;
    data->continue_reading = false;
}

static void tslib_delete_cb(lv_event_t * e)
{
    lv_indev_t * indev = lv_event_get_target(e);
    lv_tslib_data_t * dsc = lv_indev_get_driver_data(indev);
    if(dsc == NULL) return;

    if(dsc->ts) {
        ts_close(dsc->ts);
    }
    lv_free(dsc);
}

#endif /* LV_USE_TSLIB */
