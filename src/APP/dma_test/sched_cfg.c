#include "sched.h"
#include "sched_cfg.h"
#include "app.h"

const SCHED_runnableCfg_t SCHED_cfg[SCHED_RUNNABLE_COUNT] =
    {
        [SCHED_RUN_DMA_TEST] =
            {
                .func = APP_vReport,
                .firstDelayMs = 200U,
                .periodMs = 1000U,
            },

};