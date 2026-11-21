#ifndef HKEYPAD_H
#define HKEYPAD_H
#include "gpio.h"
#include "hkeypad_cfg.h"

#define HKEYPAD_ROWS            4U
#define HKEYPAD_COLUMNS         4U
#define HKEYPAD_KEYS            (HKEYPAD_COLUMNS * HKEYPAD_ROWS)

typedef enum{
    HKEYPAD_OK = 0,
    HKEYPAD_NULLPTR,
    HKEYPAD_GPIO_ERROR,
    HKEYPAD_INVALID_ID,
    HKEYPAD_ERROR_COUNT
}HKEYPAD_errCode_t;



typedef struct
{
    GPIO_port_t port;
    GPIO_pin_t  pin;
}HKEYPAD_pinCfg_t;

typedef struct
{
    HKEYPAD_pinCfg_t rows[HKEYPAD_ROWS];
    HKEYPAD_pinCfg_t columns[HKEYPAD_COLUMNS];
    char keymap[HKEYPAD_ROWS][HKEYPAD_COLUMNS];
}HKEYPAD_cfg_t;

HKEYPAD_errCode_t HKEYPAD_enuInit(void);
void HKEYPAD_vRunnable(void);
HKEYPAD_errCode_t HKEYPAD_enuGetKey(HKEYPAD_id_t id, char* ptr_key );

#endif /*HKEYPAD_H*/