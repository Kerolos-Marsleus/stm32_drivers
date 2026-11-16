#ifndef HLCD_H
#define HLCD_H
#include "gpio.h"
#include "hlcd_cfg.h"

typedef enum
{
    HLCD_OK = 0,
    HLCD_GPIO_ERROR,      /* a GPIO call failed */
    HLCD_BUSY,
    HLCD_ERROR_COUNT
} HLCD_errCode_t;

typedef struct
{
    GPIO_port_t port;
    GPIO_pin_t  pin;
} HLCD_pinCfg_t;



void HLCD_vRunnable(void);
HLCD_errCode_t HLCD_enuInit(void);
#endif /*HLCD_H*/