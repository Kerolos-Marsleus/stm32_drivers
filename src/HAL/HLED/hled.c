#include "hled.h"

extern const HLED_cfg_t HLED_cfg[HLED_COUNT];

static GPIO_pinState_t HLED_enuOffState(HLED_id_t id)
{
    return (HLED_cfg[id].activeState == GPIO_PIN_STATE_HIGH) ? GPIO_PIN_STATE_LOW
                                                             : GPIO_PIN_STATE_HIGH;
}

HLED_errCode_t HLED_enuInit(void){
    HLED_errCode_t error = HLED_OK;
    for(uint32_t i = 0U; (i < HLED_COUNT) && (error == HLED_OK); i++){
        GPIO_pinCfg_t pinCfg = 
        {
            .port           = HLED_cfg[i].port,
            .pin            = HLED_cfg[i].pin,
            .outType        = HLED_cfg[i].outType,
            .speed          = HLED_cfg[i].speed,
            .mode           = GPIO_MODE_OUTPUT,
            .pupd           = GPIO_PUPD_NONE,
            .af             = GPIO_AF0,
            .initState      = HLED_enuOffState((HLED_id_t)i),
        };
        if(GPIO_enuPinInit(&pinCfg) != GPIO_OK){
            error = HLED_GPIO_ERROR;
        }
    }
    return error;
}
HLED_errCode_t HLED_enuTurnOn(HLED_id_t id){
    HLED_errCode_t error = HLED_OK;
    if((uint32_t)id >= HLED_COUNT){
        error = HLED_INVALID_ID;
    }else if(GPIO_enuPinWrite(HLED_cfg[id].port,HLED_cfg[id].pin,HLED_cfg[id].activeState) != GPIO_OK){
            error =  HLED_GPIO_ERROR;
    }
    return error;
}
HLED_errCode_t HLED_enuTurnOff(HLED_id_t id){
    HLED_errCode_t error = HLED_OK;
    if((uint32_t)id >= HLED_COUNT){
        error = HLED_INVALID_ID;
    }else if(GPIO_enuPinWrite(HLED_cfg[id].port,HLED_cfg[id].pin,HLED_enuOffState(id)) != GPIO_OK){
            error =  HLED_GPIO_ERROR;
    }
    return error;
}
HLED_errCode_t HLED_enuToggle(HLED_id_t id){
     HLED_errCode_t error = HLED_OK;
    if((uint32_t)id >= HLED_COUNT){
        error = HLED_INVALID_ID;
    }else if(GPIO_enuPinToggle(HLED_cfg[id].port,HLED_cfg[id].pin) != GPIO_OK){
        error =  HLED_GPIO_ERROR;
    }
    return error;
}