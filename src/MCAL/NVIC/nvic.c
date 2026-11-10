#include "nvic.h"
#include "nvic_priv.h"

NVIC_errCode_t NVIC_enuEnableIRQ(NVIC_irq_t irq){
    NVIC_errCode_t error;
    if((uint32_t)irq >= NVIC_IRQ_COUNT ){
        error = NVIC_INVALID_IRQ;
    }else{
        NVIC->ISER[irq >> NVIC_REG_SHIFT] = (1U << (irq & NVIC_BIT_MASK));
        error = NVIC_OK;
    }
    return error;
}

NVIC_errCode_t NVIC_enuDisableIRQ(NVIC_irq_t irq){
    NVIC_errCode_t error;
    if((uint32_t)irq >= NVIC_IRQ_COUNT ){
        error = NVIC_INVALID_IRQ;
    }else{
        NVIC->ICER[irq >> NVIC_REG_SHIFT] = (1U << (irq & NVIC_BIT_MASK));
        error = NVIC_OK;
    }
    return error;
}

NVIC_errCode_t NVIC_enuSetPendingIRQ(NVIC_irq_t irq){
    NVIC_errCode_t error;
    if((uint32_t)irq >= NVIC_IRQ_COUNT ){
        error = NVIC_INVALID_IRQ;
    }else{
        NVIC->ISPR[irq >> NVIC_REG_SHIFT] = (1U << (irq & NVIC_BIT_MASK));
        error = NVIC_OK;
    }
    return error;
}

NVIC_errCode_t NVIC_enuClearPendingIRQ(NVIC_irq_t irq){
    NVIC_errCode_t error;
    if((uint32_t)irq >= NVIC_IRQ_COUNT ){
        error = NVIC_INVALID_IRQ;
    }else{
        NVIC->ICPR[irq >> NVIC_REG_SHIFT] = (1U << (irq & NVIC_BIT_MASK));
        error = NVIC_OK;
    }
    return error;
}

NVIC_errCode_t NVIC_enuGetPendingIRQ(NVIC_irq_t irq, uint8_t *ptr_isPending){
    NVIC_errCode_t error;
    if((uint32_t)irq >= NVIC_IRQ_COUNT ){
        error = NVIC_INVALID_IRQ;
    }else if(ptr_isPending == NULL){
        error = NVIC_NULLPTR;
    }else{
        *ptr_isPending = (uint8_t)(((NVIC->ISPR[irq >> NVIC_REG_SHIFT]) >> (irq & NVIC_BIT_MASK)) & NVIC_1BIT_MASK );
        error = NVIC_OK;
    }
    return error;
}

NVIC_errCode_t NVIC_enuSetPriority(NVIC_irq_t irq, uint8_t priority){
    NVIC_errCode_t error;
    if(priority >= NVIC_PRIORITY_LEVEL_COUNT){
        error = NVIC_INVALID_PRIORITY;
    }else if((uint32_t)irq >= NVIC_IRQ_COUNT ){
        error = NVIC_INVALID_IRQ;
    }else{
        NVIC->IPR[irq] =(uint8_t)((uint32_t)priority << NVIC_4BIT_SHIFT);
        error = NVIC_OK; 
    }
    return error;
}

NVIC_errCode_t NVIC_enuGetPriority(NVIC_irq_t irq, uint8_t *ptr_priority){
    NVIC_errCode_t error;
    if(ptr_priority == NULL){
        error = NVIC_NULLPTR;
    }else if((uint32_t)irq >= NVIC_IRQ_COUNT ){
        error = NVIC_INVALID_IRQ;
    }else{
        *ptr_priority = NVIC->IPR[irq] >> NVIC_4BIT_SHIFT;
        error = NVIC_OK; 
    }
    return error;
}
