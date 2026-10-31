#include "rcc.h"
#include "gpio.h"
#include <stdio.h>

int main(void){
    RCC_errCode_t rcc_error;
    GPIO_errCode_t gpio_error;
    const GPIO_pinCfg_t LED_pin = 
    {
        .port = GPIO_PORTC,
        .pin  = GPIO_PIN13,
        .mode = GPIO_MODE_OUTPUT,
        .outType = GPIO_OUTPUT_PUSH_PULL,
        .speed = GPIO_SPEED_LOW,
        .pupd = GPIO_PUPD_NONE,
        .af = GPIO_AF0,
        .initState = GPIO_PIN_STATE_HIGH
    };

    static const uint8_t SEG_digits[10] =
{
    0x3F,   /* 0: a b c d e f   */
    0x06,   /* 1:   b c         */
    0x5B,   /* 2: a b   d e   g */
    0x4F,   /* 3: a b c d     g */
    0x66,   /* 4:   b c     f g */
    0x6D,   /* 5: a   c d   f g */
    0x7D,   /* 6: a   c d e f g */
    0x07,   /* 7: a b c         */
    0x7F,   /* 8: a b c d e f g */
    0x6F    /* 9: a b c d   f g */
};
    
    rcc_error = RCC_enuPeriphClkCtrl(RCC_GPIOA, RCC_ENABLE);
    rcc_error = RCC_enuPeriphClkCtrl(RCC_GPIOB, RCC_ENABLE);
    // configure port B 
    gpio_error = GPIO_enuCfgPinMode(GPIO_PORTB, GPIO_PIN0,GPIO_MODE_INPUT);
    gpio_error = GPIO_enuCfgPinMode(GPIO_PORTB, GPIO_PIN1,GPIO_MODE_INPUT);
    gpio_error = GPIO_enuCfgPinMode(GPIO_PORTB, GPIO_PIN5,GPIO_MODE_INPUT);
    gpio_error = GPIO_enuCfgPUPD(GPIO_PORTB, GPIO_PIN0,GPIO_PUPD_UP);
    gpio_error = GPIO_enuCfgPUPD(GPIO_PORTB, GPIO_PIN1,GPIO_PUPD_UP);
    gpio_error = GPIO_enuCfgPUPD(GPIO_PORTB, GPIO_PIN5,GPIO_PUPD_UP);

    GPIO_pinCfg_t ledCfg = 
    {
        .port      = GPIO_PORTA,
        .pin       = GPIO_PIN0,
        .mode      = GPIO_MODE_OUTPUT,
        .outType   = GPIO_OUTPUT_PUSH_PULL,
        .speed     = GPIO_SPEED_LOW,
        .pupd      = GPIO_PUPD_NONE,
        .af        = GPIO_AF0,
        .initState = GPIO_PIN_STATE_HIGH
    };
    for (uint32_t i = 0U; i < 8U; i++)
    {
        ledCfg.pin = (GPIO_pin_t)i;
        gpio_error = GPIO_enuPinInit(&ledCfg);
    }

    GPIO_pinState_t status;
    uint16_t count = 0;
    while(1){
        GPIO_enuPinRead(GPIO_PORTB,GPIO_PIN0,&status);
        for(volatile uint32_t i = 0; i < 100000; i++){};
        if(status == 0){
            count++;
        }
        
        GPIO_enuPinRead(GPIO_PORTB,GPIO_PIN1,&status);
        for(volatile uint32_t i = 0; i < 100000; i++){};
        if(status == 0){
            count--;
        }
        GPIO_enuPortWrite(GPIO_PORTA, ~SEG_digits[count]);

    
    }   
}

