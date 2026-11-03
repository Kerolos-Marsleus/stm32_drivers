#include "board.h"
#include "hled.h"
#include "rcc.h"
#include "stk.h"

#define APP_BLINK_TICKS             8000000U
#define STK_MAX                     0x00FFFFFFU

static void APP_vErrorHandler(void);
int main(void){
    
    if(RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}

    if(HLED_enuInit() != HLED_OK) {APP_vErrorHandler();}
    if(STK_enuInit() != STK_OK) {APP_vErrorHandler();}
    if(STK_enuStartFromMax() != STK_OK){APP_vErrorHandler();}
    uint32_t end;
    uint32_t start;
    if(STK_enuGetTicks(&start) != STK_OK){APP_vErrorHandler();}
    while(1){
        if(STK_enuGetTicks(&end) != STK_OK){APP_vErrorHandler();}
        uint32_t elapsed = (start - end) & STK_MAX;
        if(elapsed > APP_BLINK_TICKS){
            if(HLED_enuToggle(HLED_STATUS) != HLED_OK){APP_vErrorHandler();}
            start = end;
        }

    }
}


static void APP_vErrorHandler(void){
    while(1){}
}