#include "board.h"
#include "hled.h"
#include "rcc.h"
#include "stk.h"

static void APP_vErrorHandler(void);
int main(void){
    
    if(RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}

    if(HLED_enuInit() != HLED_OK) {APP_vErrorHandler();}
    if(STK_enuInit() != STK_OK) {APP_vErrorHandler();}

    while(1){
        if(HLED_enuToggle(HLED_STATUS) != HLED_OK) {APP_vErrorHandler();}
        if(STK_enuDelayMs(500U) != STK_OK) {APP_vErrorHandler();}
    }
}


static void APP_vErrorHandler(void){
    while(1){}
}