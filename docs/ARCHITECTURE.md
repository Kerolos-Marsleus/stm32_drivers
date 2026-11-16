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
        HSW["HSWITCH<br/>(next)"]
        HLCD["HLCD<br/>(next)"]
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

## 5. Repository layout

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

## 6. Status

| Module | Layer | State |
|---|---|---|
| RCC, GPIO, STK (polling + interrupt), NVIC | MCAL | done |
| HLED | HAL | done |
| SCHED | SERVICES | done |
| HSWITCH | HAL | in progress |
| HLCD (8-bit, then 4-bit, async) | HAL | planned |
| UART | MCAL | planned |
