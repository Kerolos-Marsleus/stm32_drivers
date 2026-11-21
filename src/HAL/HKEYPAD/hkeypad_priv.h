#ifndef HKEYPAD_PRIV_H
#define HKEYPAD_PRIV_H

#define HKEYPAD_STABLE_READINGS     5U
#define HKEYPAD_HISTORY_MASK        ((1U << HKEYPAD_STABLE_READINGS) -1U)
#define HKEYPAD_PRESSED_LEVEL   GPIO_PIN_STATE_LOW   /* active row LOW, columns pulled up */

#endif /*HKEYPAD_PRIV_H*/