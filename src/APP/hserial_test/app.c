#include "app.h"
#include "rcc.h"
#include "uart.h"
#include "dma.h"
#include "hserial.h"
#include <stddef.h>

static uint32_t dataRegister;
static uint8_t APP_rxBuf[4];
void APP_vTxDone(void);
void APP_vRxDone(void);


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

    HSERIAL_enuInit();
    HSERIAL_enuRecv(HSERIAL_DEBUG,APP_rxBuf,4U,APP_vRxDone);


}

void APP_vErrorHandler(void)
{
    while (1)
    {
    }
}

void APP_vTxDone(void){
    HSERIAL_enuRecv(HSERIAL_DEBUG,APP_rxBuf,4U,APP_vRxDone);
}

void APP_vRxDone(void){
   HSERIAL_enuSend(HSERIAL_DEBUG,APP_rxBuf,4U,APP_vTxDone);
}