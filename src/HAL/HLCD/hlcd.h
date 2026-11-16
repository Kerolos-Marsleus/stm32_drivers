#ifndef HLCD_H
#define HLCD_H
#include "gpio.h"
#include "hlcd_cfg.h"

#define HLCD_DATA_PINS 8U
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



static HLCD_errCode_t HLCD_enuSendByte(uint8_t byte, GPIO_pinState_t rs);
static HLCD_errCode_t HLCD_enuInitPins(void);
void HLCD_vRunnable(void);
HLCD_errCode_t HLCD_enuInit(void);
#endif /*HLCD_H*/