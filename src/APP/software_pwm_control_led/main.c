#include "board.h"
#include "hled.h"
#include "rcc.h"
#include "stk.h"

#define ELAPSED_10ms                160000U     
#define ELAPSED_1ms                 16000U
#define STK_MAX                     0x00FFFFFFU
#define APP_LEDS                    HLED_PA7 - HLED_PA0 + 1
uint8_t levels[APP_LEDS] = { 0U, 2U, 3U, 5U, 6U, 8U, 9U, 10U };   

static void APP_vErrorHandler(void);
int main(void){
    
    if(RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    if(RCC_enuPeriphClkCtrl(RCC_GPIOA,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    
    if(HLED_enuInit() != HLED_OK) {APP_vErrorHandler();}
    if(STK_enuInit() != STK_OK) {APP_vErrorHandler();}



    if(STK_enuStartFromMax() != STK_OK){APP_vErrorHandler();}
    uint32_t end;
    uint32_t start;
    if(STK_enuGetTicks(&start) != STK_OK){APP_vErrorHandler();}
    while(1){
        if(STK_enuGetTicks(&end) != STK_OK){APP_vErrorHandler();}
        uint32_t elapsed = (start - end) & STK_MAX;
        for(uint32_t i = 0U; i < 8U; i++){
            HLED_id_t led = (HLED_id_t)(HLED_PA0 + i);
            if(elapsed < (levels[i] * ELAPSED_1ms)){
                HLED_enuTurnOn(led);
            }else{
                HLED_enuTurnOff(led);
            }
        }
        if(elapsed > ELAPSED_10ms){
            if(HLED_enuToggle(HLED_STATUS) != HLED_OK){APP_vErrorHandler();}
            start = end;
        }

    }
}


static void APP_vErrorHandler(void){
    while(1){}
}