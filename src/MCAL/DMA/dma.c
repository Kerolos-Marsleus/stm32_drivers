#include "dma.h"
#include "dma_priv.h"
#include <stddef.h>
#include "nvic.h"

static DMA_reg_t *const DMA_base[DMA_COUNT] = 
{
    [DMA_1] = (DMA_reg_t *)DMA_DMA1_BASE,
    [DMA_2] = (DMA_reg_t *)DMA_DMA2_BASE,
};

static const NVIC_irq_t DMA_irq[DMA_COUNT][DMA_STREAM_COUNT] =
{
    [DMA_1] =
    {
        NVIC_IRQ_DMA1_STREAM0, NVIC_IRQ_DMA1_STREAM1, NVIC_IRQ_DMA1_STREAM2, NVIC_IRQ_DMA1_STREAM3,
        NVIC_IRQ_DMA1_STREAM4, NVIC_IRQ_DMA1_STREAM5, NVIC_IRQ_DMA1_STREAM6, NVIC_IRQ_DMA1_STREAM7,
    },
    [DMA_2] =
    {
        NVIC_IRQ_DMA2_STREAM0, NVIC_IRQ_DMA2_STREAM1, NVIC_IRQ_DMA2_STREAM2, NVIC_IRQ_DMA2_STREAM3,
        NVIC_IRQ_DMA2_STREAM4, NVIC_IRQ_DMA2_STREAM5, NVIC_IRQ_DMA2_STREAM6, NVIC_IRQ_DMA2_STREAM7,
    },
};

static const uint8_t DMA_flagPos[4] = {0U, 6U, 16U, 22U};
static DMA_cbf_t DMA_cbf[DMA_COUNT][DMA_STREAM_COUNT];
DMA_errCode_t DMA_enuConfigure(DMA_id_t dma, DMA_stream_t stream, const DMA_cfg_t *cfg){
    DMA_errCode_t error = DMA_OK;
    if((uint32_t)dma >= DMA_COUNT){
        error = DMA_INVALID_ID;
    }else if((uint32_t) stream >= DMA_STREAM_COUNT){
        error = DMA_INVALID_ID;
    }else if(cfg == NULL){
        error = DMA_NULLPTR;
    }else if((dma == DMA_1) && (cfg->direction == DMA_DIR_MEM_TO_MEM)){
        error = DMA_INVALID_CFG;
    }else{
        DMA_streamReg_t *s = &DMA_base[dma]->S[stream];
        s->CR &= ~(1U << DMA_SXCR_EN_POS); // clear enable
        while((s->CR & DMA_SXCR_EN_MASK) != 0U){
            // wait to become 0
        } 
        uint32_t ctrl = (cfg   -> channel << DMA_SXCR_CHSEL_POS) 
                        | ((uint32_t)cfg->priority << DMA_SXCR_PL_POS)
                        | ((uint32_t)cfg->memSize << DMA_SXCR_MSIZE_POS)
                        | ((uint32_t)cfg->periphSize << DMA_SXCR_PSIZE_POS)
                        | ((uint32_t)cfg->memInc << DMA_SXCR_MINC_POS)
                        | ((uint32_t)cfg->periphInc << DMA_SXCR_PINC_POS)
                        | ((uint32_t)cfg->circular << DMA_SXCR_CIRC_POS)
                        | ((uint32_t)cfg->direction << DMA_SXCR_DIR_POS)
                        | ((uint32_t)1U << DMA_SXCR_TEIE_POS);
        s->CR = ctrl;
    }
    return error;
}

DMA_errCode_t DMA_enuStart(DMA_id_t dma, DMA_stream_t stream, uint32_t src, uint32_t dst, uint16_t count){
    DMA_errCode_t error = DMA_OK;
    if((uint32_t)dma >= DMA_COUNT){
        error = DMA_INVALID_ID;
    }else if((uint32_t) stream >= DMA_STREAM_COUNT){
        error = DMA_INVALID_ID;
    }else if(count == 0U){
        error = DMA_ZERO_COUNT;
    }else if((DMA_base[dma]->S[stream].CR & (1U << DMA_SXCR_EN_POS)) != 0U){
        error = DMA_BUSY;
    }
    else{
        uint32_t clear = (DMA_FLAG_CLEAR_MASK << DMA_flagPos[stream % 4U]);
        if(stream < DMA_STREAM_4){
            DMA_base[dma]->LIFCR = clear;
        }else{
            DMA_base[dma]->HIFCR = clear;
        }
        DMA_streamReg_t *s = &DMA_base[dma]->S[stream];
        uint32_t dir = (s->CR >> DMA_SXCR_DIR_POS) & 0x3U;
        if(dir == (uint32_t)DMA_DIR_MEM_TO_PERIPH ){
            s->PAR = dst;
            s->M0AR = src;
        }else{
            s->PAR = src;
            s->M0AR = dst;
        }
        s->NDTR = count;
        uint32_t cr = s->CR;
        cr &= ~(DMA_SXCR_TCIE_MASK);
        if(DMA_cbf[dma][stream] != NULL){
            cr |= DMA_SXCR_TCIE_MASK;
        }
        cr |= DMA_SXCR_EN_MASK;
        s->CR = cr;
    }
    return error;
}

DMA_errCode_t DMA_enuStop(DMA_id_t dma, DMA_stream_t stream){
    DMA_errCode_t error = DMA_OK;
    if((uint32_t)dma >= DMA_COUNT){
        error = DMA_INVALID_ID;
    }else if((uint32_t) stream >= DMA_STREAM_COUNT){
        error = DMA_INVALID_ID;
    }else{
        DMA_streamReg_t *s = &DMA_base[dma]->S[stream];
        s->CR &= ~(1U << DMA_SXCR_EN_POS | DMA_SXCR_TCIE_MASK); // clear enable
        while((s->CR & DMA_SXCR_EN_MASK) != 0U){
            // wait to become 0
        } 
        uint32_t clear = (DMA_FLAG_CLEAR_MASK << DMA_flagPos[stream % 4U]);
        if(stream < DMA_STREAM_4){
            DMA_base[dma]->LIFCR = clear;
        }else{
            DMA_base[dma]->HIFCR = clear;
        }
    }
    return error;
}


DMA_errCode_t DMA_enuGetRemaining(DMA_id_t dma, DMA_stream_t stream, uint16_t *ptr_count)
{
    DMA_errCode_t error = DMA_OK;
    if ((uint32_t)dma >= DMA_COUNT)
    {
        error = DMA_INVALID_ID;
    }
    else if ((uint32_t)stream >= DMA_STREAM_COUNT)
    {
        error = DMA_INVALID_ID;
    }
    else if (ptr_count == NULL)
    {
        error = DMA_NULLPTR;
    }
    else
    {
        *ptr_count = (uint16_t)DMA_base[dma]->S[stream].NDTR;   /* counts down to 0 */
    }
    return error;
}

DMA_errCode_t DMA_enuSetCallback(DMA_id_t dma, DMA_stream_t stream, DMA_cbf_t cbf){
    DMA_errCode_t error = DMA_OK;
    if ((uint32_t)dma >= DMA_COUNT)
    {
        error = DMA_INVALID_ID;
    }
    else if ((uint32_t)stream >= DMA_STREAM_COUNT)
    {
        error = DMA_INVALID_ID;
    }else {
        DMA_cbf[dma][stream] = cbf;
        if(cbf != NULL){
            if(NVIC_enuEnableIRQ(DMA_irq[dma][stream]) != NVIC_OK){
                error = DMA_NVIC_ERROR;
            }
        }else{
            if(NVIC_enuDisableIRQ(DMA_irq[dma][stream])!= NVIC_OK){
                error = DMA_NVIC_ERROR;
            }
        }
    }
    return error;
}

static void DMA_vIrqHandler(DMA_id_t dma, DMA_stream_t stream){
    uint8_t pos = DMA_flagPos[stream % 4U];
    volatile uint32_t *isr;
    volatile uint32_t *ifcr;
    if (stream < DMA_STREAM_4)
    {
        isr  = &DMA_base[dma]->LISR;
        ifcr = &DMA_base[dma]->LIFCR;
    }
    else
    {
        isr  = &DMA_base[dma]->HISR;
        ifcr = &DMA_base[dma]->HIFCR;
    }
    uint32_t status = *isr;
    if((status & (1U << ((uint32_t)pos + DMA_FLAG_TCIF_OFFSET))) != 0U){
        *ifcr = (1U << ((uint32_t)pos + DMA_FLAG_TCIF_OFFSET));
        if(DMA_cbf[dma][stream] != NULL){
            DMA_cbf[dma][stream]();
        }
    }
    if((status & (1U << ((uint32_t)pos + DMA_FLAG_TEIF_OFFSET))) != 0U){
        *ifcr = (1U << ((uint32_t)pos + DMA_FLAG_TEIF_OFFSET));
    }
}


void DMA1_Stream0_IRQHandler(void) { DMA_vIrqHandler(DMA_1, DMA_STREAM_0); }
void DMA1_Stream1_IRQHandler(void) { DMA_vIrqHandler(DMA_1, DMA_STREAM_1); }
void DMA1_Stream2_IRQHandler(void) { DMA_vIrqHandler(DMA_1, DMA_STREAM_2); }
void DMA1_Stream3_IRQHandler(void) { DMA_vIrqHandler(DMA_1, DMA_STREAM_3); }
void DMA1_Stream4_IRQHandler(void) { DMA_vIrqHandler(DMA_1, DMA_STREAM_4); }
void DMA1_Stream5_IRQHandler(void) { DMA_vIrqHandler(DMA_1, DMA_STREAM_5); }
void DMA1_Stream6_IRQHandler(void) { DMA_vIrqHandler(DMA_1, DMA_STREAM_6); }
void DMA1_Stream7_IRQHandler(void) { DMA_vIrqHandler(DMA_1, DMA_STREAM_7); }

void DMA2_Stream0_IRQHandler(void) { DMA_vIrqHandler(DMA_2, DMA_STREAM_0); }
void DMA2_Stream1_IRQHandler(void) { DMA_vIrqHandler(DMA_2, DMA_STREAM_1); }
void DMA2_Stream2_IRQHandler(void) { DMA_vIrqHandler(DMA_2, DMA_STREAM_2); }
void DMA2_Stream3_IRQHandler(void) { DMA_vIrqHandler(DMA_2, DMA_STREAM_3); }
void DMA2_Stream4_IRQHandler(void) { DMA_vIrqHandler(DMA_2, DMA_STREAM_4); }
void DMA2_Stream5_IRQHandler(void) { DMA_vIrqHandler(DMA_2, DMA_STREAM_5); }
void DMA2_Stream6_IRQHandler(void) { DMA_vIrqHandler(DMA_2, DMA_STREAM_6); }
void DMA2_Stream7_IRQHandler(void) { DMA_vIrqHandler(DMA_2, DMA_STREAM_7); }