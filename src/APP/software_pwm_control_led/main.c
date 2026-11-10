#include "board.h"
#include "hled.h"
#include "rcc.h"
#include "stk.h"

#define ELAPSED_1ms                 16000U
#define ELAPSED_10ms                (ELAPSED_1ms * 10U)     
#define Elapsed_20ms                (ELAPSED_1ms * 20U)
#define STK_MAX                     0x00FFFFFFU
#define APP_LEDS                    (HLED_PA7 - HLED_PA0 + 1)
uint8_t levels[APP_LEDS] = { 10U, 10U, 10U, 10U, 10U, 10U, 10U ,10U};   

typedef enum
{
    APP_BTN_SEL = 0,
    APP_BTN_PLUS,
    APP_BTN_MINUS,
    APP_BTN_COUNT
}APP_btnId_t;

typedef enum
{
    APP_MODE_DEFAULT,
    APP_MODE_SELECT,
    APP_MODE_COUNT
}APP_mode_t;
static const GPIO_pin_t APP_btnPins[APP_BTN_COUNT] = 
    {
        [APP_BTN_SEL] = GPIO_PIN0,
        [APP_BTN_PLUS]= GPIO_PIN1,
        [APP_BTN_MINUS]= GPIO_PIN5,
    };

static void APP_vErrorHandler(void);
int main(void){
    // give clk for each port
    if(RCC_enuPeriphClkCtrl(BOARD_LED_RCC,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    if(RCC_enuPeriphClkCtrl(RCC_GPIOA,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    if(RCC_enuPeriphClkCtrl(RCC_GPIOB,RCC_ENABLE) != RCC_OK){APP_vErrorHandler();}
    
    // initialize hled and stk
    if(HLED_enuInit() != HLED_OK) {APP_vErrorHandler();}
    if(STK_enuInit() != STK_OK) {APP_vErrorHandler();}
    
    // configure the btns
    GPIO_pinCfg_t BTN_cfg = 
    {
        .port = GPIO_PORTB,
        .mode = GPIO_MODE_INPUT,
        .outType = GPIO_OUTPUT_PUSH_PULL,
        .speed = GPIO_SPEED_LOW,
        .pupd = GPIO_PUPD_UP,
        .af = GPIO_AF0,
        .initState = GPIO_PIN_STATE_LOW // init in input is ignored
    };
    for(uint32_t i = 0U; i < APP_BTN_COUNT; i++){
        BTN_cfg.pin = APP_btnPins[i];
        if(GPIO_enuPinInit(&BTN_cfg) != GPIO_OK) {APP_vErrorHandler();}
    }
    // start the stk time
    if(STK_enuStartFromMax() != STK_OK){APP_vErrorHandler();}
    uint32_t end;
    uint32_t start;
    if(STK_enuGetTicks(&start) != STK_OK){APP_vErrorHandler();}
    
    GPIO_pinState_t btnLastSaw[APP_BTN_COUNT];
    GPIO_pinState_t btnStable[APP_BTN_COUNT];
    uint32_t btnTimerStart[APP_BTN_COUNT];
    for(uint32_t i = 0U; i<APP_BTN_COUNT; i++){
        btnLastSaw[i] = GPIO_PIN_STATE_HIGH;
        btnStable[i] = GPIO_PIN_STATE_HIGH;
        btnTimerStart[i] = start;
    }
    
    APP_mode_t mode = APP_MODE_DEFAULT;
    uint32_t Selected_led = 0;
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
            start = end;
        }
        
        GPIO_pinState_t BtnState;
        
        for(uint32_t i = 0U; i < APP_BTN_COUNT;i++){
            GPIO_enuPinRead(GPIO_PORTB,APP_btnPins[i], &BtnState);
            elapsed = (btnTimerStart[i] - end) & STK_MAX;
            if(BtnState != btnLastSaw[i]){
                // something changed
                btnTimerStart[i] = end;
                btnLastSaw[i] = BtnState;
            }else if(elapsed > Elapsed_20ms){
                if(BtnState != btnStable[i]){
                    btnStable[i] = BtnState;
                    if(btnStable[i] == GPIO_PIN_STATE_LOW){
                        // switch case to do the switch or + or -
                        switch (i)
                        {
                        case APP_BTN_SEL:
                            if(mode == APP_MODE_SELECT){
                                if(Selected_led == APP_LEDS-1U){
                                    mode = APP_MODE_DEFAULT;
                                    Selected_led = 0U;
                                }else{
                                    Selected_led++;
                                }
                            
                            }else if (mode == APP_MODE_DEFAULT){
                                mode = APP_MODE_SELECT;
                                Selected_led = 0U;
                            }

                            break;
                        case APP_BTN_PLUS:
                            if(mode == APP_MODE_SELECT){
                                if(levels[Selected_led] >= 10){
                                    levels[Selected_led] = 10;
                                }else{
                                    levels[Selected_led]++;
                                }
                            }else if(mode == APP_MODE_DEFAULT){
                                for(uint32_t i = 0U; i < APP_LEDS; i++){
                                    if(levels[i] >= 0 && levels[i] <10){
                                        levels[i]++;
                                    }
                                }
                            }
                            break;
                        case APP_BTN_MINUS:
                            if(mode == APP_MODE_SELECT){
                                    if(levels[Selected_led] == 0){
                                    levels[Selected_led] = 0;
                                    }else{
                                        levels[Selected_led]--;
                                    }
                                }else if(mode == APP_MODE_DEFAULT){
                                    for(uint32_t led = 0U; led < APP_LEDS; led++){
                                        if(levels[led] > 0 && levels[led] <=10){
                                            levels[led]--;
                                        }
                                    }
                                }
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
        
        if(mode == APP_MODE_SELECT){
            HLED_enuTurnOff(HLED_STATUS);
        }else{
            HLED_enuTurnOn(HLED_STATUS);
        }

    }
}


static void APP_vErrorHandler(void){
    while(1){}
}