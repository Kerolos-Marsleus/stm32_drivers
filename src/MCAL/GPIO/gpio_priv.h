#ifndef GPIO_PRIV_H
#define GPIO_PRIV_H
#include "gpio.h"
#include <stdint.h>

#define GPIO_2BIT_FIELD_WIDTH           2U
#define GPIO_4BIT_FIELD_WIDTH           4U
#define GPIO_BSRR_RESET_OFFSET          16U
#define GPIO_IDR_MASK                   0x1U
#define GPIO_AFR_PINS_PER_REG           8U

#define GPIOA_BASE   0x40020000U
#define GPIOB_BASE   0x40020400U
#define GPIOC_BASE   0x40020800U
#define GPIOD_BASE   0x40020C00U
#define GPIOE_BASE   0x40021000U
#define GPIOH_BASE   0x40021C00U

#ifdef UNIT_TEST
extern GPIO_Reg_t g_fakeGPIO[GPIO_PORT_COUNT];
#define GPIOA   (&g_fakeGPIO[GPIO_PORTA])
#define GPIOB   (&g_fakeGPIO[GPIO_PORTB])
#define GPIOC   (&g_fakeGPIO[GPIO_PORTC])
#define GPIOD   (&g_fakeGPIO[GPIO_PORTD])
#define GPIOE   (&g_fakeGPIO[GPIO_PORTE])
#define GPIOH   (&g_fakeGPIO[GPIO_PORTH])
#else
#define GPIOA   ((GPIO_Reg_t *) GPIOA_BASE)
#define GPIOB   ((GPIO_Reg_t *) GPIOB_BASE)
#define GPIOC   ((GPIO_Reg_t *) GPIOC_BASE)
#define GPIOD   ((GPIO_Reg_t *) GPIOD_BASE)
#define GPIOE   ((GPIO_Reg_t *) GPIOE_BASE)
#define GPIOH   ((GPIO_Reg_t *) GPIOH_BASE)


#endif /*UNIT_TEST*/


typedef struct
{
    volatile uint32_t MODER;     /* 0x00 */
    volatile uint32_t OTYPER;    /* 0x04 */
    volatile uint32_t OSPEEDR;   /* 0x08 */
    volatile uint32_t PUPDR;     /* 0x0C */
    volatile uint32_t IDR;       /* 0x10 */
    volatile uint32_t ODR;       /* 0x14 */
    volatile uint32_t BSRR;      /* 0x18 */
    volatile uint32_t LCKR;      /* 0x1C */
    volatile uint32_t AFRL;      /* 0x20: AF for pins 0-7  */
    volatile uint32_t AFRH;      /* 0x24: AF for pins 8-15 */
} GPIO_Reg_t;

#endif /*GPIO_PRIV_H*/