#include "app.h"
#include "sched.h"


int main(void){
    APP_vInit();
    if(SCHED_enuInit() != SCHED_OK){APP_vErrorHandler();}
    if(SCHED_enuStart() != SCHED_OK){APP_vErrorHandler();}
    APP_vErrorHandler(); 
}