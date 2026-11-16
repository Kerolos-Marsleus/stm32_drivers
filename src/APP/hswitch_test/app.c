#include "app.h"
#include "hled.h"
#include "rcc.h"
#include "board.h"
#include "hswitch.h"

void APP_vInit(void){
    if(RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    if(RCC_enuPeriphClkCtrl(BOARD_BTN_RCC,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    if(HLED_enuInit() != HLED_OK){APP_vErrorHandler();}
    if(HSWITCH_enuInit() != HSWITCH_OK){APP_vErrorHandler();}
}


void APP_vButtonToLed(void){
    HSWITCH_state_t state;
    if(HSWITCH_enuReadState(HSWITCH_USER_BTN,&state)!= HSWITCH_OK){APP_vErrorHandler();}
    if(state == HSWITCH_STATE_PRESSED){
        (void)HLED_enuTurnOn(HLED_STATUS);
    }else{
        (void)HLED_enuTurnOff(HLED_STATUS);
    }
}

void APP_vErrorHandler(void){
    while(1){}
}
