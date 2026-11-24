#include "app.h"
#include "rcc.h"
#include "uart.h"
#include "dma.h"
#include <stddef.h>

static uint32_t dataRegister;
static uint8_t APP_rxBuf[4];
void APP_vRecieveCallBack(void);
void APP_vTransmitCallBack(void);

static const DMA_cfg_t APP_rxDmaCfg =
{
    .channel    = DMA_CHANNEL_4,         
    .direction  = DMA_DIR_PERIPH_TO_MEM,
    .priority   = DMA_PRIORITY_MEDIUM,
    .memSize    = DMA_SIZE_BYTE,
    .periphSize = DMA_SIZE_BYTE,          
    .memInc     = DMA_ENABLE,             
    .periphInc  = DMA_DISABLE,             
    .circular   = DMA_DISABLE,            
};
static const DMA_cfg_t APP_txDmaCfg =
{
    .channel    = DMA_CHANNEL_4,         
    .direction  = DMA_DIR_MEM_TO_PERIPH,
    .priority   = DMA_PRIORITY_LOW,
    .memSize    = DMA_SIZE_BYTE,
    .periphSize = DMA_SIZE_BYTE,          
    .memInc     = DMA_ENABLE,             
    .periphInc  = DMA_DISABLE,             
    .circular   = DMA_DISABLE,            
};


void APP_vInit(void)
{
    
    if (RCC_enuPeriphClkCtrl(RCC_GPIOA, RCC_ENABLE) != RCC_OK)
    {
        APP_vErrorHandler();
    }
    if (RCC_enuPeriphClkCtrl(RCC_DMA1, RCC_ENABLE) != RCC_OK)
    {
        APP_vErrorHandler();
    }
    if (UART_enuInit() != UART_OK)
    {
        APP_vErrorHandler();
    }
    DMA_enuConfigure(DMA_1,DMA_STREAM_5,&APP_rxDmaCfg);
    DMA_enuConfigure(DMA_1,DMA_STREAM_6,&APP_txDmaCfg);
    DMA_enuSetCallback(DMA_1,DMA_STREAM_5,APP_vRecieveCallBack);
    DMA_enuSetCallback(DMA_1,DMA_STREAM_6,APP_vTransmitCallBack);
    UART_enuGetDataRegAddr(UART_DEBUG,&dataRegister);
    DMA_enuStart(DMA_1,DMA_STREAM_5,dataRegister,(uint32_t)APP_rxBuf,1);
    UART_enuSetDma(UART_DEBUG,UART_DMA_TX_RX);



}

void APP_vErrorHandler(void)
{
    while (1)
    {
    }
}

void APP_vRecieveCallBack(void){
    DMA_enuStart(DMA_1,DMA_STREAM_6,(uint32_t) APP_rxBuf,dataRegister,1);
}

void APP_vTransmitCallBack(void){
    DMA_enuStart(DMA_1,DMA_STREAM_5,dataRegister,(uint32_t)APP_rxBuf,1);

}