#include "uart.h"
#include "uart_cfg.h"

const UART_cfg_t UART_cfg[UART_COUNT] =
    {
        [UART_DEBUG] =
            {
                .channel = UART_CH_2,
                .baudRate = 9600U,
                .wordLength = UART_WORD_8BIT,
                .parity = UART_PARITY_NONE,
                .stopBits = UART_STOP_1,
                .direction = UART_DIR_TX_RX,
                .oversampling = UART_OVERSAMPLING_16,
                .txPin.port = GPIO_PORTA,
                .txPin.pin = GPIO_PIN2,
                .rxPin.port = GPIO_PORTA,
                .rxPin.pin = GPIO_PIN3,

            },
};