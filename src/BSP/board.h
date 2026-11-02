#ifndef     BOARD_H
#define     BOARD_H

#include "gpio.h"
#if defined(BOARD_BLACKPILL_F401)

    #define BOARD_LED_PORT          GPIO_PORTC
    #define BOARD_LED_PIN           GPIO_PIN13
    #define BOARD_LED_ON            GPIO_PIN_STATE_LOW
    #define BOARD_LED_OFF           GPIO_PIN_STATE_HIGH

    #define BOARD_BTN_PORT          GPIO_PORTA
    #define BOARD_BTN_PIN           GPIO_PIN0
    #define BOARD_BTN_PULL          GPIO_PUPD_UP
    #define BOARD_BTN_PRESSED       GPIO_PIN_STATE_LOW

#elif defined(BOARD_NUCLEO_F411)
    #define BOARD_LED_PORT          GPIO_PORTA
    #define BOARD_LED_PIN           GPIO_PIN5
    #define BOARD_LED_ON            GPIO_PIN_STATE_HIGH
    #define BOARD_LED_OFF           GPIO_PIN_STATE_LOW

    #define BOARD_BTN_PORT          GPIO_PORTC
    #define BOARD_BTN_PIN           GPIO_PIN13
    #define BOARD_BTN_PULL          GPIO_PUPD_NONE
    #define BOARD_BTN_PRESSED       GPIO_PIN_STATE_LOW


#else   
    #error  "No board selected"



#endif

#endif      /*BOARD_H*/