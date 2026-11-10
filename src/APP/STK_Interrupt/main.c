#include "board.h"
#include "hled.h"
#include "rcc.h"
#include "stk.h"
#include "nvic.h"

static void APP_vToggleStatus(void);

int main(void){
    RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE);
    STK_enuInit();
    HLED_enuInit();
    STK_enuSetCallback(APP_vToggleStatus);
    STK_enuNotifyOnMs(1000U);
    STK_enuStart();
    uint8_t pend = 0;
    NVIC_enuDisableIRQ(NVIC_IRQ_EXTI0);
    NVIC_enuClearPendingIRQ(NVIC_IRQ_EXTI0);
    NVIC_enuSetPendingIRQ(NVIC_IRQ_EXTI0);
    NVIC_enuGetPendingIRQ(NVIC_IRQ_EXTI0,&pend);
    NVIC_enuClearPendingIRQ(NVIC_IRQ_EXTI0);
    NVIC_enuGetPendingIRQ(NVIC_IRQ_EXTI0,&pend);
    NVIC_enuClearPendingIRQ(NVIC_IRQ_EXTI0);
    NVIC_enuSetPendingIRQ(NVIC_IRQ_EXTI0);
    NVIC_enuGetPendingIRQ(NVIC_IRQ_EXTI0,&pend);
    NVIC_enuClearPendingIRQ(NVIC_IRQ_EXTI0);
    NVIC_enuGetPendingIRQ(NVIC_IRQ_EXTI0,&pend);
    while(1){
        
    }
}

static void APP_vToggleStatus(void){
    HLED_enuToggle(HLED_STATUS);
}
