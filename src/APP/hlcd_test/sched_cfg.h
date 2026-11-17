#ifndef SCHED_CFG_H
#define SCHED_CFG_H


typedef enum
{
    SCHED_RUN_UPTIME = 0, /* highest pri*/
    SCHED_RUN_HLCD,
    SCHED_RUN_DANCE,
    SCHED_RUNNABLE_COUNT
}SCHED_runnableId_t;


#endif /*SCHED_CFG_H*/