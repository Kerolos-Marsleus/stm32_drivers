## TODO
// software pwm twiks
- [ ] Add hled_cfg.c/.h to every app that uses HLED (led_blink, led_button, switch_led_bar, ...)
- [ ] Decide what apps without HLED do (hled.c is compiled by +<HAL/>)
- [ ] .ini: one `app = <name>` per env, build filter + -I from ${this.app}
- [ ] Move cxx_flags out of build_flags (C++-only flags warn on C files)
- [ ] Fix APP_LEDS parentheses + replace magic 8U
- [ ] STK_enuGetElapsed() so apps don't need STK_MAX
- [ ] Remove `levels >= 0` and `levels <= 10` (always true)
- [ ] `10` → APP_LEVEL_MAX
- [ ] `Elapsed_20ms` → APP_DEBOUNCE_TICKS
- [ ] Break-it: buttons with GPIO_PUPD_NONE
- [ ] Interrupt mode: rewrite STK_enuDelayMs to use STK_u32TimeMs (no LOAD changes); wrap StartFromMax/GetTicks in #if POLLING