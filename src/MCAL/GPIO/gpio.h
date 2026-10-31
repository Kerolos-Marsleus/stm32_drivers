#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef enum{
    GPIO_OK,
    GPIO_NULLPTR,
    GPIO_INVALID_ARGUMENT,
    GPIO_ERROR_COUNT
}GPIO_errCode_t;



typedef enum{
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_ALTERNATE,
    GPIO_MODE_ANALOG,
    GPIO_MODE_COUNT
}GPIO_mode_t;

typedef enum{
    GPIO_OUTPUT_PUSH_PULL,
    GPIO_OUTPUT_OPEN_DRAIN,
    GPIO_OUTPUT_COUNT
}GPIO_outputType_t;

typedef enum{
    GPIO_SPEED_LOW,
    GPIO_SPEED_MEDIUM,
    GPIO_SPEED_HIGH,
    GPIO_SPEED_VERY_HIGH,
    GPIO_SPEED_COUNT
}GPIO_speed_t;

typedef enum{
    GPIO_PUPD_NONE,
    GPIO_PUPD_UP,
    GPIO_PUPD_DOWN,
    GPIO_PUPD_COUNT
}GPIO_pupd_t;



typedef enum
{
    GPIO_AF0 = 0U, GPIO_AF1,  GPIO_AF2,  GPIO_AF3,
    GPIO_AF4,      GPIO_AF5,  GPIO_AF6,  GPIO_AF7,
    GPIO_AF8,      GPIO_AF9,  GPIO_AF10, GPIO_AF11,
    GPIO_AF12,     GPIO_AF13, GPIO_AF14, GPIO_AF15,
    GPIO_AF_COUNT
} GPIO_af_t;

typedef enum
{
    GPIO_PORTA = 0U,
    GPIO_PORTB,
    GPIO_PORTC,
    GPIO_PORTD,
    GPIO_PORTE,
    GPIO_PORTH,
    GPIO_PORT_COUNT
} GPIO_port_t;

typedef enum
{
    GPIO_PIN0 = 0U, GPIO_PIN1,  GPIO_PIN2,  GPIO_PIN3,
    GPIO_PIN4,      GPIO_PIN5,  GPIO_PIN6,  GPIO_PIN7,
    GPIO_PIN8,      GPIO_PIN9,  GPIO_PIN10, GPIO_PIN11,
    GPIO_PIN12,     GPIO_PIN13, GPIO_PIN14, GPIO_PIN15,
    GPIO_PIN_COUNT
} GPIO_pin_t;

typedef enum
{
    GPIO_PIN_STATE_LOW  = 0U,
    GPIO_PIN_STATE_HIGH = 1U,
    GPIO_PIN_STATE_COUNT
} GPIO_pinState_t;

typedef struct
{
    GPIO_port_t        port;
    GPIO_pin_t         pin;
    GPIO_mode_t        mode;
    GPIO_outputType_t  outType;
    GPIO_speed_t       speed;
    GPIO_pupd_t        pupd;
    GPIO_af_t          af;         /* used only when mode == ALTERNATE */
    GPIO_pinState_t    initState;  /* used only when mode == OUTPUT    */
} GPIO_pinCfg_t;

GPIO_errCode_t GPIO_enuCfgPinMode(GPIO_port_t port, GPIO_pin_t pin, GPIO_mode_t mode);
GPIO_errCode_t GPIO_enuPinWrite(GPIO_port_t port, GPIO_pin_t pin,GPIO_pinState_t state);
GPIO_errCode_t GPIO_enuPinRead(GPIO_port_t port, GPIO_pin_t pin, GPIO_pinState_t* state);
GPIO_errCode_t GPIO_enuCfgSpeed(GPIO_port_t port, GPIO_pin_t pin,GPIO_speed_t speed);
GPIO_errCode_t GPIO_enuCfgPUPD(GPIO_port_t port, GPIO_pin_t pin,GPIO_pupd_t pupd);
GPIO_errCode_t GPIO_enuCfgOutType(GPIO_port_t port, GPIO_pin_t pin,GPIO_outputType_t type);
GPIO_errCode_t GPIO_enuCfgAlternativeFunc(GPIO_port_t port, GPIO_pin_t pin,GPIO_af_t af);
GPIO_errCode_t GPIO_enuPinInit(const GPIO_pinCfg_t *Ptr_Cfg);
GPIO_errCode_t GPIO_enuPortWrite(GPIO_port_t port, uint16_t value);
GPIO_errCode_t GPIO_enuPortRead(GPIO_port_t port, uint16_t *value);
#endif  /*GPIO_H*/