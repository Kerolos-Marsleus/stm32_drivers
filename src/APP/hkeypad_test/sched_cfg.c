#include "sched.h"
#include "sched_cfg.h"
#include "app.h"
#include "hlcd.h"
#include "hkeypad.h"
const SCHED_runnableCfg_t SCHED_cfg[SCHED_RUNNABLE_COUNT] =
    {
        [SCHED_RUN_HLCD] =
            {
                .func = HLCD_vRunnable,
                .firstDelayMs = 50U,
                .periodMs = 2U,
            },
        [SCHED_RUN_HKEYPAD] =
            {
                .func = HKEYPAD_vRunnable,
                .firstDelayMs = 100U,
                .periodMs = 1U,
            },
        [SCHED_RUN_DISPLAY_KEYPAD] =
            {
                .func = APP_vShowKey,
                .firstDelayMs = 100U,
                .periodMs = 50U,
            },

};