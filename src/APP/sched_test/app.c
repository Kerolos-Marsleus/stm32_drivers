#include "app.h"
#include "hled.h"
#include "rcc.h"
#include "board.h"

void APP_vBlinkStatus(void){
    (void)HLED_enuToggle(HLED_STATUS);
}

void APP_vBlinkPA0(void){
    (void)HLED_enuToggle(HLED_PA0);
}

void APP_vInit(void){
    if(RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    if(RCC_enuPeriphClkCtrl(RCC_GPIOA,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    if(HLED_enuInit() != HLED_OK){APP_vErrorHandler();}
}


void APP_vErrorHandler(void){
    while(1){}
}