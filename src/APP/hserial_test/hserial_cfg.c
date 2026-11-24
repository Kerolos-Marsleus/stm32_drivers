#include "hserial.h"
#include "hserial_cfg.h"
#include "uart.h"
#include "uart_cfg.h"

const HSERIAL_cfg_t HSERIAL_cfg[HSERIAL_COUNT] =
    {
        [HSERIAL_DEBUG] =
            {
                .uart = UART_DEBUG,
                .txMode = HSERIAL_MODE_INTERRUPT,
                .rxMode = HSERIAL_MODE_INTERRUPT,
                .txDma = {
                    .controller = DMA_1,
                    .stream = DMA_STREAM_6,
                    .channel = DMA_CHANNEL_4,
                    .priority = DMA_PRIORITY_LOW,
                },
                .rxDma = {
                    .controller = DMA_1,
                    .stream = DMA_STREAM_5,
                    .channel = DMA_CHANNEL_4,
                    .priority = DMA_PRIORITY_MEDIUM,
                }
            },
};