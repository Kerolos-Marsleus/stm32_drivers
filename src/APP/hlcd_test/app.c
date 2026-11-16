#include "app.h"
#include "hled.h"
#include "rcc.h"
#include "board.h"
#include "hlcd.h"

void APP_vInit(void){
    if(RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    if(RCC_enuPeriphClkCtrl(RCC_GPIOA,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    if(RCC_enuPeriphClkCtrl(RCC_GPIOB,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    if(HLED_enuInit() != HLED_OK){APP_vErrorHandler();}
    if(HLCD_enuInit() != HLCD_OK){APP_vErrorHandler();}
}



void APP_vErrorHandler(void){
    while(1){}
}
