#include "hserial.h"
#include <stddef.h>
extern const HSERIAL_cfg_t HSERIAL_cfg[HSERIAL_COUNT];

// for dma
static HSERIAL_cbf_t HSERIAL_txCbf[HSERIAL_COUNT];
static volatile uint8_t HSERIAL_u8TxBusy[HSERIAL_COUNT];
static HSERIAL_cbf_t HSERIAL_rxCbf[HSERIAL_COUNT];
static volatile uint8_t HSERIAL_u8RxBusy[HSERIAL_COUNT];

static void HSERIAL_vTxDmaDone(uint32_t context);
static void HSERIAL_vRxDmaDone(uint32_t context);

HSERIAL_errCode_t HSERIAL_enuInit(void)
{
    HSERIAL_errCode_t error = HSERIAL_OK;
    for (HSERIAL_id_t id = 0U; (id < HSERIAL_COUNT) && (error == HSERIAL_OK); ++id)
    {
        if (HSERIAL_cfg[id].txMode == HSERIAL_MODE_DMA)
        {
            const DMA_cfg_t txDmaCfg =
                {
                    .channel = HSERIAL_cfg[id].txDma.channel,
                    .direction = DMA_DIR_MEM_TO_PERIPH,
                    .priority = HSERIAL_cfg[id].txDma.priority,
                    .memSize = DMA_SIZE_BYTE,
                    .periphSize = DMA_SIZE_BYTE,
                    .memInc = DMA_ENABLE,
                    .periphInc = DMA_DISABLE,
                    .circular = DMA_DISABLE,
            };
            if (DMA_enuConfigure(HSERIAL_cfg[id].txDma.controller, HSERIAL_cfg[id].txDma.stream, &txDmaCfg) != DMA_OK)
            {
                error = HSERIAL_DMA_ERROR;
            }
            else
            {
                if (DMA_enuSetCallback(HSERIAL_cfg[id].txDma.controller, HSERIAL_cfg[id].txDma.stream, HSERIAL_vTxDmaDone, (uint32_t)id) != DMA_OK)
                {
                    error = HSERIAL_DMA_ERROR;
                }
            }
        }
        if ((HSERIAL_cfg[id].rxMode == HSERIAL_MODE_DMA) && (error == HSERIAL_OK))
        {
            const DMA_cfg_t rxDmaCfg =
                {
                    .channel = HSERIAL_cfg[id].rxDma.channel,
                    .direction = DMA_DIR_PERIPH_TO_MEM,
                    .priority = HSERIAL_cfg[id].rxDma.priority,
                    .memSize = DMA_SIZE_BYTE,
                    .periphSize = DMA_SIZE_BYTE,
                    .memInc = DMA_ENABLE,
                    .periphInc = DMA_DISABLE,
                    .circular = DMA_DISABLE,
            };
            if (DMA_enuConfigure(HSERIAL_cfg[id].rxDma.controller, HSERIAL_cfg[id].rxDma.stream, &rxDmaCfg) != DMA_OK)
            {
                error = HSERIAL_DMA_ERROR;
            }
            else
            {
                if (DMA_enuSetCallback(HSERIAL_cfg[id].rxDma.controller, HSERIAL_cfg[id].rxDma.stream, HSERIAL_vRxDmaDone, (uint32_t)id) != DMA_OK)
                {
                    error = HSERIAL_DMA_ERROR;
                }
            }
        }
        uint32_t dmaBits = 0U;
        if (HSERIAL_cfg[id].txMode == HSERIAL_MODE_DMA)
        {
            dmaBits |= (uint32_t)UART_DMA_TX;
        }
        if (HSERIAL_cfg[id].rxMode == HSERIAL_MODE_DMA)
        {
            dmaBits |= (uint32_t)UART_DMA_RX;
        }
        if (error == HSERIAL_OK)
        {
            if (UART_enuSetDma(HSERIAL_cfg[id].uart, (UART_dma_t)dmaBits) != UART_OK)
            {
                error = HSERIAL_UART_ERROR;
            }
        }
    }
    return error;
}

static void HSERIAL_vTxDmaDone(uint32_t context)
{
    HSERIAL_id_t id = (HSERIAL_id_t)context;
    if ((uint32_t)id < HSERIAL_COUNT)
    {
        HSERIAL_u8TxBusy[id] = 0U;
        if (HSERIAL_txCbf[id] != NULL)
        {
            HSERIAL_txCbf[id]();
        }
    }
}
static void HSERIAL_vRxDmaDone(uint32_t context)
{
    HSERIAL_id_t id = (HSERIAL_id_t)context;
    if ((uint32_t)id < HSERIAL_COUNT)
    {
        HSERIAL_u8RxBusy[id] = 0U;
        if (HSERIAL_rxCbf[id] != NULL)
        {
            HSERIAL_rxCbf[id]();
        }
    }
}

HSERIAL_errCode_t HSERIAL_enuSend(HSERIAL_id_t id, const uint8_t *buf, uint16_t len, HSERIAL_cbf_t cbf)
{
    HSERIAL_errCode_t error = HSERIAL_OK;
    if ((uint32_t)id >= HSERIAL_COUNT)
    {
        error = HSERIAL_INVALID_ID;
    }
    else if (buf == NULL)
    {
        error = HSERIAL_NULLPTR;
    }
    else if (len == 0U)
    {
        error = HSERIAL_INVALID_LEN;
    }
    else if (HSERIAL_cfg[id].txMode == HSERIAL_MODE_INTERRUPT)
    {
        UART_errCode_t uartErr = UART_enuSendBufAsync(HSERIAL_cfg[id].uart, buf, len, cbf);
        if (uartErr == UART_BUSY)
        {
            error = HSERIAL_BUSY;
        }
        else if (uartErr != UART_OK)
        {
            error = HSERIAL_UART_ERROR;
        }
    }
    else
    {
        if (HSERIAL_u8TxBusy[id] != 0U)
        {
            error = HSERIAL_BUSY;
        }
        else
        {
            uint32_t dataRegister;
            HSERIAL_u8TxBusy[id] = 1U;
            HSERIAL_txCbf[id] = cbf;
            if (UART_enuGetDataRegAddr(HSERIAL_cfg[id].uart, &dataRegister) != UART_OK)
            {
                error = HSERIAL_UART_ERROR;
            }
            else
            {
                if (DMA_enuStart(HSERIAL_cfg[id].txDma.controller, HSERIAL_cfg[id].txDma.stream, (uint32_t)buf, dataRegister, len) != DMA_OK)
                {
                    error = HSERIAL_DMA_ERROR;
                }
            }
            if (error != HSERIAL_OK)
            {
                HSERIAL_u8TxBusy[id] = 0U;
            }
        }
    }
    return error;
}

HSERIAL_errCode_t HSERIAL_enuRecv(HSERIAL_id_t id, uint8_t *buf, uint16_t len, HSERIAL_cbf_t cbf)
{
    HSERIAL_errCode_t error = HSERIAL_OK;
    if ((uint32_t)id >= HSERIAL_COUNT)
    {
        error = HSERIAL_INVALID_ID;
    }
    else if (buf == NULL)
    {
        error = HSERIAL_NULLPTR;
    }
    else if (len == 0U)
    {
        error = HSERIAL_INVALID_LEN;
    }
    else if (HSERIAL_cfg[id].rxMode == HSERIAL_MODE_INTERRUPT)
    {
        UART_errCode_t uartErr = UART_enuRecvBufAsync(HSERIAL_cfg[id].uart, buf, len, cbf);
        if (uartErr == UART_BUSY)
        {
            error = HSERIAL_BUSY;
        }
        else if (uartErr != UART_OK)
        {
            error = HSERIAL_UART_ERROR;
        }
    }
    else
    {
        if (HSERIAL_u8RxBusy[id] != 0U)
        {
            error = HSERIAL_BUSY;
        }
        else
        {
            uint32_t dataRegister;
            HSERIAL_u8RxBusy[id] = 1U;
            HSERIAL_rxCbf[id] = cbf;
            if (UART_enuGetDataRegAddr(HSERIAL_cfg[id].uart, &dataRegister) != UART_OK)
            {
                error = HSERIAL_UART_ERROR;
            }
            else
            {
                if (DMA_enuStart(HSERIAL_cfg[id].rxDma.controller, HSERIAL_cfg[id].rxDma.stream, (uint32_t)dataRegister, (uint32_t) buf, len) != DMA_OK)
                {
                    error = HSERIAL_DMA_ERROR;
                }
            }
            if (error != HSERIAL_OK)
            {
                HSERIAL_u8RxBusy[id] = 0U;
            }
        }
    }
    return error;
}
