# stm32_drivers

Bare-metal drivers for the **STM32F401 (Black Pill)** and **STM32F411RE (Nucleo-64)**,
written from the reference manual, with no vendor HAL. Built with PlatformIO (CMSIS framework).

## Layers

| Layer | Answers | Folder | Talks to |
|---|---|---|---|
| **APP**  | *What* should happen? (LED on while the button is pressed) | `src/APP/<app>/`  | HAL, BSP, MCAL |
| **HAL**  | *How* do I use this device? (an LED)                        | `src/HAL/<DRIVER>/` | MCAL |
| **BSP**  | *Where* is it on this board? (PA5, active-high)             | `src/BSP/board.h` | (facts only) |
| **MCAL** | *How* do I drive the chip? (GPIO, RCC registers)            | `src/MCAL/<DRIVER>/` | registers |

Rule: a layer only uses layers **below** it.

## Driver file pattern

| File | Contains | Who edits it |
|---|---|---|
| `x.h`     | public types and function prototypes | driver author |
| `x.c`     | the implementation                   | driver author |
| `x_cfg.h` | the list of things (e.g. LED names)  | project user |
| `x_cfg.c` | their details (port, pin, levels)    | project user |
| `x_priv.h`| register map, masks (MCAL only)      | driver author |

## Drivers

- **MCAL/RCC:** clock sources (HSI, HSE, PLL), system clock switch, bus prescalers,
  peripheral clock enable, reset cause.
- **MCAL/GPIO:** pin mode, output type, speed, pull, alternate function, pin/port read/write, toggle.
- **HAL/HLED:** `HLED_enuInit`, `HLED_enuTurnOn`, `HLED_enuTurnOff`, `HLED_enuToggle`.
  Handles active-high and active-low LEDs.

## Building

Each environment is `<app>_<board>`:

```bash
pio run -e led_button_blackpill -t upload
pio run -e led_button_nucleo    -t upload
pio test -e native              # unit tests on the PC
```

The board is selected at build time: each env's base adds `-D BOARD_BLACKPILL_F401`
or `-D BOARD_NUCLEO_F411`, and `src/BSP/board.h` turns that into pins.

## Boards

| | Black Pill (F401CC) | Nucleo (F411RE) |
|---|---|---|
| User LED | PC13, active-low  | PA5 (LD2), active-high |
| User button | PA0 (KEY), needs internal pull-up | PC13 (B1), pull-up on the board |
| Debugger | external ST-Link (SWDIO, SWCLK, GND, 3.3 V) | on-board ST-LINK/V2-1 |

## How to add an LED

1. `src/HAL/HLED/hled_cfg.h`: add a name above `HLED_COUNT`, e.g. `HLED_BRAKE`.
2. `src/HAL/HLED/hled_cfg.c`: add its row (port, pin, output type, speed, active level).

`HLED_enuInit()` configures it automatically; the app calls `HLED_enuTurnOn(HLED_BRAKE)`.

## How to add an app

1. Create `src/APP/<app>/main.c`.
2. Add an env in `platformio.ini`:
   `build_src_filter = +<MCAL/> +<HAL/> +<APP/<app>/>`