#include "hlcd_cfg.h"
#include "hlcd.h"
const HLCD_pinCfg_t HLCD_pinCfg[HLCD_PIN_COUNT]=
{
    [HLCD_PIN_D0] = {
        .port   = GPIO_PORTA,
        .pin    = GPIO_PIN1
    },
    [HLCD_PIN_D1] = {
        .port   = GPIO_PORTA,
        .pin    = GPIO_PIN2
    },
    [HLCD_PIN_D2] = {
        .port   = GPIO_PORTA,
        .pin    = GPIO_PIN3
    },
    [HLCD_PIN_D3] = {
        .port   = GPIO_PORTA,
        .pin    = GPIO_PIN4
    },
    [HLCD_PIN_D4] = {
        .port   = GPIO_PORTA,
        .pin    = GPIO_PIN5
    },
    [HLCD_PIN_D5] = {
        .port   = GPIO_PORTA,
        .pin    = GPIO_PIN6
    },
    [HLCD_PIN_D6] = {
        .port   = GPIO_PORTA,
        .pin    = GPIO_PIN7
    },
    [HLCD_PIN_D7] = {
        .port   = GPIO_PORTA,
        .pin    = GPIO_PIN8
    },
    [HLCD_PIN_RS] = {
        .port   = GPIO_PORTB,
        .pin    = GPIO_PIN0
    },
    [HLCD_PIN_E] = {
        .port   = GPIO_PORTB,
        .pin    = GPIO_PIN1
    }
};
