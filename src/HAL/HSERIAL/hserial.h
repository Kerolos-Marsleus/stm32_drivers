#ifndef HSERIAL_H
#define HSERIAL_H

#include <stdint.h>
#include "uart.h"
#include "dma.h"
#include "hserial_cfg.h"

typedef enum
{
    HSERIAL_MODE_INTERRUPT = 0,
    HSERIAL_MODE_DMA,
    HSERIAL_MODE_COUNT
} HSERIAL_mode_t;

typedef struct
{
    DMA_id_t       controller;
    DMA_stream_t   stream;
    DMA_channel_t  channel;
    DMA_priority_t priority;
} HSERIAL_dmaCfg_t;

typedef struct
{
    UART_id_t        uart;
    HSERIAL_mode_t   txMode;
    HSERIAL_mode_t   rxMode;
    HSERIAL_dmaCfg_t txDma;      /* used only if txMode == DMA */
    HSERIAL_dmaCfg_t rxDma;      /* used only if rxMode == DMA */
} HSERIAL_cfg_t;

typedef void (*HSERIAL_cbf_t)(void);

typedef enum
{
    HSERIAL_OK = 0,
    HSERIAL_NULLPTR,
    HSERIAL_INVALID_ID,
    HSERIAL_INVALID_LEN,
    HSERIAL_INVALID_CFG,
    HSERIAL_BUSY,
    HSERIAL_UART_ERROR,
    HSERIAL_DMA_ERROR,
    HSERIAL_ERROR_COUNT
} HSERIAL_errCode_t;

HSERIAL_errCode_t HSERIAL_enuInit(void);
HSERIAL_errCode_t HSERIAL_enuSend(HSERIAL_id_t id, const uint8_t *buf, uint16_t len, HSERIAL_cbf_t cbf);
HSERIAL_errCode_t HSERIAL_enuRecv(HSERIAL_id_t id, uint8_t *buf, uint16_t len, HSERIAL_cbf_t cbf);

#endif /* HSERIAL_H */