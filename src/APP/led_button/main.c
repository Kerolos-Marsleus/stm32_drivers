#include "gpio.h"
#include "rcc.h"
#include "board.h"
#include <stdio.h>



int main(void){
    RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE);
    RCC_enuPeriphClkCtrl(BOARD_BTN_RCC,RCC_ENABLE);


    const GPIO_pinCfg_t LED_pin = 
    {
        .port = BOARD_LED_PORT,
        .pin  = BOARD_LED_PIN,
        .mode = GPIO_MODE_OUTPUT,
        .outType = GPIO_OUTPUT_PUSH_PULL,
        .speed = GPIO_SPEED_LOW,
        .pupd = GPIO_PUPD_NONE,
        .af = GPIO_AF0,
        .initState = BOARD_LED_OFF
    };
    const GPIO_pinCfg_t BTN_pin = 
    {
        .port = BOARD_BTN_PORT,
        .pin  = BOARD_BTN_PIN,
        .mode = GPIO_MODE_INPUT,
        .outType = GPIO_OUTPUT_PUSH_PULL,
        .speed = GPIO_SPEED_LOW,
        .pupd = BOARD_BTN_PULL,
        .af = GPIO_AF0,
        .initState = BOARD_BTN_PRESSED // init in input is ignored
    };
    GPIO_enuPinInit(&LED_pin);
    GPIO_enuPinInit(&BTN_pin);


    GPIO_pinState_t status;
    while(1){
        GPIO_enuPinRead(BTN_pin.port,BTN_pin.pin,&status);
        if(status == BOARD_BTN_PRESSED){
            GPIO_enuPinWrite(LED_pin.port,LED_pin.pin,BOARD_LED_ON);
        }else{
            GPIO_enuPinWrite(LED_pin.port,LED_pin.pin,BOARD_LED_OFF);
        }
    }
}
