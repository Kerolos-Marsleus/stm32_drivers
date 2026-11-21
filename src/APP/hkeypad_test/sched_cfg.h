#ifndef SCHED_CFG_H
#define SCHED_CFG_H


typedef enum
{ 
    SCHED_RUN_HLCD = 0, /* highest pri*/
    SCHED_RUN_HKEYPAD,
    SCHED_RUN_DISPLAY_KEYPAD,
    SCHED_RUNNABLE_COUNT
}SCHED_runnableId_t;


#endif /*SCHED_CFG_H*/