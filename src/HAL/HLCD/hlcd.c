#include "hlcd.h"
#include "hlcd_priv.h"
#include <stddef.h>

typedef enum
{
    HLCD_INIT_NO_REQ = 0,        /* nobody asked for init yet                 */
    HLCD_INIT_PINS,                            
    HLCD_INIT_NIBBLE,
    HLCD_INIT_NIBBLE_LATCH,
    HLCD_INIT_DONE
} HLCD_initState_t;

typedef enum
{
    HLCD_DATA_STATE_SEND = 0,
    HLCD_DATA_STATE_LATCH,
    HLCD_DATA_STATE_SEND_LOW, 
    HLCD_DATA_STATE_LATCH_LOW, 
    HLCD_DATA_STATE_COUNT
} HLCD_dataState_t;

typedef struct
{
    uint32_t rsBitmap; /* bit i = RS of buffer[i]: 1 data, 0 command */
    uint8_t buffer[HLCD_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} HLCD_queue_t;

static HLCD_errCode_t HLCD_enuSendByte(HLCD_id_t id, uint8_t byte, GPIO_pinState_t rs);
static HLCD_errCode_t HLCD_enuInitPins(HLCD_id_t id);
static HLCD_errCode_t HLCD_enuQueuePush(HLCD_id_t id, uint8_t byte, GPIO_pinState_t rs);
static HLCD_errCode_t HLCD_enuQueuePop(HLCD_id_t id, uint8_t *byte, GPIO_pinState_t *rs);
static HLCD_errCode_t HLCD_enuLatch(HLCD_id_t id);

static HLCD_initState_t HLCD_initState[HLCD_COUNT];
extern const HLCD_cfg_t HLCD_cfg[HLCD_COUNT];
static HLCD_queue_t HLCD_queue[HLCD_COUNT];
static const uint8_t HLCD_lineBase[] = {
    HLCD_LINE1_ADDR,
    HLCD_LINE2_ADDR};
static HLCD_dataState_t HLCD_dataState[HLCD_COUNT];

static HLCD_errCode_t HLCD_enuInitPins(HLCD_id_t id)
{
    HLCD_errCode_t error = HLCD_OK;
    uint32_t i = HLCD_PIN_D0;
    if (HLCD_cfg[id].mode == HLCD_MODE_4BIT)
    {
        i = HLCD_PIN_D4;
    }
    for (; (i < HLCD_PIN_COUNT) && (error == HLCD_OK); i++)
    {
        GPIO_pinCfg_t pinCfg =
            {
                .port = HLCD_cfg[id].pins[i].port,
                .pin = HLCD_cfg[id].pins[i].pin,
                .outType = GPIO_OUTPUT_PUSH_PULL,
                .af = GPIO_AF0,
                .speed = GPIO_SPEED_LOW,
                .mode = GPIO_MODE_OUTPUT,
                .pupd = GPIO_PUPD_NONE,
                .initState = GPIO_PIN_STATE_LOW};
        if (GPIO_enuPinInit(&pinCfg) != GPIO_OK)
        {
            error = HLCD_GPIO_ERROR;
        }
    }
    return error;
}

static HLCD_errCode_t HLCD_enuLatch(HLCD_id_t id)
{
    HLCD_errCode_t error = HLCD_OK;
    if (GPIO_enuPinWrite(HLCD_cfg[id].pins[HLCD_PIN_E].port,
                         HLCD_cfg[id].pins[HLCD_PIN_E].pin,
                         GPIO_PIN_STATE_LOW) != GPIO_OK)
    {
        error = HLCD_GPIO_ERROR;
    }
    return error;
}

HLCD_errCode_t HLCD_enuInit(void)
{
    HLCD_errCode_t error = HLCD_OK;
    for (HLCD_id_t id = 0U; id < HLCD_COUNT; id++)
    {
        if (HLCD_initState[id] != HLCD_INIT_NO_REQ)
        {
            error = HLCD_BUSY;
        }
        else
        {
            HLCD_initState[id] = HLCD_INIT_PINS;
            if(error == HLCD_OK){
                uint8_t cmd = HLCD_CMD_FUNCTION_SET | (HLCD_cfg[id].mode << HLCD_DL_POS) | (HLCD_cfg[id].lines << HLCD_N_POS) | (HLCD_cfg[id].font << HLCD_F_POS);
                error = HLCD_enuQueuePush(id,cmd,GPIO_PIN_STATE_LOW);
            }
            if(error == HLCD_OK){
                uint8_t cmd = HLCD_CMD_DISPLAY_CTRL | (HLCD_ON << HLCD_D_POS) | (HLCD_cfg[id].cursor << HLCD_C_POS) | (HLCD_cfg[id].blink << HLCD_B_POS);
                error = HLCD_enuQueuePush(id,cmd,GPIO_PIN_STATE_LOW);
            }
            if(error == HLCD_OK){
                uint8_t cmd = HLCD_CMD_CLEAR;
                error = HLCD_enuQueuePush(id,cmd,GPIO_PIN_STATE_LOW);
            }
            if(error == HLCD_OK){
                uint8_t cmd = HLCD_CMD_ENTRY_MODE | (HLCD_cfg[id].entryDirection << HLCD_ID_POS) | (HLCD_cfg[id].displayShift << HLCD_SH_POS);
                error = HLCD_enuQueuePush(id,cmd,GPIO_PIN_STATE_LOW);
            }
        }
    }
    return error;
}

static HLCD_errCode_t HLCD_enuSendByte(HLCD_id_t id, uint8_t byte, GPIO_pinState_t rs)
{
    HLCD_errCode_t error = HLCD_OK;
    if (GPIO_enuPinWrite(HLCD_cfg[id].pins[HLCD_PIN_RS].port, HLCD_cfg[id].pins[HLCD_PIN_RS].pin, rs) != GPIO_OK)
    {
        error = HLCD_GPIO_ERROR;
    }
    uint32_t i = HLCD_PIN_D0;
    if (HLCD_cfg[id].mode == HLCD_MODE_4BIT)
    {
        i = HLCD_PIN_D4;
    }
    for (; (i < HLCD_DATA_PINS) && (error == HLCD_OK); i++)
    {
        GPIO_pinState_t level = (GPIO_pinState_t)((byte >> i) & 1U);
        if (GPIO_enuPinWrite(HLCD_cfg[id].pins[i + HLCD_PIN_D0].port, HLCD_cfg[id].pins[i + HLCD_PIN_D0].pin, level) != GPIO_OK)
        {
            error = HLCD_GPIO_ERROR;
        }
    }
    if (error == HLCD_OK)
    {
        if (GPIO_enuPinWrite(HLCD_cfg[id].pins[HLCD_PIN_E].port, HLCD_cfg[id].pins[HLCD_PIN_E].pin, GPIO_PIN_STATE_HIGH) != GPIO_OK)
        {
            error = HLCD_GPIO_ERROR;
        }
    }
    return error;
}

void HLCD_vRunnable(void)
{
    for (HLCD_id_t id = 0U; id < HLCD_COUNT; id++)
    {
        switch (HLCD_initState[id])
        {
        case HLCD_INIT_PINS:
        {
            if (HLCD_enuInitPins(id) == HLCD_OK)
            {
                HLCD_initState[id] = HLCD_INIT_NIBBLE;
            }
            break;
        }
        case HLCD_INIT_NIBBLE:
        {

        }
        case HLCD_INIT_DONE:
        {
            switch (HLCD_dataState[id])
            {
            case HLCD_DATA_STATE_SEND:
            {
                uint8_t byte;
                GPIO_pinState_t rs;
                if (HLCD_enuQueuePop(id, &byte, &rs) == HLCD_OK)
                {
                    if (HLCD_enuSendByte(id, byte, rs) == HLCD_OK)
                    {
                        HLCD_dataState[id] = HLCD_DATA_STATE_LATCH;
                    }
                }

                break;
            }

            case HLCD_DATA_STATE_LATCH:
            {
                if (HLCD_enuLatch(id) == HLCD_OK)
                {
                    HLCD_dataState[id] = HLCD_DATA_STATE_SEND;
                }
                break;
            }

            default:
                break;
            }
            break;
        }
        default:
        {
            break;
        }
        }
    }
}

static HLCD_errCode_t HLCD_enuQueuePush(HLCD_id_t id, uint8_t byte, GPIO_pinState_t rs)
{
    HLCD_errCode_t error = HLCD_OK;
    if ((uint32_t)id >= HLCD_COUNT)
    {
        error = HLCD_INVALID_LCD;
    }
    else if (HLCD_queue[id].count == HLCD_QUEUE_SIZE)
    {
        error = HLCD_BUSY;
    }
    else
    {
        HLCD_queue_t *lcd = &HLCD_queue[id];
        lcd->buffer[lcd->head] = byte;
        lcd->rsBitmap = ((lcd->rsBitmap & ~(1U << lcd->head)) | ((uint32_t)rs << lcd->head));
        lcd->head = ((lcd->head + 1U) & (HLCD_QUEUE_MASK));
        lcd->count++;
    }
    return error;
}

static HLCD_errCode_t HLCD_enuQueuePop(HLCD_id_t id, uint8_t *byte, GPIO_pinState_t *rs)
{
    HLCD_errCode_t error = HLCD_OK;
    if ((uint32_t)id >= HLCD_COUNT)
    {
        error = HLCD_INVALID_LCD;
    }
    else if (HLCD_queue[id].count == 0U)
    {
        error = HLCD_EMPTY;
    }
    else if ((byte == NULL) || (rs == NULL))
    {
        error = HLCD_NULLPTR;
    }
    else
    {
        HLCD_queue_t *lcd = &HLCD_queue[id];
        *byte = lcd->buffer[lcd->tail];
        *rs = (GPIO_pinState_t)((lcd->rsBitmap >> lcd->tail) & (1U));
        lcd->tail = ((lcd->tail + 1U) & (HLCD_QUEUE_MASK));
        lcd->count--;
    }
    return error;
}

HLCD_errCode_t HLCD_enuWriteChar(HLCD_id_t id, uint8_t byte)
{
    return HLCD_enuQueuePush(id, byte, GPIO_PIN_STATE_HIGH);
}

HLCD_errCode_t HLCD_enuWriteString(HLCD_id_t id, const char *str)
{
    uint32_t i = 0U;
    HLCD_errCode_t error = HLCD_OK;
    if (str == NULL)
    {
        error = HLCD_NULLPTR;
    }
    else
    {
        while ((str[i] != '\0') && (error == HLCD_OK))
        {
            error = HLCD_enuWriteChar(id, (uint8_t)str[i]);
            i++;
        }
    }
    return error;
}

HLCD_errCode_t HLCD_enuSetCursor(HLCD_id_t id, uint8_t row, uint8_t col)
{
    HLCD_errCode_t error = HLCD_OK;
    if ((uint32_t)id >= HLCD_COUNT)
    {
        error = HLCD_INVALID_LCD;
    }
    else if (row > (uint8_t)HLCD_cfg[id].lines)
    {
        error = HLCD_INVALID_POS;
    }
    else if (col >= HLCD_COLUMNS)
    {
        error = HLCD_INVALID_POS;
    }
    else
    {
        uint8_t addr = HLCD_lineBase[row] + col;
        error = HLCD_enuQueuePush(id, (HLCD_CMD_SET_DDRAM_ADDR | addr), GPIO_PIN_STATE_LOW);
    }
    return error;
}

HLCD_errCode_t HLCD_enuStoreCustomChar(HLCD_id_t id, uint8_t index, const uint8_t *pattern)
{
    HLCD_errCode_t error = HLCD_OK;
    if ((uint32_t)id >= HLCD_COUNT)
    {
        error = HLCD_INVALID_LCD;
    }
    else if (index >= HLCD_CGRAM_CHARS)
    {
        error = HLCD_INVALID_INDEX;
    }
    else if (pattern == NULL)
    {
        error = HLCD_NULLPTR;
    }
    else if ((HLCD_QUEUE_SIZE - HLCD_queue[id].count) < (1U + HLCD_CGRAM_BYTES_PER_CHAR))
    {
        error = HLCD_BUSY;
    }
    else
    {
        error = HLCD_enuQueuePush(id, (HLCD_CMD_SET_CGRAM_ADDR | (index << HLCD_CGRAM_INDEX_POS)), GPIO_PIN_STATE_LOW);
        for (uint32_t i = 0U; (i < HLCD_CGRAM_BYTES_PER_CHAR) && (error == HLCD_OK); i++)
        {
            error = HLCD_enuQueuePush(id, pattern[i], GPIO_PIN_STATE_HIGH);
        }
    }
    return error;
}
