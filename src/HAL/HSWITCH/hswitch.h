#ifndef HSWITCH_H
#define HSWITCH_H
#include "gpio.h"
#include "hswitch_cfg.h"

typedef struct{
    GPIO_port_t port;
    GPIO_pin_t pin;
    GPIO_pupd_t pupd;
    GPIO_pinState_t pressedState;
} HSWITCH_cfg_t;

typedef enum{
    HSWITCH_STATE_RELEASED = 0,
    HSWITCH_STATE_PRE_PRESSED,
    HSWITCH_STATE_PRESSED,
    HSWITCH_STATE_PRE_RELEASED,
    HSWITCH_STATE_COUNT
} HSWITCH_state_t;

typedef enum{
    HSWITCH_OK = 0,
    HSWITCH_INVALID_ID,
    HSWITCH_NULLPTR,
    HSWITCH_GPIO_ERROR,
    HSWITCH_ERROR_COUNT
}HSWITCH_errCode_t;

HSWITCH_errCode_t   HSWITCH_enuInit(void);
HSWITCH_errCode_t   HSWITCH_enuReadState(HSWITCH_id_t id,HSWITCH_state_t* ptr_state);
void                HSWITCH_vRunnable(void);  
#endif /*HSWITCH_H*/