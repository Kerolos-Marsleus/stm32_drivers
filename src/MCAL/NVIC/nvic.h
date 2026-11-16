#ifndef NVIC_H
#define NVIC_H
#include <stdint.h>

typedef enum
{
    NVIC_OK = 0,
    NVIC_INVALID_IRQ,        /* IRQ number bigger than the chip has      */
    NVIC_INVALID_PRIORITY,   /* priority > 15 (only 4 bits on STM32F4)   */
    NVIC_NULLPTR,            /* Get functions: output pointer is NULL    */
    NVIC_ERROR_COUNT
} NVIC_errCode_t;


typedef enum
{
    NVIC_IRQ_WWDG               = 0,
    NVIC_IRQ_PVD                = 1,
    NVIC_IRQ_TAMP_STAMP         = 2,
    NVIC_IRQ_RTC_WKUP           = 3,
    NVIC_IRQ_FLASH              = 4,
    NVIC_IRQ_RCC                = 5,
    NVIC_IRQ_EXTI0              = 6,
    NVIC_IRQ_EXTI1              = 7,
    NVIC_IRQ_EXTI2              = 8,
    NVIC_IRQ_EXTI3              = 9,
    NVIC_IRQ_EXTI4              = 10,
    NVIC_IRQ_DMA1_STREAM0       = 11,
    NVIC_IRQ_DMA1_STREAM1       = 12,
    NVIC_IRQ_DMA1_STREAM2       = 13,
    NVIC_IRQ_DMA1_STREAM3       = 14,
    NVIC_IRQ_DMA1_STREAM4       = 15,
    NVIC_IRQ_DMA1_STREAM5       = 16,
    NVIC_IRQ_DMA1_STREAM6       = 17,
    NVIC_IRQ_ADC                = 18,
    NVIC_IRQ_EXTI9_5            = 23,
    NVIC_IRQ_TIM1_BRK_TIM9      = 24,
    NVIC_IRQ_TIM1_UP_TIM10      = 25,
    NVIC_IRQ_TIM1_TRG_COM_TIM11 = 26,
    NVIC_IRQ_TIM1_CC            = 27,
    NVIC_IRQ_TIM2               = 28,
    NVIC_IRQ_TIM3               = 29,
    NVIC_IRQ_TIM4               = 30,
    NVIC_IRQ_I2C1_EV            = 31,
    NVIC_IRQ_I2C1_ER            = 32,
    NVIC_IRQ_I2C2_EV            = 33,
    NVIC_IRQ_I2C2_ER            = 34,
    NVIC_IRQ_SPI1               = 35,
    NVIC_IRQ_SPI2               = 36,
    NVIC_IRQ_USART1             = 37,
    NVIC_IRQ_USART2             = 38,
    NVIC_IRQ_EXTI15_10          = 40,
    NVIC_IRQ_RTC_ALARM          = 41,
    NVIC_IRQ_OTG_FS_WKUP        = 42,
    NVIC_IRQ_DMA1_STREAM7       = 47,
    NVIC_IRQ_SDIO               = 49,
    NVIC_IRQ_TIM5               = 50,
    NVIC_IRQ_SPI3               = 51,
    NVIC_IRQ_DMA2_STREAM0       = 56,
    NVIC_IRQ_DMA2_STREAM1       = 57,
    NVIC_IRQ_DMA2_STREAM2       = 58,
    NVIC_IRQ_DMA2_STREAM3       = 59,
    NVIC_IRQ_DMA2_STREAM4       = 60,
    NVIC_IRQ_OTG_FS             = 67,
    NVIC_IRQ_DMA2_STREAM5       = 68,
    NVIC_IRQ_DMA2_STREAM6       = 69,
    NVIC_IRQ_DMA2_STREAM7       = 70,
    NVIC_IRQ_USART6             = 71,
    NVIC_IRQ_I2C3_EV            = 72,
    NVIC_IRQ_I2C3_ER            = 73,
    NVIC_IRQ_FPU                = 81,
    NVIC_IRQ_SPI4               = 84
} NVIC_irq_t;

#define NVIC_IRQ_COUNT   85U   /* positions 0..84 on STM32F401 */


NVIC_errCode_t NVIC_enuEnableIRQ(NVIC_irq_t irq);
NVIC_errCode_t NVIC_enuDisableIRQ(NVIC_irq_t irq);
NVIC_errCode_t NVIC_enuSetPendingIRQ(NVIC_irq_t irq);
NVIC_errCode_t NVIC_enuClearPendingIRQ(NVIC_irq_t irq);
NVIC_errCode_t NVIC_enuGetPendingIRQ(NVIC_irq_t irq, uint8_t *ptr_isPending);
NVIC_errCode_t NVIC_enuSetPriority(NVIC_irq_t irq, uint8_t priority);
NVIC_errCode_t NVIC_enuGetPriority(NVIC_irq_t irq, uint8_t *ptr_priority);

#endif /*NVIC_H*/