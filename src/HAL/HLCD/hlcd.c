#include "hlcd.h"
#include "hlcd_priv.h"
extern const HLCD_pinCfg_t HLCD_pinCfg[HLCD_PIN_COUNT];


static HLCD_errCode_t HLCD_enuSendByte(uint8_t byte, GPIO_pinState_t rs);
static HLCD_errCode_t HLCD_enuInitPins(void);

typedef enum
{
    HLCD_INIT_NO_REQ = 0,           /* nobody asked for init yet                 */
    HLCD_INIT_PINS,                 /* configure the 10 GPIO pins                */
    HLCD_INIT_FUNCTION_SET,         /* send 0x38, E = HIGH                       */
    HLCD_INIT_FUNCTION_SET_WAIT,    /* E = LOW -> LCD latches it                 */
    HLCD_INIT_DISPLAY_ON_OFF,       /* send 0x0C                                 */
    HLCD_INIT_DISPLAY_ON_OFF_WAIT,
    HLCD_INIT_DISPLAY_CLEAR,        /* send 0x01                                 */
    HLCD_INIT_DISPLAY_CLEAR_WAIT,
    HLCD_INIT_ENTRY_MODE_SET,       /* send 0x06                                 */
    HLCD_INIT_ENTRY_MODE_SET_WAIT,  /* added: the slide's list has no E = LOW here */
    HLCD_INIT_DONE
} HLCD_initState_t;

static HLCD_initState_t HLCD_initState = HLCD_INIT_NO_REQ;

static HLCD_errCode_t HLCD_enuInitPins(void){
    HLCD_errCode_t error = HLCD_OK;
    for(uint32_t i = 0U; (i< HLCD_PIN_COUNT) && (error == HLCD_OK) ; i++){
        GPIO_pinCfg_t pinCfg = 
        {
            .port           = HLCD_pinCfg[i].port,
            .pin            = HLCD_pinCfg[i].pin,
            .outType        = GPIO_OUTPUT_PUSH_PULL,
            .af             = GPIO_AF0,
            .speed          = GPIO_SPEED_LOW,
            .mode           = GPIO_MODE_OUTPUT,
            .pupd           = GPIO_PUPD_NONE,
            .initState      = GPIO_PIN_STATE_LOW
        };
        if(GPIO_enuPinInit(&pinCfg) != GPIO_OK){
            error = HLCD_GPIO_ERROR;
        }
    }
    return error;
}

HLCD_errCode_t HLCD_enuInit(void){
    HLCD_errCode_t error = HLCD_OK;
    if(HLCD_initState != HLCD_INIT_NO_REQ){
        error = HLCD_BUSY;
    }else{
        HLCD_initState = HLCD_INIT_PINS;
    }
    return error;
}

static HLCD_errCode_t HLCD_enuSendByte(uint8_t byte, GPIO_pinState_t rs){
    HLCD_errCode_t error = HLCD_OK;
    if(GPIO_enuPinWrite(HLCD_pinCfg[HLCD_PIN_RS].port,HLCD_pinCfg[HLCD_PIN_RS].pin,rs) !=GPIO_OK){
        error = HLCD_GPIO_ERROR;
    }
    for(uint32_t i = 0U; (i < HLCD_DATA_PINS) && (error == HLCD_OK); i++){
        GPIO_pinState_t level = (GPIO_pinState_t)((byte >> i) & 1U);
        if(GPIO_enuPinWrite(HLCD_pinCfg[i + HLCD_PIN_D0].port,HLCD_pinCfg[i + HLCD_PIN_D0].pin,level)!=GPIO_OK){
        error = HLCD_GPIO_ERROR;
        }
    }
    if(error == HLCD_OK){
        if(GPIO_enuPinWrite(HLCD_pinCfg[HLCD_PIN_E].port,HLCD_pinCfg[HLCD_PIN_E].pin,GPIO_PIN_STATE_HIGH) != GPIO_OK){
            error = HLCD_GPIO_ERROR;
        }
    }
    return error;
}

void HLCD_vRunnable(void){
    switch(HLCD_initState)
    {
        case HLCD_INIT_PINS:
        {
            if(HLCD_enuInitPins() == HLCD_OK){
                HLCD_initState = HLCD_INIT_FUNCTION_SET;
            }
            break;
        }
        case HLCD_INIT_FUNCTION_SET:
        {
            uint8_t cmd = HLCD_CMD_FUNCTION_SET
                        | (HLCD_DL_8BIT << HLCD_DL_POS)
                        | (HLCD_LINES   << HLCD_N_POS)
                        | (HLCD_FONT    << HLCD_F_POS);
            if (HLCD_enuSendByte(cmd, GPIO_PIN_STATE_LOW) == HLCD_OK)
                {
                    HLCD_initState = HLCD_INIT_FUNCTION_SET_WAIT;
                }
                break;
        }
        case HLCD_INIT_FUNCTION_SET_WAIT:
        {
            if(GPIO_enuPinWrite(HLCD_pinCfg[HLCD_PIN_E].port,HLCD_pinCfg[HLCD_PIN_E].pin,GPIO_PIN_STATE_LOW) == GPIO_OK){
                HLCD_initState = HLCD_INIT_DISPLAY_ON_OFF;
            }
            break;
        }
        case HLCD_INIT_DISPLAY_ON_OFF:
        {
            uint8_t cmd = HLCD_CMD_DISPLAY_CTRL
                        | (HLCD_ON          << HLCD_D_POS)
                        | (HLCD_CURSOR      << HLCD_C_POS)
                        | (HLCD_BLINK       << HLCD_B_POS);
            if (HLCD_enuSendByte(cmd, GPIO_PIN_STATE_LOW) == HLCD_OK)
                {
                    HLCD_initState = HLCD_INIT_DISPLAY_ON_OFF_WAIT;
                }
                break;
        }
        case HLCD_INIT_DISPLAY_ON_OFF_WAIT:
        {
            if(GPIO_enuPinWrite(HLCD_pinCfg[HLCD_PIN_E].port,HLCD_pinCfg[HLCD_PIN_E].pin,GPIO_PIN_STATE_LOW) == GPIO_OK){
                HLCD_initState = HLCD_INIT_DISPLAY_CLEAR;
            }
            break;
        }
        case HLCD_INIT_DISPLAY_CLEAR:
        {
            uint8_t cmd = HLCD_CMD_CLEAR;
            if (HLCD_enuSendByte(cmd, GPIO_PIN_STATE_LOW) == HLCD_OK)
                {
                    HLCD_initState = HLCD_INIT_DISPLAY_CLEAR_WAIT;
                }
                break;
        }
        case HLCD_INIT_DISPLAY_CLEAR_WAIT:
        {
            if(GPIO_enuPinWrite(HLCD_pinCfg[HLCD_PIN_E].port,HLCD_pinCfg[HLCD_PIN_E].pin,GPIO_PIN_STATE_LOW) == GPIO_OK){
                HLCD_initState = HLCD_INIT_ENTRY_MODE_SET;
            }
            break;
        }
        case HLCD_INIT_ENTRY_MODE_SET:
        {
            uint8_t cmd = HLCD_CMD_ENTRY_MODE
                        | (HLCD_ENTRY_DIRECTION         << HLCD_ID_POS)
                        | (HLCD_DISPLAY_SHIFT                  << HLCD_SH_POS);
            if (HLCD_enuSendByte(cmd, GPIO_PIN_STATE_LOW) == HLCD_OK)
                {
                    HLCD_initState = HLCD_INIT_ENTRY_MODE_SET_WAIT;
                }
                break;
        }
        case HLCD_INIT_ENTRY_MODE_SET_WAIT: 
        {
            if(GPIO_enuPinWrite(HLCD_pinCfg[HLCD_PIN_E].port,HLCD_pinCfg[HLCD_PIN_E].pin,GPIO_PIN_STATE_LOW) == GPIO_OK){
                HLCD_initState = HLCD_INIT_DONE;
            }
            break;
        }
        case HLCD_INIT_DONE:
        {
            /* nothing yet: later, the writing machine runs here */
            break;
        }
        default:
        {
            break;
        }
    }
}