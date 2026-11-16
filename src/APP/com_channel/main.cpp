#include "gpio.h"
#include "rcc.h"
#include "hled.h"
#include "board.h"
#include "server.hpp"
#include "client.hpp"

static void APP_vErrorHandler(void);

int main(){
    RCC_enuPeriphClkCtrl(BOARD_BTN_RCC,RCC_ENABLE);
    RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE);
    
    HLED_enuInit();
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

    Server s;
    Client c(s,TEMP);

    while(1){
        GPIO_pinState_t state;
        GPIO_enuPinRead(BTN_pin.port,BTN_pin.pin,&state);
        s.Write_channel(TEMP,state);
        int val = c.listen();
        if(val){
            HLED_enuTurnOn(HLED_STATUS);
        }else{
            HLED_enuTurnOff(HLED_STATUS);
        }
        // if(state == BOARD_BTN_PRESSED){
        //     HLED_enuTurnOn(HLED_STATUS);
        // }else{
        //     HLED_enuTurnOff(HLED_STATUS);
        // }
    }
    
}


static void APP_vErrorHandler(void){
    while(1){}
}