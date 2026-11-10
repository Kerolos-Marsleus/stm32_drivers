#ifndef NVIC_PRIV_H
#define NVIC_PRIV_H


#include <stdint.h>
#include <stddef.h>

typedef struct
{
    volatile uint32_t ISER[8];        
    volatile uint32_t RESERVED0[24];  
    volatile uint32_t ICER[8];        
    volatile uint32_t RESERVED1[24];
    volatile uint32_t ISPR[8];        
    volatile uint32_t RESERVED2[24];
    volatile uint32_t ICPR[8];        
    volatile uint32_t RESERVED3[24];
    volatile uint32_t IABR[8];        
    volatile uint32_t RESERVED4[56];  
    volatile uint8_t  IPR[240];         // because it is accessible byte level
    volatile uint32_t RESERVED5[644]; 
    volatile uint32_t STIR;           
} NVIC_Reg_t;

_Static_assert(offsetof(NVIC_Reg_t, ICER) == 0x080U, "NVIC ICER offset");
_Static_assert(offsetof(NVIC_Reg_t, IPR)  == 0x300U, "NVIC IPR offset");
_Static_assert(offsetof(NVIC_Reg_t, STIR) == 0xE00U, "NVIC STIR offset");


#define NVIC_BASE        0xE000E100U

/* NVIC name also exists in CMSIS core_cm4.h: never include CMSIS headers in nvic.c */
#ifdef UNIT_TEST
    extern NVIC_Reg_t g_fakeNVIC;               /* fake registers in RAM for PC tests */
    #define NVIC       (&g_fakeNVIC)
#else
    #define NVIC            ((NVIC_Reg_t *)NVIC_BASE)
#endif

#define NVIC_REG_SHIFT              5U     /* IRQn >> 5  → which ISER/ICER/... register  / 32 */
#define NVIC_BIT_MASK               31U    /* IRQn & 31  → which bit inside it           % 32 */
#define NVIC_1BIT_MASK              1U
#define NVIC_4BIT_SHIFT             4U
#define NVIC_PRIORITY_LEVEL_COUNT   16U

#endif /*NVIC_PRIV_H*/