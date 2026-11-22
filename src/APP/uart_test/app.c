#include "app.h"
#include "rcc.h"
#include "uart.h"
#include <stddef.h>

#define APP_POSE_COUNT 2U

static uint8_t APP_rxByte;
static void APP_vRxDone(void);
// static const uint8_t APP_msg[] = "HELLO ITI\r\n";
// #define APP_MSG_LEN   (sizeof(APP_msg) - 1U)

static const uint8_t APP_stickman[8] = {0x0E, 0x0E, 0x04, 0x1F, 0x04, 0x0A, 0x11, 0x00};
static const uint8_t APP_stickmanUp[8] = {0x0E, 0x0E, 0x15, 0x0E, 0x04, 0x0A, 0x0A, 0x00};

static const uint8_t *const APP_poses[APP_POSE_COUNT] = {APP_stickman, APP_stickmanUp};

static const uint8_t APP_mystery[8] =
    {
        0x04, 0x04, 0x04, 0x1F, 0x1F, 0x0E, 0x0E, 0x00};

void APP_vInit(void)
{
    if (RCC_enuPeriphClkCtrl(RCC_GPIOA, RCC_ENABLE) != RCC_OK)
    {
        APP_vErrorHandler();
    }
    if (UART_enuInit() != UART_OK)
    {
        APP_vErrorHandler();
    }
    /* */ 
    (void)UART_enuRecvBufAsync(UART_DEBUG, &APP_rxByte, 1U, APP_vRxDone);   /* start listening */
    /*  */
}

void APP_vErrorHandler(void)
{
    while (1)
    {
    }
}

static void APP_vTwoDigit(char *dest, uint8_t val)
{
    dest[0] = (char)('0' + val / 10);
    dest[1] = (char)('0' + val % 10);
}

// void APP_vUartTest(void){
//     UART_enuSendBufAsync(UART_DEBUG, APP_msg,APP_MSG_LEN,NULL);
// }

static void APP_vRxDone(void)          /* runs inside the ISR */
{
    (void)UART_enuSendBufAsync(UART_DEBUG, &APP_rxByte, 1U, NULL);          
    (void)UART_enuRecvBufAsync(UART_DEBUG, &APP_rxByte, 1U, APP_vRxDone);  
}