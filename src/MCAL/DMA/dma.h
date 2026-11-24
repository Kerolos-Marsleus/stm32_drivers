#ifndef DMA_H
#define DMA_H

#include <stdint.h>

typedef enum
{
    DMA_1 = 0,
    DMA_2,
    DMA_COUNT
} DMA_id_t;

typedef enum
{
    DMA_STREAM_0 = 0,
    DMA_STREAM_1,
    DMA_STREAM_2,
    DMA_STREAM_3,
    DMA_STREAM_4,
    DMA_STREAM_5,
    DMA_STREAM_6,
    DMA_STREAM_7,
    DMA_STREAM_COUNT
} DMA_stream_t;

typedef enum
{
    DMA_CHANNEL_0 = 0,
    DMA_CHANNEL_1,
    DMA_CHANNEL_2,
    DMA_CHANNEL_3,
    DMA_CHANNEL_4,
    DMA_CHANNEL_5,
    DMA_CHANNEL_6,
    DMA_CHANNEL_7
} DMA_channel_t; /* CHSEL [27:25] */

typedef enum
{
    DMA_PRIORITY_LOW = 0,
    DMA_PRIORITY_MEDIUM,
    DMA_PRIORITY_HIGH,
    DMA_PRIORITY_VERY_HIGH
} DMA_priority_t; /* PL [17:16]    */

typedef enum
{
    DMA_SIZE_BYTE = 0,  /* 8 bits        */
    DMA_SIZE_HALF_WORD, /* 16 bits       */
    DMA_SIZE_WORD       /* 32 bits       */
} DMA_size_t;           /* MSIZE / PSIZE */

typedef enum
{
    DMA_DIR_PERIPH_TO_MEM = 0,
    DMA_DIR_MEM_TO_PERIPH,
    DMA_DIR_MEM_TO_MEM /* DMA2 only!    */
} DMA_direction_t;     /* DIR [7:6]     */

typedef enum
{
    DMA_DISABLE = 0,
    DMA_ENABLE
} DMA_state_t; /* MINC, PINC, CIRC */

/* ---- one stream's configuration ---- */
typedef struct
{
    DMA_channel_t channel;
    DMA_direction_t direction;
    DMA_priority_t priority;
    DMA_size_t memSize;
    DMA_size_t periphSize;
    DMA_state_t memInc;
    DMA_state_t periphInc;
    DMA_state_t circular;
} DMA_cfg_t;

typedef void (*DMA_cbf_t)(uint32_t context);

typedef enum
{
    DMA_OK = 0,
    DMA_NULLPTR,
    DMA_INVALID_ID,  
    DMA_INVALID_CFG, 
    DMA_BUSY,  
    DMA_NVIC_ERROR, 
    DMA_ZERO_COUNT,    
    DMA_ERROR_COUNT
} DMA_errCode_t;

DMA_errCode_t DMA_enuConfigure(DMA_id_t dma, DMA_stream_t stream, const DMA_cfg_t *cfg);
DMA_errCode_t DMA_enuSetCallback(DMA_id_t dma, DMA_stream_t stream, DMA_cbf_t cbf,uint32_t context);
DMA_errCode_t DMA_enuStart(DMA_id_t dma, DMA_stream_t stream, uint32_t src, uint32_t dst, uint16_t count);
DMA_errCode_t DMA_enuStop(DMA_id_t dma, DMA_stream_t stream);
DMA_errCode_t DMA_enuGetRemaining(DMA_id_t dma, DMA_stream_t stream, uint16_t *ptr_count);
#endif /* DMA_H */