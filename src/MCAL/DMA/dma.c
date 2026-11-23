#include "dma.h"
#include "dma_priv.h"
#include <stddef.h>

static DMA_reg_t *const DMA_base[DMA_COUNT] = 
{
    [DMA_1] = (DMA_reg_t *)DMA_DMA1_BASE,
    [DMA_2] = (DMA_reg_t *)DMA_DMA2_BASE,
};

static const uint8_t DMA_flagPos[4] = {0U, 6U, 16U, 22U};

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
        while(s->CR & (1U << DMA_SXCR_EN_POS)){
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
        s->CR |= (1U << DMA_SXCR_EN_POS);

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
