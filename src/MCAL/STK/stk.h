#ifndef STK_H
#define STK_H

#include <stdint.h>
/*
    Call STK_enuInit(void) after the system clock is configured
*/

typedef void (*STK_cbf_t)(void);
typedef enum
{
    STK_OK = 0,
    STK_NULLPTR,
    STK_LOAD_OVERFLOW,
    STK_NOT_INITIALIZED,
    STK_RCC_ERROR,
    STK_INVALID_PERIOD,
    STK_ERROR_COUNT
} STK_errCode_t;


STK_errCode_t STK_enuInit(void);
STK_errCode_t STK_enuStart(void);
STK_errCode_t STK_enuStop(void);
STK_errCode_t STK_enuDelayMs(uint32_t delayMs);
STK_errCode_t STK_enuGetTicks(uint32_t *ticks);
STK_errCode_t STK_enuStartFromMax(void);
STK_errCode_t STK_enuSetCallback(STK_cbf_t cbf);
STK_errCode_t STK_enuNotifyOnMs(uint32_t delayMs);
#endif /*STK_H*/