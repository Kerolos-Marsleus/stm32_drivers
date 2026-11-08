#include "board.h"
#include "hled.h"
#include "rcc.h"
#include "stk.h"

static void APP_vToggleStatus(void);

int main(void){
    RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE);
    STK_enuInit();
    HLED_enuInit();
    STK_enuSetCallback(APP_vToggleStatus);
    STK_enuStart();
    while(1){
        
    }
}

static void APP_vToggleStatus(void){
    STK_enuNotifyOnMs(1000U);
    HLED_enuToggle(HLED_STATUS);
}
