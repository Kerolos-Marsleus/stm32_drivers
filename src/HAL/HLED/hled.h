#ifndef HLED_H
#define HLED_H

#include "gpio.h"
#include "hled_cfg.h"

typedef struct
{
    GPIO_port_t        port;
    GPIO_pin_t         pin;
    GPIO_outputType_t  outType;
    GPIO_speed_t       speed;
    GPIO_pinState_t    activeState;  // the pin level that turns this led on
} HLED_cfg_t;


typedef enum
{
    HLED_OK = 0,
    HLED_INVALID_ID,
    HLED_GPIO_ERROR,
    HLED_ERROR_COUNT
} HLED_errCode_t;

HLED_errCode_t HLED_enuInit(void);
HLED_errCode_t HLED_enuTurnOn(HLED_id_t id);
HLED_errCode_t HLED_enuTurnOff(HLED_id_t id);
HLED_errCode_t HLED_enuToggle(HLED_id_t id);

#endif /*HLED_H*/