#include "app.h"
#include "hled.h"
#include "rcc.h"
#include "board.h"
#include "hlcd.h"

#define APP_UPTIME_ROW 1U
#define APP_UPTIME_COL 0U
#define APP_SEC_PER_MIN 60U
#define APP_MIN_PER_HOUR 60U
#define APP_HOURS_WRAP 100U

#define APP_DANCER_SLOT     2U
#define APP_DANCER_ROW      0U
#define APP_DANCER_COL      15U 
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
    if (/* ｱ ﾘ ｶ ﾞ ﾄ ｳ _ ｺ ﾞ ｻ ﾞ ｲ ﾏ ｽ */ HLCD_enuWriteString(HLCD_MAIN, "\xB1\xD8\xB6\xDE\xC4\xB3 \xBA\xDE\xBB\xDE\xB2\xCF\xBD") != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    if (HLCD_enuSetCursor(HLCD_MAIN, 1U, 0U) != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    if (HLCD_enuStoreCustomChar(HLCD_MAIN, 1U, APP_mystery) != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    if (HLCD_enuSetCursor(HLCD_MAIN, 0U, 15U) != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    if (HLCD_enuWriteChar(HLCD_MAIN,1U) != HLCD_OK)
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

void APP_vUptime(void)
{
    static uint8_t sec = 0U;
    static uint8_t min = 0U;
    static uint8_t hour = 0U;
    static char upTime[] = "UP: 00:00:00"; // and \0 at the end
                          //012345678910
    APP_vTwoDigit(&upTime[4], hour);
    APP_vTwoDigit(&upTime[7], min);
    APP_vTwoDigit(&upTime[10], sec);
    if (HLCD_enuSetCursor(HLCD_MAIN, APP_UPTIME_ROW, APP_UPTIME_COL) != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    if (HLCD_enuWriteString(HLCD_MAIN, upTime) != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    sec++;
    if (sec == APP_SEC_PER_MIN)
    {
        sec = 0U;
        min++;
    }
    if (min == APP_MIN_PER_HOUR)
    {
        min = 0U;
        hour++;
    }
    if (hour == APP_HOURS_WRAP)
    {
        hour = 0U;
    }
}


void APP_vDance(void)       /* every 500 ms */
{
    static uint8_t pose = 0U;

    if (HLCD_enuStoreCustomChar(HLCD_MAIN, APP_DANCER_SLOT, APP_poses[pose]) == HLCD_OK)
    {
        pose = (pose + 1U) % APP_POSE_COUNT;
    }
    (void)HLCD_enuSetCursor(HLCD_MAIN, APP_DANCER_ROW, APP_DANCER_COL);
    (void)HLCD_enuWriteChar(HLCD_MAIN, APP_DANCER_SLOT);
}