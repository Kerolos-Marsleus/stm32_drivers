#include "uart.h"
#include "uart_priv.h"
#include "uart_cfg.h"
#include "rcc.h"
#include "nvic.h"
#include <stddef.h>

extern const UART_cfg_t UART_cfg[UART_COUNT];
static const RCC_periph_t UART_rccPeriph[UART_CH_COUNT] =
    {
        [UART_CH_1] = RCC_USART1,
        [UART_CH_2] = RCC_USART2,
        [UART_CH_6] = RCC_USART6,
};

static const GPIO_af_t UART_af[UART_CH_COUNT] =
    {
        [UART_CH_1] = GPIO_AF7,
        [UART_CH_2] = GPIO_AF7,
        [UART_CH_6] = GPIO_AF8,
};

static UART_reg_t *const UART_base[UART_CH_COUNT] =
    {
        [UART_CH_1] = (UART_reg_t *)UART_USART1_BASE,
        [UART_CH_2] = (UART_reg_t *)UART_USART2_BASE,
        [UART_CH_6] = (UART_reg_t *)UART_USART6_BASE,
};

static const NVIC_irq_t UART_irq[UART_CH_COUNT] =
    {
        [UART_CH_1] = NVIC_IRQ_USART1,
        [UART_CH_2] = NVIC_IRQ_USART2,
        [UART_CH_6] = NVIC_IRQ_USART6,
};

typedef struct
{
    const uint8_t *buf;
    uint16_t len;
    volatile uint16_t idx;
    UART_cbf_t cbf;
    volatile uint8_t busy;
} UART_txState_t;
static UART_txState_t UART_tx[UART_CH_COUNT];

typedef struct
{
    uint8_t *buf;
    uint16_t len;
    volatile uint16_t idx;
    UART_cbf_t cbf;
    volatile uint8_t busy;
} UART_rxState_t;
static UART_rxState_t UART_rx[UART_CH_COUNT];

UART_errCode_t UART_enuInit(void)
{
    UART_errCode_t error = UART_OK;
    for (UART_id_t id = 0U; (id < UART_COUNT) && (error == UART_OK); ++id)
    {
        UART_CH_t ch = UART_cfg[id].channel;
        if (RCC_enuPeriphClkCtrl(UART_rccPeriph[ch], RCC_ENABLE) != RCC_OK)
        {
            error = UART_RCC_ERROR;
        }
        if ((error == UART_OK) && ((UART_cfg[id].direction & UART_DIR_TX_MASK) != 0U))
        {
            GPIO_pinCfg_t txCfg =
                {
                    .port = UART_cfg[id].txPin.port,
                    .pin = UART_cfg[id].txPin.pin,
                    .outType = GPIO_OUTPUT_PUSH_PULL,
                    .speed = GPIO_SPEED_LOW,
                    .mode = GPIO_MODE_ALTERNATE,
                    .pupd = GPIO_PUPD_NONE,
                    .af = UART_af[ch],
                    .initState = GPIO_PIN_STATE_HIGH, // ignored in AF state
            };
            if (GPIO_enuPinInit(&txCfg) != GPIO_OK)
            {
                error = UART_GPIO_ERROR;
            }
        }
        if ((error == UART_OK) && ((UART_cfg[id].direction & UART_DIR_RX_MASK) != 0U))
        {
            GPIO_pinCfg_t rxCfg =
                {
                    .port = UART_cfg[id].rxPin.port,
                    .pin = UART_cfg[id].rxPin.pin,
                    .outType = GPIO_OUTPUT_PUSH_PULL,
                    .speed = GPIO_SPEED_LOW,
                    .mode = GPIO_MODE_ALTERNATE,
                    .pupd = GPIO_PUPD_UP,
                    .af = UART_af[ch],
                    .initState = GPIO_PIN_STATE_HIGH, // ignored in AF state
            };
            if (GPIO_enuPinInit(&rxCfg) != GPIO_OK)
            {
                error = UART_GPIO_ERROR;
            }
        }
        if (error == UART_OK)
        {
            if (UART_cfg[id].baudRate == 0U)
            {
                error = UART_INVALID_CFG;
            }
            else
            {
                uint32_t s = 8U * (2U - (uint32_t)UART_cfg[id].oversampling);
                uint64_t usartDiv100 = ((uint64_t)UART_CLK_HZ * 100U) / (s * UART_cfg[id].baudRate);
                uint32_t mantissa = (uint32_t)usartDiv100 / 100U;
                uint32_t frac = ((((uint32_t)usartDiv100 % 100U) * s) + 50U) / 100U;
                if (frac >= s)
                {
                    mantissa++;
                    frac = 0U;
                }
                if ((mantissa == 0U) || (mantissa > UART_BRR_MANTISSA_MAX))
                {
                    error = UART_INVALID_CFG;
                }
                else
                {
                    UART_base[ch]->BRR = (mantissa << UART_BRR_MANTISSA_POS) | frac;
                }
            }
        }
        if (error == UART_OK)
        {
            uint32_t ctr2 = UART_base[ch]->CR2;
            ctr2 = (ctr2 & ~UART_CR2_STOP_MASK) |
                   ((uint32_t)UART_cfg[id].stopBits << UART_CR2_STOP_POS);
            UART_base[ch]->CR2 = ctr2;
            uint32_t ctr1 = UART_base[ch]->CR1;
            ctr1 &= ~(UART_CR1_CFG_MASK);
            ctr1 |= ((uint32_t)UART_cfg[id].oversampling << UART_CR1_OVER8_POS) | ((uint32_t)UART_cfg[id].wordLength << UART_CR1_M_POS) | ((uint32_t)UART_cfg[id].parity << UART_CR1_PS_POS) | ((uint32_t)UART_cfg[id].direction << UART_CR1_RE_POS);
            UART_base[ch]->CR1 = ctr1;
            UART_base[ch]->CR1 |= (1U << UART_CR1_UE_POS);
            if (NVIC_enuEnableIRQ(UART_irq[ch]) != NVIC_OK)
            {
                error = UART_NVIC_ERROR;
            }
        }
    }
    return error;
}

UART_errCode_t UART_enuSendBufAsync(UART_id_t id, const uint8_t *buf, uint16_t len, UART_cbf_t cbf)
{
    UART_errCode_t error = UART_OK;
    if ((uint32_t)id >= UART_COUNT)
    {
        error = UART_INVALID_ID;
    }
    else if (buf == NULL)
    {
        error = UART_NULLPTR;
    }
    else if (len == 0U)
    {
        error = UART_INVALID_LEN;
    }
    else if ((UART_cfg[id].direction & UART_DIR_TX_MASK) == 0U)
    {
        error = UART_INVALID_CFG;
    }
    else
    {
        UART_CH_t ch = UART_cfg[id].channel;
        if (UART_tx[ch].busy != 0U)
        {
            error = UART_BUSY;
        }
        else
        {
            UART_tx[ch].buf = buf;
            UART_tx[ch].cbf = cbf;
            UART_tx[ch].len = len;
            UART_tx[ch].idx = 0U;
            UART_tx[ch].busy = 1U;
            UART_base[ch]->CR1 |= (UART_CR1_TXEIE_MASK);
        }
    }
    return error;
}
UART_errCode_t UART_enuRecvBufAsync(UART_id_t id, uint8_t *buf, uint16_t len, UART_cbf_t cbf)
{
    UART_errCode_t error = UART_OK;
    if ((uint32_t)id >= UART_COUNT)
    {
        error = UART_INVALID_ID;
    }
    else if (buf == NULL)
    {
        error = UART_NULLPTR;
    }
    else if (len == 0U)
    {
        error = UART_INVALID_LEN;
    }
    else if ((UART_cfg[id].direction & UART_DIR_RX_MASK) == 0U)
    {
        error = UART_INVALID_CFG;
    }
    else
    {
        UART_CH_t ch = UART_cfg[id].channel;
        if (UART_rx[ch].busy != 0U)
        {
            error = UART_BUSY;
        }
        else
        {
            UART_rx[ch].buf = buf;
            UART_rx[ch].cbf = cbf;
            UART_rx[ch].len = len;
            UART_rx[ch].idx = 0U;
            UART_rx[ch].busy = 1U;
            UART_base[ch]->CR1 |= (UART_CR1_RXNEIE_MASK);
        }
    }
    return error;
}

UART_errCode_t UART_enuSetDma(UART_id_t id, UART_dma_t dma)
{
    UART_errCode_t error = UART_OK;
    if ((uint32_t)id >= UART_COUNT)
    {
        error = UART_INVALID_ID;
    }
    else if (((uint32_t)dma & ~(uint32_t)UART_cfg[id].direction) != 0U)
    {
        error = UART_INVALID_CFG;
    }
    else
    {
        UART_CH_t ch = UART_cfg[id].channel;
        uint32_t cr3 = UART_base[ch]->CR3;
        cr3 &= ~(UART_CR3_DMA_MASK);
        cr3 |= ((uint32_t) dma << UART_CR3_DMAR_POS);
        UART_base[ch]->CR3 = cr3;
    }
    return error;
}

UART_errCode_t UART_enuGetDataRegAddr(UART_id_t id, uint32_t *ptr_addr){
    UART_errCode_t error = UART_OK;
    if ((uint32_t)id >= UART_COUNT)
    {
        error = UART_INVALID_ID;
    }else if (ptr_addr == NULL)
    {
        error = UART_NULLPTR;
    }else{
        UART_CH_t ch = UART_cfg[id].channel;
        *ptr_addr = (uint32_t)&UART_base[ch]->DR;
    }
    return error;
}


static void UART_vIrqHandler(UART_CH_t ch)
{
    // transmission
    if (((UART_base[ch]->CR1 & UART_CR1_TXEIE_MASK) != 0U) &&
        ((UART_base[ch]->SR & UART_SR_TXE_MASK) != 0U))
    {
        if (UART_tx[ch].idx < UART_tx[ch].len)
        {
            UART_base[ch]->DR = UART_tx[ch].buf[UART_tx[ch].idx++];
        }
        if (UART_tx[ch].idx == UART_tx[ch].len)
        {
            UART_base[ch]->CR1 = (UART_base[ch]->CR1 & ~UART_CR1_TXEIE_MASK) | UART_CR1_TCIE_MASK;
        }
    }
    if (((UART_base[ch]->CR1 & UART_CR1_TCIE_MASK) != 0U) &&
        ((UART_base[ch]->SR & UART_SR_TC_MASK) != 0U))
    {
        UART_base[ch]->CR1 &= ~UART_CR1_TCIE_MASK;
        UART_tx[ch].busy = 0U;
        if (UART_tx[ch].cbf != NULL)
        {
            UART_tx[ch].cbf();
        }
    }
    // Receiving
    if (((UART_base[ch]->CR1 & UART_CR1_RXNEIE_MASK) != 0U) &&
        ((UART_base[ch]->SR & UART_SR_RXNE_MASK) != 0U))
    {
        if (UART_rx[ch].idx < UART_rx[ch].len)
        {
            UART_rx[ch].buf[UART_rx[ch].idx++] = (uint8_t)UART_base[ch]->DR;
        }
        if (UART_rx[ch].idx == UART_rx[ch].len)
        {
            UART_base[ch]->CR1 = (UART_base[ch]->CR1 & ~UART_CR1_RXNEIE_MASK);
            UART_rx[ch].busy = 0U;
            if (UART_rx[ch].cbf != NULL)
            {
                UART_rx[ch].cbf();
            }
        }
    }
}

void USART2_IRQHandler(void)
{
    UART_vIrqHandler(UART_CH_2);
}
void USART1_IRQHandler(void)
{
    UART_vIrqHandler(UART_CH_1);
}
void USART6_IRQHandler(void)
{
    UART_vIrqHandler(UART_CH_6);
}