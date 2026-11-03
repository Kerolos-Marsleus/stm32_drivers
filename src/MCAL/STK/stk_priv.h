#ifndef STK_PRIV_H
#define STK_PRIV_H

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    volatile uint32_t CTRL;         // control and status
    volatile uint32_t LOAD;         // reload value (24 bits)
    volatile uint32_t VAL;          // current value
    volatile uint32_t CALIB;        // calibration
} STK_Reg_t;


_Static_assert(offsetof(STK_Reg_t, CALIB) == 0x0CU, "STK register map mismatch");

#define STK_BASE             0xE000E010U 


// CTRL bits positions
#define STK_CTRL_ENABLE_POS             0U
#define STK_CTRL_TICKINT_POS            1U
#define STK_CTRL_CLKSOURCE_POS          2U
#define STK_CTRL_COUNTFLAG_POS          16U

#define STK_LOAD_MAX                    0x00FFFFFFU

// masks
#define STK_1BIT_MASK                   1U




#ifdef  UNIT_TEST
extern  STK_Reg_t g_fakeSTK;
#define STK     (&g_fakeSTK)
#else
#define STK     ((STK_Reg_t *) STK_BASE)
#endif





#endif /*STK_PRIV_H*/