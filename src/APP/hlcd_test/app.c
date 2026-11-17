#include "app.h"
#include "hled.h"
#include "rcc.h"
#include "board.h"
#include "hlcd.h"

#define APP_UPTIME_ROW          1U
#define APP_UPTIME_COL          4U 
#define APP_SEC_PER_MIN     60U
#define APP_MIN_PER_HOUR    60U
#define APP_HOURS_WRAP      100U

void APP_vInit(void)
{
    if (RCC_enuPeriphClkCtrl(BOARD_LED_RCC, RCC_ENABLE) != RCC_OK)
    {
        APP_vErrorHandler();
    }
    if (RCC_enuPeriphClkCtrl(RCC_GPIOA, RCC_ENABLE) != RCC_OK)
    {
        APP_vErrorHandler();
    }
    if (RCC_enuPeriphClkCtrl(RCC_GPIOB, RCC_ENABLE) != RCC_OK)
    {
        APP_vErrorHandler();
    }
    if (HLED_enuInit() != HLED_OK)
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
    if (/* ｱ ﾘ ｶ ﾞ ﾄ ｳ _ ｺ ﾞ ｻ ﾞ ｲ ﾏ ｽ */HLCD_enuWriteString(HLCD_MAIN, "\xB1\xD8\xB6\xDE\xC4\xB3 \xBA\xDE\xBB\xDE\xB2\xCF\xBD") != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    if (HLCD_enuSetCursor(HLCD_MAIN, 1U, 0U) != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    if (/* ｱ ﾘ ｶ ﾞ ﾄ ｳ _ ｺ ﾞ ｻ ﾞ ｲ ﾏ ｽ */HLCD_enuWriteString(HLCD_MAIN, "UP: ") != HLCD_OK)
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

static void APP_vTwoDigit(char* dest, uint8_t val){
    dest[0] = (char)('0' + val / 10);
    dest[1] = (char)('0' + val % 10);
}


void APP_vUptime(void){
    static uint8_t sec = 0U;
    static uint8_t min = 0U;
    static uint8_t hour = 0U;
    static char upTime[] = "00:00:00"; // and \0 at the end
    
    APP_vTwoDigit(&upTime[0],hour);
    APP_vTwoDigit(&upTime[3],min);
    APP_vTwoDigit(&upTime[6],sec);
    if (HLCD_enuSetCursor(HLCD_MAIN, APP_UPTIME_ROW, APP_UPTIME_COL) != HLCD_OK)
    {
        APP_vErrorHandler();
    }
    if (HLCD_enuWriteString(HLCD_MAIN,upTime)!= HLCD_OK)
    {
        APP_vErrorHandler();
    }
    sec++;
    if (sec == APP_SEC_PER_MIN)  { sec = 0U;  min++;  }
    if (min == APP_MIN_PER_HOUR) { min = 0U;  hour++; }
    if (hour == APP_HOURS_WRAP)  { hour = 0U; }
    
}