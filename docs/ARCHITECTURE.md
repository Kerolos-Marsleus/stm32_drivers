# Architecture

Bare-metal drivers for the **STM32F401CC (Black Pill)**, written from the reference manual (RM0368) and the Cortex-M4 programming manual (PM0214). No HAL library, no CMSIS device headers: every register access lives in this repo.

> GitHub renders the diagrams below directly from the Mermaid source.

---

## 1. Layers

Calls only go **down**. The only way up is a **callback**: a function pointer that a lower layer stores and calls without knowing its name.

```mermaid
flowchart TB
    subgraph APP["APP · src/APP/&lt;app&gt;/"]
        direction LR
        MAIN["main.c<br/>startup only"]
        APPC["app.c<br/>runnables · APP_vInit"]
        CFG["*_cfg.c / *_cfg.h<br/>this app's configuration"]
    end

    subgraph SVC["SERVICES · src/SERVICES/"]
        SCHED["SCHED · time-triggered cooperative scheduler<br/>woken every 1 ms by a callback from STK"]
    end

    subgraph HAL["HAL · src/HAL/"]
        direction LR
        HLED["HLED"]
        HSW["HSWITCH<br/>debounced buttons"]
        HLCD["HLCD<br/>async character LCD"]
    end

    subgraph MCAL["MCAL · src/MCAL/"]
        direction LR
        RCC["RCC"]
        GPIO["GPIO"]
        STK["STK<br/>SysTick"]
        NVIC["NVIC"]
    end

    subgraph HW["HARDWARE"]
        direction LR
        REGS["peripheral registers<br/>0x4002xxxx · 0xE000Exxx"]
        PINS["pins · LEDs · buttons · LCD"]
    end

    APP == "init + API calls" ==> SVC
    SVC == "calls runnables from sched_cfg.c" ==> HAL
    HAL == "GPIO calls" ==> MCAL
    MCAL == "reads / writes registers" ==> HW

    classDef app fill:#F1ECFA,stroke:#6A3FB5,color:#16212C
    classDef svc fill:#E6F4F2,stroke:#0E7C73,color:#16212C
    classDef hal fill:#FBF1E2,stroke:#B06A00,color:#16212C
    classDef mcal fill:#E8F0FB,stroke:#1D5FB8,color:#16212C
    classDef hw fill:#E4E8EC,stroke:#3A4652,color:#16212C
    class MAIN,APPC,CFG app
    class SCHED svc
    class HLED,HSW,HLCD hal
    class RCC,GPIO,STK,NVIC mcal
    class REGS,PINS hw
```

Arrows are simplified to one per layer. In detail: the app also calls HAL and MCAL directly (for example HLED and RCC in `APP_vInit`), SCHED calls the **app's** runnables through the same table, and SCHED uses STK for its tick. The two upward paths are callbacks: STK → SCHED (the tick) and SCHED → runnables. `board.h` (BSP) sits beside the stack and only holds pin names per board.

| Layer | Knows | Never does |
|---|---|---|
| **APP** | what the product does | touch a register |
| **SERVICES** | *when* things run | know what a runnable does |
| **HAL** | devices: LED, switch, LCD | touch a register (goes through GPIO) |
| **MCAL** | registers and bit positions | know there is an LED on the pin |

---

## 2. One 1 ms tick

```mermaid
sequenceDiagram
    autonumber
    participant HW as SysTick hardware
    participant STK as STK (MCAL)
    participant SCH as SCHED (SERVICES)
    participant HAL as HAL runnable<br/>e.g. HSWITCH
    participant APP as APP runnable

    HW->>STK: counter reaches 0 → exception 15
    Note over STK: interrupt context
    STK->>STK: STK_u32TimeMs++
    STK->>SCH: callback SCHED_vTickCbf()
    SCH->>SCH: tickPending = 1
    SCH-->>HW: return from interrupt

    Note over SCH: main context · SCHED_enuStart loop
    SCH->>SCH: sees tickPending, clears it
    loop every row of sched_cfg.c
        SCH->>SCH: remaining−1 (if > 0)
        alt remaining == 0
            SCH->>HAL: call row's function
            HAL-->>SCH: one short step, return
            SCH->>APP: call row's function
            APP-->>SCH: one short step, return
            SCH->>SCH: remaining = period
        end
    end
```

Every runnable must **return quickly**. The scheduler is cooperative: a runnable that blocks freezes all the others.

---

## 3. Where a button press travels

```mermaid
flowchart LR
    BTN(["button pin<br/>bounces for ~5–20 ms"])
    GPIO["GPIO_enuPinRead<br/>(MCAL)"]
    RUN["HSWITCH runnable<br/>called by SCHED every 5 ms"]
    STATE[("accepted state<br/>static in hswitch.c")]
    API["HSWITCH_enuReadState<br/>(public API)"]
    APP["APP runnable<br/>decides what a press means"]

    BTN --> GPIO --> RUN --> STATE --> API --> APP

    classDef hw fill:#E4E8EC,stroke:#3A4652,color:#16212C
    classDef mcal fill:#E8F0FB,stroke:#1D5FB8,color:#16212C
    classDef hal fill:#FBF1E2,stroke:#B06A00,color:#16212C
    classDef app fill:#F1ECFA,stroke:#6A3FB5,color:#16212C
    class BTN hw
    class GPIO mcal
    class RUN,STATE,API hal
    class APP app
```

The app never reads the pin and never sees a bounce. It asks HSWITCH for the latest **accepted** state.

---

## 4. Config vs. logic

Each driver is compiled **once per app**. The app folder supplies the configuration through `-I src/APP/<app>`.

```mermaid
flowchart TB
    ENV["platformio.ini env<br/>build_src_filter + -I src/APP/&lt;app&gt;"]
    subgraph APPF["app folder · one per app"]
        CH["x_cfg.h<br/>IDs … X_COUNT"]
        CC["x_cfg.c<br/>const table in flash"]
    end
    subgraph DRV["driver folder · shared by every app"]
        H["x.h<br/>API + cfg struct TYPE"]
        C["x.c<br/>logic + static RAM state"]
        P["x_priv.h<br/>registers, MCAL only"]
    end

    ENV --> APPF
    CH -->|"found through -I"| H
    CC -->|"linked by extern"| C
    H --> C
    P --> C
```

| What | Where |
|---|---|
| register address, bit position | `x_priv.h` |
| the *shape* of a config row | `x.h` |
| the *values* (pins, periods) | `x_cfg.c` in the app |
| the list of IDs | `x_cfg.h` in the app |
| runtime state (counters, flags) | `static` in `x.c` (`static volatile` if an ISR shares it) |

---

## 5. HLCD: asynchronous character LCD

Driver for KS0066 / HD44780-compatible LCDs. Supports **several LCDs at once**, **8-bit or 4-bit** bus per LCD, text, cursor moves and **custom characters** (CGRAM). No public function ever waits: the app puts bytes in a queue and returns; one runnable drains it.

### Data path

```mermaid
flowchart LR
    APP["APP<br/>WriteString · SetCursor<br/>StoreCustomChar"]
    Q[("ring buffer · per LCD<br/>32 × byte + 32-bit RS bitmap")]
    RUN["HLCD_vRunnable<br/>every 2 ms"]
    PINS["GPIO<br/>RS · E · D7–D4 (· D3–D0)"]
    LCD(["LCD controller"])

    APP -- "put (byte, RS)" --> Q
    Q -- "get, one byte per 2–4 ticks" --> RUN
    RUN --> PINS --> LCD

    classDef app fill:#F1ECFA,stroke:#6A3FB5,color:#16212C
    classDef hal fill:#FBF1E2,stroke:#B06A00,color:#16212C
    classDef mcal fill:#E8F0FB,stroke:#1D5FB8,color:#16212C
    classDef hw fill:#E4E8EC,stroke:#3A4652,color:#16212C
    class APP app
    class Q,RUN hal
    class PINS mcal
    class LCD hw
```

Commands (cursor moves, CGRAM writes, the init sequence) travel through the **same queue** as characters, so everything reaches the LCD in the order it was requested. Bit *i* of the RS bitmap holds the RS level of slot *i* (0 = command, 1 = data).

### Two state machines per LCD

```mermaid
stateDiagram-v2
    direction LR
    state "INIT" as init {
        [*] --> NO_REQ
        NO_REQ --> PINS: HLCD_enuInit() (queues 4 init commands)
        PINS --> NIBBLE: 4-bit
        PINS --> DONE: 8-bit
        NIBBLE --> NIBBLE_LATCH: D7–D4 = 0010, E↑
        NIBBLE_LATCH --> DONE: E↓ (LCD now 4-bit)
    }
    state "DATA (runs inside DONE)" as data {
        SEND --> LATCH: get byte, E↑
        LATCH --> SEND: 8-bit, E↓
        LATCH --> SEND_LOW: 4-bit, E↓
        SEND_LOW --> LATCH_LOW: low nibble, E↑
        LATCH_LOW --> SEND: E↓
    }
```

The lone `0010` nibble switches a 4-bit LCD out of its power-on 8-bit mode; it cannot go through the queue because it has no second half. Everything after it (Function Set, Display ON, Clear, Entry Mode, then app text) is drained from the queue.

### Timing

One E pulse = 2 runnable ticks (E high for a full tick, then low).

| | per byte | 13-byte line (`UP: hh:mm:ss` + cursor) |
|---|---|---|
| 8-bit | 2 ticks × 2 ms = **4 ms** | 52 ms |
| 4-bit | 4 ticks × 2 ms = **8 ms** | 104 ms |

The longest datasheet wait (Clear, 1.53 ms) is shorter than one tick, so no state ever busy-waits. The 30 ms power-on wait is the scheduler's first delay (50 ms).

### Config per LCD (`hlcd_cfg.c`)

| Field | Values |
|---|---|
| `pins[]` | D0–D7, RS, E (D0–D3 unused in 4-bit) |
| `mode` | `HLCD_MODE_8BIT`, `HLCD_MODE_4BIT` |
| `lines`, `font` | 1 / 2 lines, 5×7 / 5×10 |
| `cursor`, `blink`, `entryDirection`, `displayShift` | on / off, increment / decrement |

Switching an app from 8-bit to 4-bit is a one-word change here; app code is untouched.

---

## 6. Repository layout

```
src/
├── MCAL/        RCC · GPIO · STK · NVIC        registers live only here
├── HAL/         HLED · HSWITCH · HLCD          devices, built on GPIO
├── SERVICES/    SCHED                          when things run
├── BSP/         board.h                        Black Pill / Nucleo pin names
└── APP/<app>/   main.c · app.c · *_cfg.*       behavior + configuration
test/            Unity tests on the PC (UNIT_TEST fake registers)
docs/            this file
```

## 7. Status

| Module | Layer | State |
|---|---|---|
| RCC, GPIO, STK (polling + interrupt), NVIC | MCAL | done |
| HLED | HAL | done |
| SCHED | SERVICES | done |
| HSWITCH (shift-register debounce, 4-state machine) | HAL | done |
| HLCD (async queue, multi-LCD, 8/4-bit, CGRAM) | HAL | done |
| HKEYPAD | HAL | next |
| UART | MCAL | planned |

| App (`src/APP/`) | Shows |
|---|---|
| `hswitch_test` | debounced button → LED |
| `hlcd_test` | katakana line, `UP: hh:mm:ss` uptime, CGRAM-animated stick figure |