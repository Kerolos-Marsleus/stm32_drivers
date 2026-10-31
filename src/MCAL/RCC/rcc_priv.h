#ifndef RCC_PRIV_H
#define RCC_PRIV_H
#include <stdint.h>

#define RCC_HSE_FREQ_HZ         25000000U
#define RCC_HSI_FREQ_HZ         16000000U
#define RCC_PLL_VCO_IN_MIN_HZ   1000000U
#define RCC_PLL_VCO_IN_MAX_HZ   2000000U
#define RCC_PLL_VCO_OUT_MIN_HZ  192000000U
#define RCC_PLL_VCO_OUT_MAX_HZ  432000000U
#define RCC_PLL_MAX_SYS_CLK     84000000U
#define RCC_PLL_MAX_USB         48000000U

#define RCC_CFGR_SWS_POS        2U
#define RCC_CFGR_SWS_MASK       0x3U
#define RCC_CFGR_SW_POS         0U
#define RCC_CFGR_SW_MASK        0x3U
#define RCC_CSR_RSTF_POS        25U
#define RCC_CSR_RMVF_POS        24U
#define RCC_CFGR_HPRE_MASK      0xFU
#define RCC_CFGR_HPRE_POS       4U
#define RCC_CFGR_PPRE_MASK      0x7U
#define RCC_CFGR_PPRE1_POS      10U
#define RCC_CFGR_PPRE2_POS      13U
#define RCC_PLLCFGR_CLEAR_MASK  0x0F437FFFU
#define RCC_PLLCFGR_M_POS       0U
#define RCC_PLLCFGR_M_MASK      0x3FU
#define RCC_PLLCFGR_N_POS       6U
#define RCC_PLLCFGR_N_MASK      0x1FFU
#define RCC_PLLCFGR_P_POS       16U
#define RCC_PLLCFGR_P_MASK      0x3U
#define RCC_PLLCFGR_SRC_POS     22U
#define RCC_PLLCFGR_SRC_MASK    0x1U
#define RCC_PLLCFGR_Q_POS       24U
#define RCC_PLLCFGR_Q_MASK      0xFU 


typedef struct
{
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    uint32_t          RESERVED0[2];
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    uint32_t          RESERVED1[2];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    uint32_t          RESERVED2[2];
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
    uint32_t          RESERVED3[2];
    volatile uint32_t AHB1LPENR;
    volatile uint32_t AHB2LPENR;
    uint32_t          RESERVED4[2];
    volatile uint32_t APB1LPENR;
    volatile uint32_t APB2LPENR;
    uint32_t          RESERVED5[2];
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    uint32_t          RESERVED6[2];
    volatile uint32_t SSCGR;
    volatile uint32_t PLLI2SCFGR;
    uint32_t          RESERVED7;
    volatile uint32_t DCKCFGR;
} RCC_Reg_t;



#ifdef UNIT_TEST
extern RCC_Reg_t g_fakeRCC;
#define RCC (&g_fakeRCC)
#else
#define RCC     ((RCC_Reg_t*) 0x40023800U)
#endif


#endif /* RCC_PRIV_H */