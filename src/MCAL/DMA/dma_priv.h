#ifndef DMA_PRIV_H
#define DMA_PRIV_H
#include <stdint.h>

typedef struct
{
    volatile uint32_t CR;      
    volatile uint32_t NDTR;    
    volatile uint32_t PAR;     
    volatile uint32_t M0AR;    
    volatile uint32_t M1AR;    
    volatile uint32_t FCR;     
} DMA_streamReg_t;            

typedef struct
{
    volatile uint32_t LISR;    
    volatile uint32_t HISR;    
    volatile uint32_t LIFCR;   
    volatile uint32_t HIFCR;   
    DMA_streamReg_t   S[8];    
} DMA_reg_t;

#define DMA_DMA1_BASE   0x40026000U
#define DMA_DMA2_BASE   0x40026400U

#define DMA_SXCR_EN_POS      0U
#define DMA_SXCR_EN_MASK     (1U << DMA_SXCR_EN_POS)
#define DMA_SXCR_TEIE_POS    2U    /* transfer error interrupt   */
#define DMA_SXCR_TCIE_POS    4U    /* transfer complete interrupt */
#define DMA_SXCR_TCIE_MASK   (1U << DMA_SXCR_TCIE_POS)
#define DMA_SXCR_DIR_POS     6U   
#define DMA_SXCR_CIRC_POS    8U
#define DMA_SXCR_PINC_POS    9U
#define DMA_SXCR_MINC_POS    10U
#define DMA_SXCR_PSIZE_POS   11U   
#define DMA_SXCR_MSIZE_POS   13U   
#define DMA_SXCR_PL_POS      16U   
#define DMA_SXCR_CHSEL_POS   25U   


#define DMA_FLAG_CLEAR_MASK  0x3DU
#define DMA_FLAG_TCIF_OFFSET   5U   /* inside a stream's 6-bit group */
#define DMA_FLAG_TEIF_OFFSET   3U
#endif /* DMA_PRIV_H */