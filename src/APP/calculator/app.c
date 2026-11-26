#include "app.h"
#include "calc.h"
#include "hkeypad.h"
#include "hlcd.h"
#include "rcc.h"
#include <stddef.h>

#define APP_MAX_LENGTH      16U
#define APP_BUFFER_LENGTH   (APP_MAX_LENGTH + 1U)
#define APP_SHIFT_KEY_COUNT 4U
#define APP_INT_STR_LEN     12U
#define APP_LCD_COLS        16U
typedef enum
{
    APP_STATE_TYPING = 0,
    APP_STATE_RESULT,
    APP_STATE_COUNT
} APP_state_t;

typedef enum
{
    APP_ACTION_NONE = 0,
    APP_ACTION_DIGIT,
    APP_ACTION_OPERATOR,
    APP_ACTION_EQUALS,
    APP_ACTION_SHIFT,
    APP_ACTION_LPAREN,
    APP_ACTION_RPAREN,
    APP_ACTION_CLEAR,
    APP_ACTION_BACKSPACE,
    APP_ACTION_COUNT
} APP_action_t;

typedef struct
{
    char     text[APP_BUFFER_LENGTH];
    uint32_t index;
    uint8_t  isChanged;

} APP_buffer_t;

typedef enum
{
    APP_OK = 0,
    APP_BUFFER_FULL,
    APP_BUFFER_EMPTY,
    APP_NULLPTR,
    APP_ERROR_COUNT
} APP_errCode_t;

static APP_buffer_t       APP_buffer                          = {0};
static const APP_action_t APP_shiftLayer[APP_SHIFT_KEY_COUNT] = {
    [('A' - 'A')] = APP_ACTION_LPAREN,
    [('B' - 'A')] = APP_ACTION_RPAREN,
    [('C' - 'A')] = APP_ACTION_CLEAR,
    [('D' - 'A')] = APP_ACTION_BACKSPACE,
};

static const char *const APP_errMsg[CALC_ERROR_COUNT] = {
    [CALC_OK]               = "",
    [CALC_SYNTAX_ERROR]     = "Syntax Error",
    [CALC_NULLPTR]          = "Internal Error",
    [CALC_DIVISION_BY_ZERO] = "Math Error",
    [CALC_OVERFLOW]         = "Overflow",
    [CALC_STACK_FULL]       = "Too Complex",
};

static APP_errCode_t APP_enuBackspace(APP_buffer_t *buffer);
static APP_errCode_t APP_enuAppend(APP_buffer_t *buffer, char data);
static uint8_t       APP_u8IsAppendable(char key);
static void          APP_vIntToString(int32_t num, char *res);
static void          APP_vRedraw(void);

void APP_vInit(void)
{
    if (RCC_enuPeriphClkCtrl(RCC_GPIOA, RCC_ENABLE) != RCC_OK)
    {
        APP_vErrorHandler();
    }
    if (RCC_enuPeriphClkCtrl(RCC_GPIOB, RCC_ENABLE) != RCC_OK)
    {
        APP_vErrorHandler();
    }
    if (HLCD_enuInit() != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    if (HLCD_enuSetCursor(HLCD_MAIN, 0U, 0U) != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    if (HKEYPAD_enuInit() != HKEYPAD_OK)
    {
        APP_vErrorHandler();
    }
}

void APP_vErrorHandler(void)
{
    while (1)
    {
    }
}

void APP_vRunnable(void)
{
    char key = '\0';
    if (HKEYPAD_enuGetKey(HKEYPAD_MAIN, &key) != HKEYPAD_OK)
    {
        APP_vErrorHandler();
    }
    if (APP_u8IsAppendable(key) == 1U)
    {
        APP_errCode_t error = APP_enuAppend(&APP_buffer, key);
        APP_buffer.isChanged = 1U;
    }
    else if (key == APP_KEY_EQUALS)
    {
        int32_t        result = 0;
        char           out[APP_INT_STR_LEN];
        CALC_errCode_t error = CALC_enuEvaluate(APP_buffer.text, &result);
        if (HLCD_enuSetCursor(HLCD_MAIN, 1U, 0U) != HLCD_OK)
        {
            APP_vErrorHandler();
        }
        if (error == CALC_OK)
        {
            APP_vIntToString(result, out);
            if (HLCD_enuWriteString(HLCD_MAIN, out) != HLCD_OK)
            {
                APP_vErrorHandler();
            }
        }
        else if (error < CALC_ERROR_COUNT)
        {
            if (HLCD_enuWriteString(HLCD_MAIN, APP_errMsg[error]) != HLCD_OK)
            {
                APP_vErrorHandler();
            }
        }
    }
    if(APP_buffer.isChanged == 1U){
        APP_vRedraw();
    }
}

static APP_errCode_t APP_enuAppend(APP_buffer_t *buffer, char data)
{
    APP_errCode_t error = APP_OK;
    if (buffer == NULL)
    {
        error = APP_NULLPTR;
    }
    else if (buffer->index >= APP_MAX_LENGTH)
    {
        error = APP_BUFFER_FULL;
    }
    else
    {
        buffer->text[buffer->index] = data;
        buffer->index++;
        buffer->text[buffer->index] = '\0';
    }
    return error;
}

static APP_errCode_t APP_enuBackspace(APP_buffer_t *buffer)
{
    APP_errCode_t error = APP_OK;
    if (buffer == NULL)
    {
        error = APP_NULLPTR;
    }
    else if (buffer->index == 0U)
    {
        error = APP_BUFFER_EMPTY;
    }
    else
    {
        buffer->index--;
        buffer->text[buffer->index] = '\0';
    }
    return error;
}

static uint8_t APP_u8IsAppendable(char key)
{
    uint8_t result = 0U;
    if ((key >= '0') && (key <= '9'))
    {
        result = 1U;
    }
    else if ((key == '+') || (key == '-') || (key == '*') || (key == '/'))
    {
        result = 1U;
    }
    else
    {
        result = 0U;
    }
    return result;
}

static void APP_vIntToString(int32_t num, char *res)
{
    char     temp[APP_INT_STR_LEN];
    uint32_t pos = APP_INT_STR_LEN - 1U;
    uint32_t mag = 0U;
    uint32_t i   = 0U;

    if (res != NULL)
    {
        if (num < 0)
        {
            mag = 0U - (uint32_t)num;
        }
        else
        {
            mag = (uint32_t)num;
        }

        temp[pos] = '\0';
        do
        {
            pos--;
            temp[pos] = (char)('0' + (mag % 10U));
            mag /= 10U;
        } while (mag != 0U);
        if (num < 0)
        {
            pos--;
            temp[pos] = '-';
        }

        while (temp[pos] != '\0')
        {
            res[i] = temp[pos];
            i++;
            pos++;
        }
        res[i] = '\0';
    }
}

static void APP_vRedraw(void)
{
    if (HLCD_enuSetCursor(HLCD_MAIN, 0U, 0U) != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    if (HLCD_enuWriteString(HLCD_MAIN, APP_buffer.text) != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    for (uint32_t i = APP_buffer.index; i < APP_LCD_COLS; ++i)
    {
        HLCD_enuWriteChar(HLCD_MAIN, ' ');
    }

    APP_buffer.isChanged = 0U;
}
