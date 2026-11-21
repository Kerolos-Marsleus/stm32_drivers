#include "app.h"
#include "rcc.h"
#include "hlcd.h"
#include "hkeypad.h"

#define APP_POSE_COUNT      2U

static const uint8_t APP_stickman[8]   = { 0x0E, 0x0E, 0x04, 0x1F, 0x04, 0x0A, 0x11, 0x00 };
static const uint8_t APP_stickmanUp[8] = { 0x0E, 0x0E, 0x15, 0x0E, 0x04, 0x0A, 0x0A, 0x00 };

static const uint8_t * const APP_poses[APP_POSE_COUNT] = { APP_stickman, APP_stickmanUp };

static const uint8_t APP_mystery[8] =
    {
        0x04, 0x04, 0x04, 0x1F, 0x1F, 0x0E, 0x0E, 0x00};

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
    if (HLCD_enuWriteString(HLCD_MAIN, "Key = ") != HLCD_OK)
    {
        APP_vErrorHandler();
    }

    if(HKEYPAD_enuInit() != HKEYPAD_OK)
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

static void APP_vTwoDigit(char *dest, uint8_t val)
{
    dest[0] = (char)('0' + val / 10);
    dest[1] = (char)('0' + val % 10);
}

void APP_vShowKey(void){
    char key = '\0';
    if(HKEYPAD_enuGetKey(HKEYPAD_MAIN,&key) != HKEYPAD_OK){
        APP_vErrorHandler();
    }
    if(key != '\0'){
        (void)HLCD_enuSetCursor(HLCD_MAIN,0U,6U);
        (void)HLCD_enuWriteChar(HLCD_MAIN,(uint8_t) key);
    }
}