#include "stk.h"
#include "stk_cfg.h"
#include "stk_priv.h"
#include "rcc.h"

static uint8_t STK_u8InitDone = 0U;
static volatile uint32_t STK_u32TimeMs = 0U;
static volatile uint32_t STK_u32PeriodMs = 0U;
static volatile uint32_t STK_u32LastCallMs = 0U;

static STK_cbf_t STK_cbf = NULL;

#if (STK_CLK_SOURCE != STK_CLK_AHB) && (STK_CLK_SOURCE != STK_CLK_AHB_DIV8)
    #error "STK_CLK_SOURCE must be STK_CLK_AHB or STK_CLK_AHB_DIV8"
#endif




STK_errCode_t STK_enuInit(void){
    STK_errCode_t error;
    uint32_t temp = STK->CTRL;
    // stop the counter
    temp &= ~(STK_1BIT_MASK << STK_CTRL_ENABLE_POS);
    // choosing the clock source
    temp &= ~(STK_1BIT_MASK << STK_CTRL_CLKSOURCE_POS);
    temp |= (STK_CLK_SOURCE << STK_CTRL_CLKSOURCE_POS);
    // choosing the mode
    temp &= ~(STK_1BIT_MASK << STK_CTRL_TICKINT_POS);
    temp |= (STK_MODE << STK_CTRL_TICKINT_POS);
    STK->CTRL = temp; 
    uint32_t freq;
    if(RCC_enuHclkFreqGet(&freq) != RCC_OK){
            error = STK_RCC_ERROR;
    }else{
        if(STK_CLK_SOURCE == STK_CLK_AHB_DIV8){
            freq /= 8U;
        }
        uint32_t loc_ticksPerMs = freq / 1000U;
        if((loc_ticksPerMs - 1U) > STK_LOAD_MAX){
            // this will not happen on these board
            // but for safety reasons if the clock became 0
            // it will overflow and give a very large number 0xFFFFFFFF
            error = STK_LOAD_OVERFLOW;
        }else{
            STK->LOAD = loc_ticksPerMs -1U;
            STK->VAL = 0U;
            STK_u8InitDone = 1U;
            error = STK_OK;
        }
    }
    return error;
}

STK_errCode_t STK_enuStart(void){
    STK_errCode_t error;
    if(STK_u8InitDone != 1U){
        error = STK_NOT_INITIALIZED;
    }else{
        STK->VAL = 0U;
        STK->CTRL |= (STK_1BIT_MASK << STK_CTRL_ENABLE_POS);
        error = STK_OK;
    }
    return error;
}

STK_errCode_t STK_enuStop(void){
    STK_errCode_t error;
    if(STK_u8InitDone != 1U){
        error = STK_NOT_INITIALIZED;
    }else{
        STK->CTRL &= ~(STK_1BIT_MASK << STK_CTRL_ENABLE_POS);
        error = STK_OK;
    }
    return error;
}



STK_errCode_t STK_enuDelayMs(uint32_t delayMs){
    STK_errCode_t error;
    if(STK_u8InitDone != 1U){
        error = STK_NOT_INITIALIZED;
    }else{
        uint32_t freq;
        if(RCC_enuHclkFreqGet(&freq) != RCC_OK){
            error = STK_RCC_ERROR;
        }else{
            if(STK_CLK_SOURCE == STK_CLK_AHB_DIV8){
                freq /= 8U;
            }
            uint32_t loc_ticksPerMs = freq / 1000U;
            if((loc_ticksPerMs - 1U) > STK_LOAD_MAX){
                // this will not happen on these board
                // but for safety reasons if the clock became 0
                // it will overflow and give a very large number 0xFFFFFFFF
                error = STK_LOAD_OVERFLOW;
            }else{
                STK->LOAD = loc_ticksPerMs -1U;
                STK->VAL = 0U;
                STK->CTRL |= (STK_1BIT_MASK << STK_CTRL_ENABLE_POS);
                for(uint32_t i = 0U; i < delayMs; i++){
                    while((STK->CTRL & (STK_1BIT_MASK << STK_CTRL_COUNTFLAG_POS)) == 0U){
                        // this while will be looping while the COUNTFLAG is not 1
                        // when COUNTFLAG become 1 it will exit and the read operation done 
                        // will by hardware turn it off again
                        // this while loop will take 1ms
                    }
                }
                STK->CTRL &= ~(STK_1BIT_MASK << STK_CTRL_ENABLE_POS);
                error = STK_OK;
            }
        }
    }
    return error;
}

STK_errCode_t STK_enuGetTicks(uint32_t *ticks){
    STK_errCode_t error;
    if(ticks == NULL){
        error = STK_NULLPTR;
    }else{
        *ticks = STK->VAL;
        error = STK_OK;
    }
    return error;
}

STK_errCode_t STK_enuStartFromMax(void){
    STK_errCode_t error;
    if(STK_u8InitDone != 1U){
        error = STK_NOT_INITIALIZED;
    }else{
        STK->LOAD = 0x00FFFFFFU;
        STK->VAL = 0U;
        STK->CTRL |= (STK_1BIT_MASK << STK_CTRL_ENABLE_POS);
        error = STK_OK;
    }
    return error;
}

void SysTick_Handler(void){
    STK_u32TimeMs++;
    if(STK_cbf != NULL){
        if((STK_u32TimeMs - STK_u32LastCallMs) >= STK_u32PeriodMs){
            STK_u32LastCallMs = STK_u32TimeMs;
            STK_cbf();
        }
    }
}

STK_errCode_t STK_enuSetCallback(STK_cbf_t cbf){
    STK_errCode_t error;
    if(cbf == NULL){
        error = STK_NULLPTR; 
    }else{
        STK_cbf = cbf;
        error = STK_OK;
    }
    return error;
}
STK_errCode_t STK_enuNotifyOnMs(uint32_t delayMs){
    STK_u32LastCallMs = STK_u32TimeMs;
    STK_u32PeriodMs = delayMs;
    return STK_OK;
}


