#ifndef UART_H
#define UART_H

#include "uart_cfg.h"
#include "gpio.h"
typedef enum
{
    UART_CH_1,
    UART_CH_2,
    UART_CH_6,
    UART_CH_COUNT
}UART_CH_t;

typedef enum
{
    UART_OK = 0,
    UART_NULLPTR,
    UART_INVALID_ID,
    UART_BUSY,
    UART_GPIO_ERROR,
    UART_RCC_ERROR,
    UART_NVIC_ERROR,
    UART_INVALID_CFG,
    UART_INVALID_LEN,
    UART_ERROR_COUNT
}UART_errCode_t;

typedef enum
{
    UART_WORD_8BIT = 0,                 
    UART_WORD_9BIT = 1                  
} UART_wordLength_t;

typedef enum
{
    UART_PARITY_NONE = 0,               
    UART_PARITY_EVEN = 2,               
    UART_PARITY_ODD  = 3                
} UART_parity_t;

typedef enum
{
    UART_STOP_1   = 0,               
    UART_STOP_0_5 = 1,               
    UART_STOP_2   = 2,                
    UART_STOP_1_5 = 3                 
} UART_stopBits_t;

typedef enum
{
    UART_DIR_RX    = 1,                 
    UART_DIR_TX    = 2,               
    UART_DIR_TX_RX = 3                 
} UART_direction_t;

typedef enum
{
    UART_DMA_NONE  = 0,  // when the dma is disables and interrupt enabled 
    UART_DMA_RX    = 1,   
    UART_DMA_TX    = 2,   
    UART_DMA_TX_RX = 3    
} UART_dma_t;

typedef enum
{
    UART_OVERSAMPLING_16 = 0,   
    UART_OVERSAMPLING_8  = 1    
} UART_oversampling_t;

typedef struct
{
    GPIO_port_t port;
    GPIO_pin_t  pin;
} UART_pinCfg_t;

typedef struct
{
    UART_CH_t         channel;          /* which hardware USART  */
    uint32_t          baudRate;        
    UART_wordLength_t wordLength;
    UART_parity_t     parity;
    UART_stopBits_t   stopBits;
    UART_direction_t  direction;
    UART_oversampling_t oversampling;
    UART_pinCfg_t     txPin;
    UART_pinCfg_t     rxPin;
} UART_cfg_t;

typedef void (*UART_cbf_t)(void);
UART_errCode_t UART_enuInit(void);
UART_errCode_t UART_enuSendBufAsync(UART_id_t id, const uint8_t *buf, uint16_t len, UART_cbf_t cbf);
UART_errCode_t UART_enuRecvBufAsync(UART_id_t id, uint8_t *buf, uint16_t len, UART_cbf_t cbf);
UART_errCode_t UART_enuSetDma(UART_id_t id, UART_dma_t dma);
UART_errCode_t UART_enuGetDataRegAddr(UART_id_t id, uint32_t *ptr_addr);
#endif /* UART_H */