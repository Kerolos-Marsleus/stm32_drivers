#include "sched.h"
#include "sched_cfg.h"
#include "app.h"

const SCHED_runnableCfg_t SCHED_cfg[SCHED_RUNNABLE_COUNT] =
    {
        [SCHED_RUN_UART_TEST] =
            {
                .func = APP_vUartTest,
                .firstDelayMs = 200U,
                .periodMs = 1000U,
            },

};