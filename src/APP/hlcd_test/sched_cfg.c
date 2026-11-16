#include "sched.h"
#include "sched_cfg.h"
#include "app.h"
#include "hswitch.h"
const SCHED_runnableCfg_t SCHED_cfg[SCHED_RUNNABLE_COUNT] = 
{
    [SCHED_RUN_CHECK_BTNS] = 
    {
        .func = HSWITCH_vRunnable,
        .firstDelayMs = 0U,
        .periodMs = 5U
    },
    [SCHED_RUN_BTN_TO_LED] = 
    {
        .func = APP_vButtonToLed,
        .firstDelayMs = 0U,
        .periodMs = 20U
    }
};