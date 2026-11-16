#include "sched.h"
#include "sched_cfg.h"
#include "stk.h"
#include <stddef.h>

extern const SCHED_runnableCfg_t SCHED_cfg[SCHED_RUNNABLE_COUNT];
static uint32_t SCHED_remainingMs[SCHED_RUNNABLE_COUNT];
static volatile uint8_t SCHED_u8TickPending = 0U;
static volatile uint8_t SCHED_u8Running= 0U;

static void SCHED_vTickCbf(void)
{
    SCHED_u8TickPending = 1U;
}

SCHED_errCode_t SCHED_enuInit(void){
    SCHED_errCode_t error = SCHED_OK;
    for(uint32_t i = 0U; (i < SCHED_RUNNABLE_COUNT) && (error == SCHED_OK); i++){
        if((SCHED_cfg[i].func == NULL) || (SCHED_cfg[i].periodMs == 0U)){
            error = SCHED_INVALID_CFG;
        }else{
            SCHED_remainingMs[i] = SCHED_cfg[i].firstDelayMs;
        }
    }
    if(error == SCHED_OK){if(STK_enuInit() != STK_OK){error = SCHED_STK_ERROR;}}
    if(error == SCHED_OK){if(STK_enuSetCallback(SCHED_vTickCbf)!= STK_OK){error = SCHED_STK_ERROR;}}
    if(error == SCHED_OK){if(STK_enuNotifyOnMs(1U)!= STK_OK){error = SCHED_STK_ERROR;}}
    return error;
}

SCHED_errCode_t SCHED_enuStart(void){
    SCHED_errCode_t error = SCHED_OK;
    if(STK_enuStart() != STK_OK){error = SCHED_STK_ERROR;}
    if(error == SCHED_OK)
    {
        SCHED_u8TickPending = 0U;
        SCHED_u8Running = 1U;
        while(SCHED_u8Running != 0U){
            if(SCHED_u8TickPending != 0U){
                SCHED_u8TickPending = 0U;
                for(uint32_t i = 0U; (i < SCHED_RUNNABLE_COUNT) ; i++){
                    if(SCHED_remainingMs[i] > 0U){
                        SCHED_remainingMs[i]--;
                    }
                    if(SCHED_remainingMs[i] == 0U)
                    {
                        SCHED_remainingMs[i] = SCHED_cfg[i].periodMs;
                        SCHED_cfg[i].func();
                    }
                }
            }
        }
    }
    return error;
}

SCHED_errCode_t SCHED_enuStop(void){
    SCHED_errCode_t error = SCHED_OK;
    if(STK_enuStop() != STK_OK){error = SCHED_STK_ERROR;}else{
        SCHED_u8Running = 0U;
    }
    return error;
}
