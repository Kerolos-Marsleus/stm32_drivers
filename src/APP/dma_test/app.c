#include "app.h"
#include "rcc.h"
#include "uart.h"
#include "dma.h"
#include <stddef.h>


#define APP_POSE_COUNT 2U
#define APP_DATA_LEN   16U

static uint8_t APP_rxByte;
static void APP_vRxDone(void);

static uint8_t APP_src[APP_DATA_LEN] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
static uint8_t APP_dst[APP_DATA_LEN];  
static const uint8_t APP_okMsg[]   = "DMA OK\r\n";
static const uint8_t APP_failMsg[] = "DMA FAIL\r\n";
static const uint8_t *APP_result    = APP_failMsg;   /* chosen in APP_vInit */
static uint16_t       APP_resultLen = sizeof(APP_failMsg) - 1U;

static const DMA_cfg_t APP_dmaCfg =
{
    .channel    = DMA_CHANNEL_0,         
    .direction  = DMA_DIR_MEM_TO_MEM,
    .priority   = DMA_PRIORITY_LOW,
    .memSize    = DMA_SIZE_BYTE,
    .periphSize = DMA_SIZE_BYTE,          
    .memInc     = DMA_ENABLE,             
    .periphInc  = DMA_ENABLE,             
    .circular   = DMA_DISABLE,            
};


void APP_vInit(void)
{
    uint16_t remaining = 1U;
    uint8_t  same      = 1U;
    if (RCC_enuPeriphClkCtrl(RCC_GPIOA, RCC_ENABLE) != RCC_OK)
    {
        APP_vErrorHandler();
    }
    if (RCC_enuPeriphClkCtrl(RCC_DMA2, RCC_ENABLE) != RCC_OK)
    {
        APP_vErrorHandler();
    }
    if (UART_enuInit() != UART_OK)
    {
        APP_vErrorHandler();
    }
    if(DMA_enuConfigure(DMA_2,DMA_STREAM_0,&APP_dmaCfg) != DMA_OK)
    {
        APP_vErrorHandler();
    }
    if (DMA_enuStart(DMA_2, DMA_STREAM_0, (uint32_t)APP_src, (uint32_t)APP_dst, APP_DATA_LEN) != DMA_OK)
    {
        APP_vErrorHandler();
    }
    while (remaining != 0U)
    {
        (void)DMA_enuGetRemaining(DMA_2, DMA_STREAM_0, &remaining);
    }
     for (uint32_t i = 0U; i < APP_DATA_LEN; i++)
    {
        if (APP_dst[i] != APP_src[i]) { same = 0U; }
    }
    if (same == 1U)
    {
        APP_result    = APP_okMsg;
        APP_resultLen = sizeof(APP_okMsg) - 1U;
    }
}

void APP_vErrorHandler(void)
{
    while (1)
    {
    }
}

void APP_vReport(void)
{
    (void)UART_enuSendBufAsync(UART_DEBUG, APP_result, APP_resultLen, NULL);
}

// void APP_vUartTest(void){
//     UART_enuSendBufAsync(UART_DEBUG, APP_msg,APP_MSG_LEN,NULL);
// }

static void APP_vRxDone(void) /* runs inside the ISR */
{
    (void)UART_enuSendBufAsync(UART_DEBUG, &APP_rxByte, 1U, NULL);
    (void)UART_enuRecvBufAsync(UART_DEBUG, &APP_rxByte, 1U, APP_vRxDone);
}