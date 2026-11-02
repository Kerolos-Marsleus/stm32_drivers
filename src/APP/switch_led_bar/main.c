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
        .initState = GPIO_PIN_STATE_LOW
    };
    for (uint32_t i = 0U; i < 8U; i++)
    {
        ledCfg.pin = (GPIO_pin_t)i;
        gpio_error = GPIO_enuPinInit(&ledCfg);
    }

    GPIO_pinState_t status;
    
    while(1){
        uint8_t value = 0;
        GPIO_enuPinRead(GPIO_PORTB,GPIO_PIN0,&status);
        for(volatile uint32_t i = 0; i < 50000; i++){};
        if(status == 0){
            value |= (1U << 0);
        }
        GPIO_enuPinRead(GPIO_PORTB,GPIO_PIN1,&status);
        for(volatile uint32_t i = 0; i < 50000; i++){};
        if(status == 0){
            value |= (1U << 1);
        }
        GPIO_enuPinRead(GPIO_PORTB,GPIO_PIN5,&status);
        for(volatile uint32_t i = 0; i < 50000; i++){};
        if(status == 0){
            value |= (1U << 2);
        }
        uint8_t pattern = (1U << value) -1;
        GPIO_enuPortWrite(GPIO_PORTA, pattern);

    
    }   
}

