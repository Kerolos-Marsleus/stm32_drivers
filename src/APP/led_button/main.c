#include "board.h"
#include "hled.h"
#include "rcc.h"
#include "gpio.h"

static void APP_vErrorHandler(void);
int main(void){
    
    if(RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    if(RCC_enuPeriphClkCtrl(BOARD_BTN_RCC,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}

    if(HLED_enuInit() != HLED_OK){APP_vErrorHandler();}
    
    const GPIO_pinCfg_t BTN_pin = 
    {
        .port = BOARD_BTN_PORT,
        .pin  = BOARD_BTN_PIN,
        .mode = GPIO_MODE_INPUT,
        .outType = GPIO_OUTPUT_PUSH_PULL,
        .speed = GPIO_SPEED_LOW,
        .pupd = BOARD_BTN_PULL,
        .af = GPIO_AF0,
        .initState = GPIO_PIN_STATE_LOW // init in input is ignored
    };
    if(GPIO_enuPinInit(&BTN_pin) != GPIO_OK) {APP_vErrorHandler();}


    GPIO_pinState_t status;
    while(1){
        if(GPIO_enuPinRead(BTN_pin.port,BTN_pin.pin,&status)!= GPIO_OK){APP_vErrorHandler();}
        if(status == BOARD_BTN_PRESSED){
            if(HLED_enuTurnOn(HLED_STATUS) != HLED_OK) {APP_vErrorHandler();}
        }else{
            if(HLED_enuTurnOff(HLED_STATUS)!= HLED_OK) {APP_vErrorHandler();}
        }
    }
}


static void APP_vErrorHandler(void){
    while(1){}
}