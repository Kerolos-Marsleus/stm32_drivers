#include "sched.h"
#include "sched_cfg.h"
#include "hlcd.h"
#include "hkeypad.h"
#include "app.h"
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
                .firstDelayMs = 120U,
                .periodMs = 1U,
            },
        [SCHED_RUN_CALCULATOR_APP] =
            {
                .func = APP_vRunnable,
                .firstDelayMs = 100U,
                .periodMs = 50U,
            },

};