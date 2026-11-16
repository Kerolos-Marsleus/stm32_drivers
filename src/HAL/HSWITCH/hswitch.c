#include "hswitch.h"
#include <stddef.h>
#define HSWITCH_STABLE_READINGS     5U
#define HSWITCH_HISTORY_MASK        ((1U << HSWITCH_STABLE_READINGS) -1U)

_Static_assert(HSWITCH_STABLE_READINGS <= 8U, "history is a uint8_t: max 8 readings");

extern const HSWITCH_cfg_t HSWITCH_cfg[HSWITCH_COUNT];
static HSWITCH_state_t HSWITCH_state[HSWITCH_COUNT];        /* debounced state reported to the app   */
static uint8_t HSWITCH_u8History[HSWITCH_COUNT];            /* last readings, newest in bit 0 */
HSWITCH_errCode_t   HSWITCH_enuInit(void){
    HSWITCH_errCode_t error = HSWITCH_OK;
    for(uint32_t i = 0U;(i < HSWITCH_COUNT) && (error == HSWITCH_OK); i++){
        GPIO_pinCfg_t pinCfg = 
        {
            .port       = HSWITCH_cfg[i].port,
            .pin        = HSWITCH_cfg[i].pin,
            .outType    = GPIO_OUTPUT_PUSH_PULL,
            .speed      = GPIO_SPEED_LOW,       /* ignored at input mode*/
            .mode       = GPIO_MODE_INPUT,
            .pupd       = HSWITCH_cfg[i].pupd,
            .af         = GPIO_AF0,             /* ignored at input mode*/
            .initState  = GPIO_PIN_STATE_LOW    /* ignored at input mode*/
        };
        if(GPIO_enuPinInit(&pinCfg) != GPIO_OK){
            error = HSWITCH_GPIO_ERROR;
        }
    }
    return error;
}

void HSWITCH_vRunnable(void){
    for(uint32_t i = 0U; i < HSWITCH_COUNT; i++){
        GPIO_pinState_t level;
        if(GPIO_enuPinRead(HSWITCH_cfg[i].port,HSWITCH_cfg[i].pin,&level) == GPIO_OK){
            uint8_t bit = (level == HSWITCH_cfg[i].pressedState) ? 1U : 0U;
            HSWITCH_u8History[i] = (uint8_t)((((uint32_t)HSWITCH_u8History[i] << 1U) | bit) & HSWITCH_HISTORY_MASK);
            switch (HSWITCH_u8History[i])
            {
            case 0U:
                HSWITCH_state[i] = HSWITCH_STATE_RELEASED;
                break;
            case HSWITCH_HISTORY_MASK:
                HSWITCH_state[i] = HSWITCH_STATE_PRESSED;
                break;
            default:
                if(HSWITCH_state[i] == HSWITCH_STATE_RELEASED){
                    HSWITCH_state[i] = HSWITCH_STATE_PRE_PRESSED;
                }else if (HSWITCH_state[i] == HSWITCH_STATE_PRESSED){
                    HSWITCH_state[i] = HSWITCH_STATE_PRE_RELEASED;
                }else{
                    /* already in a PRE_ state: keep it until the history is stable */
                }
                break;
            }
        }  
    }
}


HSWITCH_errCode_t   HSWITCH_enuReadState(HSWITCH_id_t id,HSWITCH_state_t* ptr_state){
    HSWITCH_errCode_t error = HSWITCH_OK;
    if((uint32_t)id >= HSWITCH_COUNT){
        error = HSWITCH_INVALID_ID;
    }else if(ptr_state == NULL){
        error = HSWITCH_NULLPTR;
    }else{
        *ptr_state = HSWITCH_state[id];
    }
    return error;
}
