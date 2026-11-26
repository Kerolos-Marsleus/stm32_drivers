#include "calc.h"
#include <stddef.h>

typedef struct
{
    int32_t data[CALC_STACK_SIZE];
    uint32_t top;
} CALC_stack_t;

typedef enum
{
    CALC_OP_ADD = 0,
    CALC_OP_SUB,
    CALC_OP_MUL,
    CALC_OP_DIV,
    CALC_OP_LPAREN,
    CALC_OP_COUNT
} CALC_op_t;

static const uint8_t CALC_precedence[CALC_OP_COUNT] =
    {
        [CALC_OP_ADD] = 1U,
        [CALC_OP_SUB] = 1U,
        [CALC_OP_MUL] = 2U,
        [CALC_OP_DIV] = 2U,
        [CALC_OP_LPAREN] = 0U,
};

static CALC_errCode_t CALC_enuPush(CALC_stack_t *stack, int32_t data);
static CALC_errCode_t CALC_enuPop(CALC_stack_t *stack, int32_t *data);
static CALC_errCode_t CALC_enuTop(const CALC_stack_t *stack, int32_t *data);
static CALC_errCode_t CALC_enuApply(CALC_stack_t *values, CALC_stack_t *ops);
static CALC_errCode_t CALC_enuAdd(int32_t lhs, int32_t rhs, int32_t *res);
static CALC_errCode_t CALC_enuSub(int32_t lhs, int32_t rhs, int32_t *res);
static CALC_errCode_t CALC_enuMul(int32_t lhs, int32_t rhs, int32_t *res);
static CALC_errCode_t CALC_enuDiv(int32_t lhs, int32_t rhs, int32_t *res);
static CALC_errCode_t CALC_enuCharToOp(char c, CALC_op_t *op);

CALC_errCode_t CALC_enuEvaluate(const char *expression, int32_t *result)
{
    CALC_errCode_t error = CALC_OK;
    uint32_t i = 0U;
    if ((expression == NULL) || (result == NULL))
    {
        error = CALC_NULLPTR;
    }
    else
    {
        CALC_stack_t values;
        values.top = 0U;
        CALC_stack_t ops;
        ops.top = 0U;
        while ((expression[i] != '\0') && (error == CALC_OK))
        {
            char c = expression[i];
            if ((c >= '0') && (c <= '9'))
            {
                int32_t num = 0;
                while ((c >= '0') && (c <= '9') && (error == CALC_OK))
                {
                    error = CALC_enuMul(10, num, &num);
                    if (error == CALC_OK)
                    {
                        error = CALC_enuAdd(num, (int32_t)(c - '0'), &num);
                    }
                    ++i;
                    c = expression[i];
                }
                if (error == CALC_OK)
                {
                    error = CALC_enuPush(&values, num);
                }
            }
            else if (c == ')')
            {
                uint8_t keepGoing = 1U;
                while ((error == CALC_OK) && (ops.top > 0U) && (keepGoing == 1U))
                {
                    int32_t top = 0;
                    error = CALC_enuTop(&ops, &top);
                    if (error == CALC_OK)
                    {
                        if (top == (int32_t)CALC_OP_LPAREN)
                        {
                            keepGoing = 0U;
                        }
                        else
                        {
                            error = CALC_enuApply(&values, &ops);
                        }
                    }
                }
                if (error == CALC_OK)
                {
                    if (keepGoing == 0U)
                    {
                        int32_t dummy = 0;
                        error = CALC_enuPop(&ops, &dummy);
                    }
                    else
                    {
                        error = CALC_SYNTAX_ERROR;
                    }
                    i++;
                }
            }
            else
            {
                CALC_op_t op = CALC_OP_ADD;
                error = CALC_enuCharToOp(c, &op);
                if (error == CALC_OK)
                {
                    if (op == CALC_OP_LPAREN)
                    {
                        error = CALC_enuPush(&ops, (int32_t)op);
                    }
                    else
                    {
                        uint8_t keepGoing = 1U;
                        while ((error == CALC_OK) && (ops.top > 0U) && (keepGoing == 1U))
                        {
                            int32_t top = 0;
                            error = CALC_enuTop(&ops, &top);
                            if (error == CALC_OK)
                            {
                                if (CALC_precedence[(uint32_t)top] < CALC_precedence[op])
                                {
                                    keepGoing = 0U;
                                }
                                else
                                {
                                    error = CALC_enuApply(&values, &ops);
                                }
                            }
                        }
                        if (error == CALC_OK)
                        {
                            error = CALC_enuPush(&ops, (int32_t)op);
                        }
                    }
                }
                i++;
            }
        }
        /* end \0 */
        while ((error == CALC_OK) && (ops.top > 0U))
        {
            int32_t top = 0;
            error = CALC_enuTop(&ops, &top);
            if (error == CALC_OK)
            {
                if (top == (int32_t)CALC_OP_LPAREN)
                {
                    error = CALC_SYNTAX_ERROR;
                }
                else
                {
                    error = CALC_enuApply(&values, &ops);
                }
            }
        }
        if (error == CALC_OK)
        {
            if (values.top == 1U)
            {

                error = CALC_enuPop(&values, result);
            }
            else
            {
                error = CALC_SYNTAX_ERROR;
            }
        }
    }
    return error;
}

static CALC_errCode_t CALC_enuPush(CALC_stack_t *stack, int32_t data)
{
    CALC_errCode_t error = CALC_OK;
    if (stack == NULL)
    {
        error = CALC_NULLPTR;
    }
    else if (stack->top >= CALC_STACK_SIZE)
    {
        error = CALC_STACK_FULL;
    }
    else
    {
        stack->data[stack->top] = data;
        stack->top++;
    }
    return error;
}

static CALC_errCode_t CALC_enuPop(CALC_stack_t *stack, int32_t *data)
{
    CALC_errCode_t error = CALC_OK;
    if ((stack == NULL) || (data == NULL))
    {
        error = CALC_NULLPTR;
    }
    else if (stack->top == 0U)
    {
        error = CALC_SYNTAX_ERROR;
    }
    else
    {
        stack->top--;
        *data = stack->data[stack->top];
    }

    return error;
}

static CALC_errCode_t CALC_enuTop(const CALC_stack_t *stack, int32_t *data)
{
    CALC_errCode_t error = CALC_OK;
    if ((stack == NULL) || (data == NULL))
    {
        error = CALC_NULLPTR;
    }
    else if (stack->top == 0U)
    {
        error = CALC_SYNTAX_ERROR;
    }
    else
    {
        *data = stack->data[stack->top - 1U];
    }

    return error;
}

static CALC_errCode_t CALC_enuApply(CALC_stack_t *values, CALC_stack_t *ops)
{
    CALC_errCode_t error = CALC_OK;
    if ((values == NULL) || (ops == NULL))
    {
        error = CALC_NULLPTR;
    }
    else
    {
        int32_t rhs = 0;
        int32_t lhs = 0;
        int32_t op = 0;
        int32_t res = 0;
        error = CALC_enuPop(values, &rhs);
        if (error == CALC_OK)
        {
            error = CALC_enuPop(values, &lhs);
        }
        if (error == CALC_OK)
        {
            error = CALC_enuPop(ops, &op);
        }
        if (error == CALC_OK)
        {
            switch ((CALC_op_t)op)
            {
            case CALC_OP_ADD:
            {
                error = CALC_enuAdd(lhs, rhs, &res);
                break;
            }
            case CALC_OP_SUB:
            {
                error = CALC_enuSub(lhs, rhs, &res);
                break;
            }
            case CALC_OP_MUL:
            {
                error = CALC_enuMul(lhs, rhs, &res);
                break;
            }
            case CALC_OP_DIV:
            {
                error = CALC_enuDiv(lhs, rhs, &res);
                break;
            }
            default:
            {
                error = CALC_SYNTAX_ERROR; /* '(' left over, or garbage */
                break;
            }
            }
        }
        if (error == CALC_OK)
        {
            error = CALC_enuPush(values, res);
        }
    }
    return error;
}

static CALC_errCode_t CALC_enuAdd(int32_t lhs, int32_t rhs, int32_t *res)
{
    CALC_errCode_t error = CALC_OK;

    if (res == NULL)
    {
        error = CALC_NULLPTR;
    }
    else if ((rhs > 0) && (lhs > (INT32_MAX - rhs)))
    {
        error = CALC_OVERFLOW; /* too high */
    }
    else if ((rhs < 0) && (lhs < (INT32_MIN - rhs)))
    {
        error = CALC_OVERFLOW; /* too low */
    }
    else
    {
        *res = lhs + rhs;
    }

    return error;
}

static CALC_errCode_t CALC_enuSub(int32_t lhs, int32_t rhs, int32_t *res)
{
    CALC_errCode_t error = CALC_OK;

    if (res == NULL)
    {
        error = CALC_NULLPTR;
    }
    else if ((rhs < 0) && (lhs > (INT32_MAX + rhs)))
    {
        error = CALC_OVERFLOW; /* minus a negative → goes up */
    }
    else if ((rhs > 0) && (lhs < (INT32_MIN + rhs)))
    {
        error = CALC_OVERFLOW; /* minus a positive → goes down */
    }
    else
    {
        *res = lhs - rhs;
    }

    return error;
}

static CALC_errCode_t CALC_enuMul(int32_t lhs, int32_t rhs, int32_t *res)
{
    CALC_errCode_t error = CALC_OK;
    int64_t wide = (int64_t)lhs * (int64_t)rhs;

    if (res == NULL)
    {
        error = CALC_NULLPTR;
    }
    else if ((wide > INT32_MAX) || (wide < INT32_MIN))
    {
        error = CALC_OVERFLOW;
    }
    else
    {
        *res = (int32_t)wide;
    }

    return error;
}

static CALC_errCode_t CALC_enuDiv(int32_t lhs, int32_t rhs, int32_t *res)
{
    CALC_errCode_t error = CALC_OK;

    if (res == NULL)
    {
        error = CALC_NULLPTR;
    }
    else if (rhs == 0)
    {
        error = CALC_DIVISION_BY_ZERO;
    }
    else if ((lhs == INT32_MIN) && (rhs == -1))
    {
        error = CALC_OVERFLOW; /* -2147483648 / -1 = +2147483648 doesn't fit */
    }
    else
    {
        *res = lhs / rhs;
    }

    return error;
}

static CALC_errCode_t CALC_enuCharToOp(char c, CALC_op_t *op)
{
    CALC_errCode_t error = CALC_OK;

    if (op == NULL)
    {
        error = CALC_NULLPTR;
    }
    else
    {
        switch (c)
        {
        case '+':
        {
            *op = CALC_OP_ADD;
            break;
        }
        case '-':
        {
            *op = CALC_OP_SUB;
            break;
        }
        case '*':
        {
            *op = CALC_OP_MUL;
            break;
        }
        case '/':
        {
            *op = CALC_OP_DIV;
            break;
        }
        case '(':
        {
            *op = CALC_OP_LPAREN;
            break;
        }
        default:
        {
            error = CALC_SYNTAX_ERROR;
            break;
        }
        }
    }

    return error;
}