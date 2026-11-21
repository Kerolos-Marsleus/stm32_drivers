#include "hkeypad.h"
#include "hkeypad_cfg.h"
#include "hkeypad_priv.h"
#include <stddef.h>

typedef enum
{
    HKEYPAD_STATE_RELEASED = 0,
    HKEYPAD_STATE_PRE_PRESSED,
    HKEYPAD_STATE_PRESSED,
    HKEYPAD_STATE_PRE_RELEASED,
    HKEYPAD_STATE_COUNT
} HKEYPAD_state_t;

extern const HKEYPAD_cfg_t HKEYPAD_cfg[HKEYPAD_COUNT];
static HKEYPAD_state_t HKEYPAD_state[HKEYPAD_COUNT][HKEYPAD_KEYS];
static uint8_t HKEYPAD_u8History[HKEYPAD_COUNT][HKEYPAD_KEYS];
static char HKEYPAD_cSavedKey[HKEYPAD_COUNT];
static uint8_t HKEYPAD_u8ActiveRow[HKEYPAD_COUNT];

HKEYPAD_errCode_t HKEYPAD_enuInit(void)
{
    HKEYPAD_errCode_t error = HKEYPAD_OK;
    for (HKEYPAD_id_t id = 0U; (id < HKEYPAD_COUNT) && (error == HKEYPAD_OK); ++id)
    {
        for (uint32_t i = 0U; (i < HKEYPAD_ROWS) && (error == HKEYPAD_OK); ++i)
        {
            GPIO_pinCfg_t pinCfg =
                {
                    .port = HKEYPAD_cfg[id].rows[i].port,
                    .pin = HKEYPAD_cfg[id].rows[i].pin,
                    .outType = GPIO_OUTPUT_OPEN_DRAIN,
                    .af = GPIO_AF0,
                    .speed = GPIO_SPEED_LOW,
                    .mode = GPIO_MODE_OUTPUT,
                    .pupd = GPIO_PUPD_NONE,
                    .initState = GPIO_PIN_STATE_HIGH};
            if (GPIO_enuPinInit(&pinCfg) != GPIO_OK)
            {
                error = HKEYPAD_GPIO_ERROR;
            }
        }
        for (uint32_t i = 0U; (i < HKEYPAD_COLUMNS) && (error == HKEYPAD_OK); ++i)
        {
            GPIO_pinCfg_t pinCfg =
                {
                    .port = HKEYPAD_cfg[id].columns[i].port,
                    .pin = HKEYPAD_cfg[id].columns[i].pin,
                    .outType = GPIO_OUTPUT_PUSH_PULL,
                    .af = GPIO_AF0,
                    .speed = GPIO_SPEED_LOW,
                    .mode = GPIO_MODE_INPUT,
                    .pupd = GPIO_PUPD_UP,
                    .initState = GPIO_PIN_STATE_HIGH};
            if (GPIO_enuPinInit(&pinCfg) != GPIO_OK)
            {
                error = HKEYPAD_GPIO_ERROR;
            }
        }
        if (error == HKEYPAD_OK)
        {
            if (GPIO_enuPinWrite(HKEYPAD_cfg[id].rows[HKEYPAD_u8ActiveRow[id]].port, HKEYPAD_cfg[id].rows[HKEYPAD_u8ActiveRow[id]].pin, GPIO_PIN_STATE_LOW) != GPIO_OK)
            {
                error = HKEYPAD_GPIO_ERROR;
            }
        }
    }
    return error;
}

void HKEYPAD_vRunnable(void)
{
    HKEYPAD_errCode_t error = HKEYPAD_OK;
    for (HKEYPAD_id_t id = 0U; (id < HKEYPAD_COUNT) && (error == HKEYPAD_OK); ++id)
    {
        for (uint32_t i = 0U; (i < HKEYPAD_COLUMNS) && (error == HKEYPAD_OK); ++i)
        {
            GPIO_pinState_t level;
            if (GPIO_enuPinRead(HKEYPAD_cfg[id].columns[i].port, HKEYPAD_cfg[id].columns[i].pin, &level) == GPIO_OK)
            {
                uint8_t bit = (level == HKEYPAD_PRESSED_LEVEL) ? 1U : 0U;
                uint32_t k = (HKEYPAD_u8ActiveRow[id] * HKEYPAD_COLUMNS) + i;
                HKEYPAD_u8History[id][k] = (uint8_t)((((uint32_t)HKEYPAD_u8History[id][k] << 1U) | bit) & HKEYPAD_HISTORY_MASK);
                switch (HKEYPAD_u8History[id][k])
                {
                case 0U:
                {
                    HKEYPAD_state[id][k] = HKEYPAD_STATE_RELEASED;
                    break;
                }
                case HKEYPAD_HISTORY_MASK:
                {
                    if (HKEYPAD_state[id][k] != HKEYPAD_STATE_PRESSED)
                    {
                        HKEYPAD_cSavedKey[id] = HKEYPAD_cfg[id].keymap[HKEYPAD_u8ActiveRow[id]][i];
                    }
                    HKEYPAD_state[id][k] = HKEYPAD_STATE_PRESSED;
                    break;
                }
                default:
                {
                    if (HKEYPAD_state[id][k] == HKEYPAD_STATE_RELEASED)
                    {
                        HKEYPAD_state[id][k] = HKEYPAD_STATE_PRE_PRESSED;
                    }
                    else if (HKEYPAD_state[id][k] == HKEYPAD_STATE_PRESSED)
                    {
                        HKEYPAD_state[id][k] = HKEYPAD_STATE_PRE_RELEASED;
                    }
                    else
                    {
                        /* already in a PRE_ state: keep it until the history is stable */
                    }
                    break;
                }
                }
            }
        }
        if (GPIO_enuPinWrite(HKEYPAD_cfg[id].rows[HKEYPAD_u8ActiveRow[id]].port, HKEYPAD_cfg[id].rows[HKEYPAD_u8ActiveRow[id]].pin, GPIO_PIN_STATE_HIGH) != GPIO_OK)
        {
            error = HKEYPAD_GPIO_ERROR;
        }
        HKEYPAD_u8ActiveRow[id] = (HKEYPAD_u8ActiveRow[id] + 1U) % HKEYPAD_ROWS;
        if (GPIO_enuPinWrite(HKEYPAD_cfg[id].rows[HKEYPAD_u8ActiveRow[id]].port, HKEYPAD_cfg[id].rows[HKEYPAD_u8ActiveRow[id]].pin, GPIO_PIN_STATE_LOW) != GPIO_OK)
        {
            error = HKEYPAD_GPIO_ERROR;
        }
    }
}

HKEYPAD_errCode_t HKEYPAD_enuGetKey(HKEYPAD_id_t id, char *ptr_key)
{
    HKEYPAD_errCode_t error = HKEYPAD_OK;
    if ((uint32_t)id >= HKEYPAD_COUNT)
    {
        error = HKEYPAD_INVALID_ID;
    }
    else if (ptr_key == NULL)
    {
        error = HKEYPAD_NULLPTR;
    }
    else
    {
        *ptr_key = HKEYPAD_cSavedKey[id];
        HKEYPAD_cSavedKey[id] = '\0';
    }
    return error;
}
