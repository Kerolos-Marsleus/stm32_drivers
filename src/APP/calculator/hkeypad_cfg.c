#include "hkeypad_cfg.h"
#include "hkeypad.h"



const HKEYPAD_cfg_t HKEYPAD_cfg[HKEYPAD_COUNT] = {
    [HKEYPAD_MAIN] =
        {
            .rows =
                {
                    {.port = GPIO_PORTA, .pin = GPIO_PIN9},  /* R0 */
                    {.port = GPIO_PORTA, .pin = GPIO_PIN10}, /* R1 */
                    {.port = GPIO_PORTA, .pin = GPIO_PIN11}, /* R2 */
                    {.port = GPIO_PORTA, .pin = GPIO_PIN12}, /* R3 */
                },
            .columns =
                {
                    {.port = GPIO_PORTB, .pin = GPIO_PIN12}, /* C0 */
                    {.port = GPIO_PORTB, .pin = GPIO_PIN13}, /* C1 */
                    {.port = GPIO_PORTB, .pin = GPIO_PIN14}, /* C2 */
                    {.port = GPIO_PORTB, .pin = GPIO_PIN15}, /* C3 */
                },
            .keymap =
                {
                    {'1', '2', '3', '+'},           /* R0 */
                    {'4', '5', '6', '-'},           /* R1 */
                    {'7', '8', '9', '*'},           /* R2 */
                    {APP_KEY_SHIFT, '0', APP_KEY_EQUALS, '/'}, /* R3 */
                },
        },
};