#ifndef CALC_H
#define CALC_H

#include <stdint.h>

#define CALC_STACK_SIZE              16U


typedef enum{
    CALC_OK = 0,
    CALC_SYNTAX_ERROR,
    CALC_NULLPTR,
    CALC_DIVISION_BY_ZERO,
    CALC_OVERFLOW,
    CALC_STACK_FULL,
    CALC_ERROR_COUNT
}CALC_errCode_t;

CALC_errCode_t CALC_enuEvaluate(const char *expression, int32_t* result);

#endif /* CALC_H */