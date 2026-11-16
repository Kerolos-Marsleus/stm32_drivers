#ifndef HLCD_CFG_H
#define HLCD_CFG_H


/* ---- possible values (datasheet 6.3, 6.4, 6.6) ---- */
#define HLCD_LINES_1            0U
#define HLCD_LINES_2            1U
#define HLCD_FONT_5X7           0U
#define HLCD_FONT_5X10          1U
#define HLCD_OFF                0U
#define HLCD_ON                 1U
#define HLCD_ENTRY_DECREMENT    0U
#define HLCD_ENTRY_INCREMENT    1U

/* ---- user choice ---- */
#define HLCD_LINES              HLCD_LINES_2
#define HLCD_FONT               HLCD_FONT_5X7
#define HLCD_CURSOR             HLCD_OFF
#define HLCD_BLINK              HLCD_OFF
#define HLCD_ENTRY_DIRECTION    HLCD_ENTRY_INCREMENT
#define HLCD_DISPLAY_SHIFT      HLCD_OFF


typedef enum{
    HLCD_PIN_D0 = 0,
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



#endif /*HLCD_CFG_H*/