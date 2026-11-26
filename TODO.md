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
- [ ] Learn the build: platformio.ini, envs, -I, build_src_filter, compile vs link
- [ ] HLCD: robust re-init after MCU-only reset (0x3 ×3, HD44780 fig. 23/24, >4.1 ms after the first)
- RCC: add RCC_enuBusClkFreqGet(bus) (decode HPRE + PPRE1/2 in RCC_CFGR, §6.3.3); UART must use the bus clock, not a #define
- DMA: report transfer errors (TEIF) to the app; HW clears EN on error (RM0368 §9.3.15), currently only the flag is cleared
UART RX: DMA + IDLE detection
## UART
- [ ] SendBreak API (CR1.SBK) — needed for LIN headers
- [ ] EIE (CR3) — report FE/NF/ORE errors during DMA RX
- [ ] Config-selectable interrupt mask (error interrupts only; TXE/TC stay driver-owned)
- [ ] HLCD: intermittent blank LCD at startup — find which start type fails (power-on / reset button / after flash)