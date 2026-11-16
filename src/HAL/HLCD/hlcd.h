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

/* Values match the datasheet bit, so they can be shifted directly */
typedef enum { HLCD_MODE_4BIT = 0, HLCD_MODE_8BIT }        HLCD_mode_t;   /* DL  */
typedef enum { HLCD_LINES_1   = 0, HLCD_LINES_2 }          HLCD_lines_t;  /* N   */
typedef enum { HLCD_FONT_5X7  = 0, HLCD_FONT_5X10 }        HLCD_font_t;   /* F   */
typedef enum { HLCD_OFF       = 0, HLCD_ON }               HLCD_onOff_t;  /* C, B, SH */
typedef enum { HLCD_ENTRY_DECREMENT = 0, HLCD_ENTRY_INCREMENT } HLCD_entry_t; /* I/D */

typedef enum
{
    HLCD_PIN_D0 = 0,    /* D0..D3 unused in 4-bit mode */
    HLCD_PIN_D1,
    HLCD_PIN_D2,
    HLCD_PIN_D3,
    HLCD_PIN_D4,
    HLCD_PIN_D5,
    HLCD_PIN_D6,
    HLCD_PIN_D7,
    HLCD_PIN_RS,
    HLCD_PIN_E,
    HLCD_PIN_COUNT
} HLCD_pin_t;

typedef struct
{
    HLCD_pinCfg_t pins[HLCD_PIN_COUNT];
    HLCD_mode_t   mode;
    HLCD_lines_t  lines;
    HLCD_font_t   font;
    HLCD_onOff_t  cursor;
    HLCD_onOff_t  blink;
    HLCD_entry_t  entryDirection;
    HLCD_onOff_t  displayShift;
} HLCD_cfg_t;

void HLCD_vRunnable(void);
HLCD_errCode_t HLCD_enuInit(void);
#endif /*HLCD_H*/