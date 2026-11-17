#include "sched.h"
#include "sched_cfg.h"
#include "app.h"
#include "hlcd.h"
const SCHED_runnableCfg_t SCHED_cfg[SCHED_RUNNABLE_COUNT] = 
{
    [SCHED_RUN_UPTIME] = 
    {
        .func = APP_vUptime,
        .firstDelayMs = 0U,
        .periodMs = 1000U
    },
    [SCHED_RUN_HLCD] = 
    {
        .func = HLCD_vRunnable,
        .firstDelayMs = 50U,
        .periodMs = 2U
    }
};