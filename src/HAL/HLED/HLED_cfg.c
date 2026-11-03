#include "HLED.h"
#include "board.h"

const HLED_cfg_t HLED_cfg[HLED_COUNT] = 
{
    [HLED_STATUS] = 
    {
        .port               = BOARD_LED_PORT,
        .pin                = BOARD_LED_PIN,
        .outType            = GPIO_OUTPUT_PUSH_PULL,
        .speed              = GPIO_SPEED_LOW,
        .activeState        = BOARD_LED_ON
    },
};