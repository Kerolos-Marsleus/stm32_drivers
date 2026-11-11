#ifndef STK_CFG_H
#define STK_CFG_H

// possible sysTick clock sources
#define STK_CLK_AHB_DIV8            0U
#define STK_CLK_AHB                 1U
// user choice
#define STK_CLK_SOURCE              STK_CLK_AHB

// Possible STK modes
#define STK_MODE_POLLING            0U
#define STK_MODE_INTERRUPT          1U
// user choice
#define STK_MODE                    STK_MODE_POLLING      



#endif /*STK_CFG_H*/