#include "hswitch.h"
#include "board.h"
const HSWITCH_cfg_t HSWITCH_cfg[HSWITCH_COUNT] =
{
    [HSWITCH_USER_BTN] = 
    {
        .port           = BOARD_BTN_PORT,
        .pin            = BOARD_BTN_PIN,
        .pupd           = BOARD_BTN_PULL,
        .pressedState   = BOARD_BTN_PRESSED
    }
};