#ifndef UART_PRIV_H
#define UART_PRIV_H

#include <stdint.h>

typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} UART_reg_t;

#define UART_USART1_BASE 0x40011000U /* APB2 */
#define UART_USART2_BASE 0x40004400U /* APB1 */
#define UART_USART6_BASE 0x40011400U /* APB2 */

#define UART_DIR_RX_MASK        0x1U
#define UART_DIR_TX_MASK        0x2U
#define UART_CLK_HZ             16000000U
#define UART_SR_RXNE_POS        5U   
#define UART_SR_RXNE_MASK       (1U << UART_SR_RXNE_POS)
// #define UART_SR_TC_POS          6U   /* transmission complete           */
#define UART_SR_TXE_POS         7U   
#define UART_SR_TXE_MASK        (1U << UART_SR_TXE_POS)
#define UART_SR_TC_POS          6U
#define UART_SR_TC_MASK         (1U << UART_SR_TC_POS)

// /* CR1 bits */
#define UART_CR1_RE_POS         2U   /* receiver enable                 */
// #define UART_CR1_TE_POS         3U   /* transmitter enable              */
#define UART_CR1_RXNEIE_POS     5U   
#define UART_CR1_RXNEIE_MASK    (1U << UART_CR1_RXNEIE_POS)
#define UART_CR1_TCIE_POS        6U
#define UART_CR1_TCIE_MASK       (1U << UART_CR1_TCIE_POS)
#define UART_CR1_TXEIE_POS      7U   
#define UART_CR1_TXEIE_MASK     (1U << UART_CR1_TXEIE_POS)
#define UART_CR1_PS_POS         9U   
#define UART_CR1_M_POS          12U  /* word length: 0 = 8 bits, 1 = 9  */
#define UART_CR1_UE_POS         13U  /* USART enable                    */
#define UART_CR1_OVER8_POS      15U  /* 0 = oversampling by 16, 1 = by 8 */
#define UART_CR1_CFG_MASK       (1U << UART_CR1_OVER8_POS) | (1U << UART_CR1_M_POS) | (3U << UART_CR1_PS_POS) | (3U << UART_CR1_RE_POS)
// /* CR2 bits */
#define UART_CR2_STOP_POS       12U  
#define UART_CR2_STOP_MASK      (3U << UART_CR2_STOP_POS)

// /* BRR fields */
#define UART_BRR_MANTISSA_POS   4U   
#define UART_BRR_MANTISSA_MAX   4095U
#endif /* UART_PRIV_H */
