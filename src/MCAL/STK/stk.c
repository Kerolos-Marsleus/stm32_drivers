#include "stk.h"
#include "stk_cfg.h"
#include "stk_priv.h"
#include "rcc.h"

static uint8_t STK_u8InitDone = 0U;

#if (STK_CLK_SOURCE != STK_CLK_AHB) && (STK_CLK_SOURCE != STK_CLK_AHB_DIV8)
    #error "STK_CLK_SOURCE must be STK_CLK_AHB or STK_CLK_AHB_DIV8"
#endif


#if (STK_MODE == STK_MODE_INTERRUPT)
    #error "STK_MODE_INTERRUPT is not implemented yet"
#endif /*systck interrupt mode*/

STK_errCode_t STK_enuInit(void){
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
    STK_u8InitDone = 1U;
    return STK_OK;
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


