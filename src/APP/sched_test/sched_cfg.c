#include "sched.h"
#include "sched_cfg.h"
#include "app.h"

const SCHED_runnableCfg_t SCHED_cfg[SCHED_RUNNABLE_COUNT] = 
{
    [SCHED_RUN_BLINK_STATUS] = 
    {
        .func = APP_vBlinkStatus,
        .firstDelayMs = 10U,
        .periodMs = 1000U
    },
    [SCHED_RUN_BLINK_PA0] = 
    {
        .func = APP_vBlinkPA0,
        .firstDelayMs = 25U,
        .periodMs = 200U
    }
};