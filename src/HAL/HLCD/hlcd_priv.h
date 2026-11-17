#ifndef HLCD_PRIV_H
#define HLCD_PRIV_H

/* LMB161A datasheet: commands Table 6.1 (p.16), sections 6.1-6.8 */

/* Command bases (all option bits = 0) */
#define HLCD_CMD_CLEAR              0x01U
#define HLCD_CMD_RETURN_HOME        0x02U
#define HLCD_CMD_ENTRY_MODE         0x04U
#define HLCD_CMD_DISPLAY_CTRL       0x08U
#define HLCD_CMD_SHIFT              0x10U
#define HLCD_CMD_FUNCTION_SET       0x20U
#define HLCD_CMD_SET_CGRAM_ADDR     0x40U
#define HLCD_CMD_SET_DDRAM_ADDR     0x80U

/* Entry Mode Set */
#define HLCD_ID_POS                 1U
#define HLCD_SH_POS                 0U
/* Display ON/OFF Control */
#define HLCD_D_POS                  2U
#define HLCD_C_POS                  1U
#define HLCD_B_POS                  0U
/* Cursor or Display Shift */
#define HLCD_SC_POS                 3U
#define HLCD_RL_POS                 2U
/* Function Set */
#define HLCD_DL_POS                 4U
#define HLCD_N_POS                  3U
#define HLCD_F_POS                  2U

/* Interface */
#define HLCD_DL_8BIT                1U
#define HLCD_DL_4BIT                0U
#define HLCD_DATA_PINS              8U

/* DDRAM */
#define HLCD_LINE1_ADDR             0x00U
#define HLCD_LINE2_ADDR             0x40U
#define HLCD_COLUMNS                16U

/* CGRAM */
#define HLCD_CGRAM_CHARS            8U
#define HLCD_CGRAM_BYTES_PER_CHAR   8U
#define HLCD_CGRAM_INDEX_POS        3U
/* Timing: max command time 1.53 ms, so a 2 ms runnable covers every command */
#define HLCD_POWER_ON_DELAY_MS      30U

#define HLCD_QUEUE_SIZE             32U
#define HLCD_QUEUE_MASK             (HLCD_QUEUE_SIZE-1U)

#endif /*HLCD_PRIV_H*/