#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>

typedef void (*SCHED_runnable_t)(void);

typedef enum
{
    SCHED_OK = 0,
    SCHED_STK_ERROR,
    SCHED_INVALID_CFG,
    SCHED_ERROR_COUNT
}SCHED_errCode_t;

typedef struct 
{
    SCHED_runnable_t func;
    uint32_t firstDelayMs;
    uint32_t periodMs;
}SCHED_runnableCfg_t;

SCHED_errCode_t SCHED_enuInit(void);
/* Starts the tick and runs the scheduler loop. Does not return unless an error occurs. */
SCHED_errCode_t SCHED_enuStart(void);
SCHED_errCode_t SCHED_enuStop(void);


#endif /*SCHED_H*/