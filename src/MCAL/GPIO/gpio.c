#include "gpio.h"
#include "gpio_priv.h"
#include <stddef.h>




static GPIO_Reg_t *const GPIO_ports[GPIO_PORT_COUNT] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOH};

static void GPIO_vWriteBitField(volatile uint32_t *reg, uint32_t pin, uint32_t width, uint32_t value){
    uint32_t mask = (1U << width) -1U;
    uint32_t loc_u32_temp = *reg;
    uint32_t loc_u32_pos =  pin * width;
    loc_u32_temp &= ~(mask << loc_u32_pos);
    loc_u32_temp |= (value << loc_u32_pos);
    *reg = loc_u32_temp;
}


GPIO_errCode_t GPIO_enuCfgPinMode(GPIO_port_t port, GPIO_pin_t pin, GPIO_mode_t mode){
    GPIO_errCode_t error;
    if((uint32_t) port >= GPIO_PORT_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if( (uint32_t) pin >= GPIO_PIN_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if((uint32_t) mode >= GPIO_MODE_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else{  
        GPIO_vWriteBitField(&(GPIO_ports[port]->MODER),(uint32_t)pin, GPIO_2BIT_FIELD_WIDTH ,(uint32_t)mode);
        error = GPIO_OK;
    }

    return error;
}

GPIO_errCode_t GPIO_enuPinWrite(GPIO_port_t port, GPIO_pin_t pin,GPIO_pinState_t state){
    GPIO_errCode_t error;
    if((uint32_t) port >= GPIO_PORT_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if( (uint32_t) pin >= GPIO_PIN_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if((uint32_t) state >= GPIO_PIN_STATE_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else{
        if((uint32_t)state == GPIO_PIN_STATE_HIGH){
            // BSRR is write only so reading it will return 0s
            GPIO_ports[port]->BSRR = (1U << (uint32_t)pin);
        }else{
            GPIO_ports[port]->BSRR = (1U << ((uint32_t)pin  + GPIO_BSRR_RESET_OFFSET));
        }
        error = GPIO_OK;
    }
    return error;
}

GPIO_errCode_t GPIO_enuPinRead(GPIO_port_t port, GPIO_pin_t pin, GPIO_pinState_t* state){
    GPIO_errCode_t error;
    if((uint32_t) port >= GPIO_PORT_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if( (uint32_t) pin >= GPIO_PIN_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if(state == NULL){
        error = GPIO_NULLPTR;
    }else{
        *state = (GPIO_pinState_t)((GPIO_ports[port]->IDR >> (uint32_t)pin) & GPIO_IDR_MASK);
        error = GPIO_OK;
    }
    return error;
}

GPIO_errCode_t GPIO_enuCfgSpeed(GPIO_port_t port, GPIO_pin_t pin,GPIO_speed_t speed){
    GPIO_errCode_t error;
    if((uint32_t) port >= GPIO_PORT_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if( (uint32_t) pin >= GPIO_PIN_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if((uint32_t) speed >= GPIO_SPEED_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else{
        GPIO_vWriteBitField(&(GPIO_ports[port]->OSPEEDR), (uint32_t)pin, GPIO_2BIT_FIELD_WIDTH , (uint32_t)speed);
        error = GPIO_OK;
    }
    return error;
}


GPIO_errCode_t GPIO_enuCfgPUPD(GPIO_port_t port, GPIO_pin_t pin,GPIO_pupd_t pupd){
    GPIO_errCode_t error;
    if((uint32_t) port >= GPIO_PORT_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if( (uint32_t) pin >= GPIO_PIN_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if((uint32_t) pupd >= GPIO_PUPD_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else{
        GPIO_vWriteBitField(&(GPIO_ports[port]->PUPDR),(uint32_t) pin, GPIO_2BIT_FIELD_WIDTH , (uint32_t) pupd);
        error = GPIO_OK;
    }
    return error;
}

GPIO_errCode_t GPIO_enuCfgOutType(GPIO_port_t port, GPIO_pin_t pin,GPIO_outputType_t type){
    GPIO_errCode_t error;
    if((uint32_t) port >= GPIO_PORT_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if( (uint32_t) pin >= GPIO_PIN_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if((uint32_t) type >= GPIO_OUTPUT_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else{
        if(type == GPIO_OUTPUT_PUSH_PULL){
            GPIO_ports[port]->OTYPER &= ~(1U << (uint32_t) pin);
        }else{
            GPIO_ports[port]->OTYPER |= (1U << (uint32_t) pin);

        }
        error = GPIO_OK;
    }
    return error;
}

GPIO_errCode_t GPIO_enuCfgAlternativeFunc(GPIO_port_t port, GPIO_pin_t pin,GPIO_af_t af){
    GPIO_errCode_t error;
    if((uint32_t) port >= GPIO_PORT_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if( (uint32_t) pin >= GPIO_PIN_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else if((uint32_t) af >= GPIO_AF_COUNT){
        error = GPIO_INVALID_ARGUMENT;
    }else{
        if((uint32_t) pin < GPIO_AFR_PINS_PER_REG ){
            GPIO_vWriteBitField(&(GPIO_ports[port]->AFRL), (uint32_t)pin,(uint32_t)GPIO_4BIT_FIELD_WIDTH,(uint32_t)af);
        }else{
            GPIO_vWriteBitField(&(GPIO_ports[port]->AFRH), (uint32_t)(pin) % GPIO_AFR_PINS_PER_REG, GPIO_4BIT_FIELD_WIDTH,(uint32_t)af);
        }
        error = GPIO_OK;
    }
    return error;
}

GPIO_errCode_t GPIO_enuPinInit(const GPIO_pinCfg_t *Ptr_Cfg){
    GPIO_errCode_t error;
    if(Ptr_Cfg == NULL)
    {
        error = GPIO_NULLPTR;
    }else
    {
        error = GPIO_enuPinWrite(Ptr_Cfg->port, Ptr_Cfg->pin,Ptr_Cfg->initState);
        if(error == GPIO_OK)
        {
            error = GPIO_enuCfgSpeed(Ptr_Cfg->port, Ptr_Cfg->pin,Ptr_Cfg->speed);
        }
        if(error == GPIO_OK)
        {
            error = GPIO_enuCfgPUPD(Ptr_Cfg->port, Ptr_Cfg->pin,Ptr_Cfg->pupd);
        }
        if(error == GPIO_OK)
        {
            error = GPIO_enuCfgOutType(Ptr_Cfg->port, Ptr_Cfg->pin,Ptr_Cfg->outType);
        }
        if(error == GPIO_OK)
        {
            error = GPIO_enuCfgAlternativeFunc(Ptr_Cfg->port, Ptr_Cfg->pin,Ptr_Cfg->af);
        }
        if(error == GPIO_OK)
        {
            error = GPIO_enuCfgPinMode(Ptr_Cfg->port, Ptr_Cfg->pin,Ptr_Cfg->mode);
        }   
    }
    return error;
}

GPIO_errCode_t GPIO_enuPortWrite(GPIO_port_t port, uint16_t value){
    GPIO_errCode_t error;
    if((uint32_t) port >= GPIO_PORT_COUNT)
    {
        error = GPIO_INVALID_ARGUMENT;
    }else
    {
        GPIO_ports[port]->ODR = value;
        error = GPIO_OK;
    }
    return error;
}


GPIO_errCode_t GPIO_enuPortRead(GPIO_port_t port, uint16_t *value){
    GPIO_errCode_t error;
    if((uint32_t) port >= GPIO_PORT_COUNT)
    {
        error = GPIO_INVALID_ARGUMENT;
    }
    else if(value == NULL)
    {
        error = GPIO_NULLPTR;
    }
    else
    {
        *value = (uint16_t) ((GPIO_ports[port]->IDR) & 0xFFFFU);
        error = GPIO_OK;
    }
    return error;
}
