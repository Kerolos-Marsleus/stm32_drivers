#ifndef RCC_H
#define RCC_H
#include <stdint.h>

#define RCC_PERIPH(bus, bit)   ((((uint32_t)(bus)) << 5U) | ((uint32_t)(bit)))
#define RCC_RST_BROWNOUT    (1U << 0)   /* CSR bit 25: BORRSTF  */
#define RCC_RST_PIN         (1U << 1)   /* CSR bit 26: PINRSTF  */
#define RCC_RST_POWERON     (1U << 2)   /* CSR bit 27: PORRSTF  */
#define RCC_RST_SOFTWARE    (1U << 3)   /* CSR bit 28: SFTRSTF  */
#define RCC_RST_IWDG        (1U << 4)   /* CSR bit 29: IWDGRSTF */
#define RCC_RST_WWDG        (1U << 5)   /* CSR bit 30: WWDGRSTF */
#define RCC_RST_LOWPOWER    (1U << 6)   /* CSR bit 31: LPWRRSTF */


typedef enum
{
    RCC_PLLP_DIV2 = 0U,
    RCC_PLLP_DIV4,
    RCC_PLLP_DIV6,
    RCC_PLLP_DIV8,
    RCC_PLLP_Count
}RCC_pllP_t;


typedef uint32_t RCC_resetCause_t;
typedef enum
{
    RCC_OK = 0,
    RCC_TIMEOUT,
    RCC_NULLPTR,
    RCC_INVALID_ARGUMENT,
    RCC_BUSY,
    RCC_NOT_READY,
    RCC_UNEXPECTED_STATE,
    RCC_error_count
}RCC_errCode_t;

typedef enum
{
    RCC_CLK_NOT_READY = 0,
    RCC_CLK_READY,
    RCC_STATUS_COUNT
}RCC_clkStatus_t;


typedef enum
{
    RCC_HSI = 0,
    RCC_HSE,
    RCC_PLL,
    RCC_CLK_COUNT
}RCC_clk_t;

typedef enum
{
    RCC_DISABLE = 0,
    RCC_ENABLE,
    RCC_STATE_COUNT
}RCC_on_off_state_t;

typedef enum
{
    RCC_BUS_AHB1 = 0,
    RCC_BUS_AHB2,
    RCC_BUS_APB1,
    RCC_BUS_APB2,
    RCC_BUS_COUNT
} RCC_bus_t;


typedef enum
{
    /* AHB1ENR */
    RCC_GPIOA    = RCC_PERIPH(RCC_BUS_AHB1, 0U),
    RCC_GPIOB    = RCC_PERIPH(RCC_BUS_AHB1, 1U),
    RCC_GPIOC    = RCC_PERIPH(RCC_BUS_AHB1, 2U),
    RCC_GPIOD    = RCC_PERIPH(RCC_BUS_AHB1, 3U),
    RCC_GPIOE    = RCC_PERIPH(RCC_BUS_AHB1, 4U),
    RCC_GPIOH    = RCC_PERIPH(RCC_BUS_AHB1, 7U),
    RCC_CRC      = RCC_PERIPH(RCC_BUS_AHB1, 12U),
    RCC_DMA1     = RCC_PERIPH(RCC_BUS_AHB1, 21U),
    RCC_DMA2     = RCC_PERIPH(RCC_BUS_AHB1, 22U),

    /* AHB2ENR */
    RCC_OTGFS    = RCC_PERIPH(RCC_BUS_AHB2, 7U),

    /* APB1ENR */
    RCC_TIM2     = RCC_PERIPH(RCC_BUS_APB1, 0U),
    RCC_TIM3     = RCC_PERIPH(RCC_BUS_APB1, 1U),
    RCC_TIM4     = RCC_PERIPH(RCC_BUS_APB1, 2U),
    RCC_TIM5     = RCC_PERIPH(RCC_BUS_APB1, 3U),
    RCC_WWDG     = RCC_PERIPH(RCC_BUS_APB1, 11U),
    RCC_SPI2     = RCC_PERIPH(RCC_BUS_APB1, 14U),
    RCC_SPI3     = RCC_PERIPH(RCC_BUS_APB1, 15U),
    RCC_USART2   = RCC_PERIPH(RCC_BUS_APB1, 17U),
    RCC_I2C1     = RCC_PERIPH(RCC_BUS_APB1, 21U),
    RCC_I2C2     = RCC_PERIPH(RCC_BUS_APB1, 22U),
    RCC_I2C3     = RCC_PERIPH(RCC_BUS_APB1, 23U),
    RCC_PWR      = RCC_PERIPH(RCC_BUS_APB1, 28U),

    /* APB2ENR */
    RCC_TIM1     = RCC_PERIPH(RCC_BUS_APB2, 0U),
    RCC_USART1   = RCC_PERIPH(RCC_BUS_APB2, 4U),
    RCC_USART6   = RCC_PERIPH(RCC_BUS_APB2, 5U),
    RCC_ADC1     = RCC_PERIPH(RCC_BUS_APB2, 8U),
    RCC_SDIO     = RCC_PERIPH(RCC_BUS_APB2, 11U),
    RCC_SPI1     = RCC_PERIPH(RCC_BUS_APB2, 12U),
    RCC_SPI4     = RCC_PERIPH(RCC_BUS_APB2, 13U),
    RCC_SYSCFG   = RCC_PERIPH(RCC_BUS_APB2, 14U),
    RCC_TIM9     = RCC_PERIPH(RCC_BUS_APB2, 16U),
    RCC_TIM10    = RCC_PERIPH(RCC_BUS_APB2, 17U),
    RCC_TIM11    = RCC_PERIPH(RCC_BUS_APB2, 18U)
} RCC_periph_t;

typedef enum
{
    RCC_AHB_DIV1   = 0U,    /* 0xxx */
    RCC_AHB_DIV2   = 8U,    /* 1000 */
    RCC_AHB_DIV4   = 9U,    /* 1001 */
    RCC_AHB_DIV8   = 10U,   /* 1010 */
    RCC_AHB_DIV16  = 11U,   /* 1011 */
    RCC_AHB_DIV64  = 12U,   /* 1100 */
    RCC_AHB_DIV128 = 13U,   /* 1101 */
    RCC_AHB_DIV256 = 14U,   /* 1110 */
    RCC_AHB_DIV512 = 15U    /* 1111 */
} RCC_ahbDiv_t;

typedef enum
{
    RCC_APB_DIV1  = 0U,     /* 0xx */
    RCC_APB_DIV2  = 4U,     /* 100 */
    RCC_APB_DIV4  = 5U,     /* 101 */
    RCC_APB_DIV8  = 6U,     /* 110 */
    RCC_APB_DIV16 = 7U      /* 111 */
} RCC_apbDiv_t;


typedef struct
{
    uint32_t M;
    uint32_t N;
    uint32_t Q;
    RCC_pllP_t P;
    RCC_clk_t  src;
}RCC_pllCfg_t;


RCC_errCode_t RCC_enuClkCtrl(RCC_clk_t Clk, RCC_on_off_state_t State);
RCC_errCode_t RCC_enuSysClkSelect(RCC_clk_t Clk);
RCC_errCode_t RCC_enuSysClkGet(RCC_clk_t *Ptr_Clk);
RCC_errCode_t RCC_enuClkStatusGet(RCC_clk_t Clk, RCC_clkStatus_t *ptr_Status);
RCC_errCode_t RCC_enuPeriphClkCtrl(RCC_periph_t Periph, RCC_on_off_state_t State);
RCC_errCode_t RCC_enuResetCauseGet(RCC_resetCause_t *Ptr_Cause);
RCC_errCode_t RCC_enuResetFlagsClear(void);
RCC_errCode_t RCC_enuAHBPrescalerSet(RCC_ahbDiv_t Div);
RCC_errCode_t RCC_enuAPBPrescalerSet(RCC_bus_t Bus, RCC_apbDiv_t Div);
RCC_errCode_t RCC_enuPLLConfig(const RCC_pllCfg_t *Ptr_CFG);
RCC_errCode_t RCC_enuSysClkFreqGet(uint32_t *Ptr_Clk_freq);
#endif  /*RCC_H*/