#ifndef STK_H
#define STK_H

#include <stdint.h>
/*
    Call STK_enuInit(void) after the system clock is cofigured
*/
typedef enum
{
    STK_OK = 0,
    STK_LOAD_OVERFLOW,
    STK_NOT_INITIALIZED,
    STK_RCC_ERROR,
    STK_ERROR_COUNT
} STK_errCode_t;


STK_errCode_t STK_enuInit(void);
STK_errCode_t STK_enuDelayMs(uint32_t delayMs);


#endif /*STK_H*/