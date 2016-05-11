/*----------------------------------------------------------------------------
 *      ECE254 Lab3
 *----------------------------------------------------------------------------
 *      Name:    RT_TASK_ext.C
 *      Purpose: Interface functions for extended task management  
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *      This code extends the RealView Run-Time Library.
 *      Created by University of Waterloo ECE254 Lab Staff.
 *---------------------------------------------------------------------------*/

#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_System.h"
#include "rt_Task.h"
#include "rt_List.h"
#include "rt_MemBox.h"
#include "rt_Robin.h"
#include "rt_HAL_CM.h"
#include "rt_Task_ext.h"

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *      Local Functions
 *---------------------------------------------------------------------------*/

/*--------------------------- rt_tsk_count_get ------------------------------*/
/* added in ECE254 lab2 keil_rtx */
int rt_tsk_count_get (void) {
	int count = 0;
	int i ;
	P_TCB p_tcb;  
	
	for ( i = 0 ; i < os_maxtaskrun ; i++){
    p_tcb = os_active_TCB[i];
		if (p_tcb != NULL && p_tcb->state != INACTIVE) count++;
	}
	if (os_idle_TCB.state != INACTIVE) count++;
	
	return count;
}
/* end of file */

/*--------------------------- rt_tsk_get ------------------------------------*/
/* added in ECE254 lab3 keil_proc */
OS_RESULT rt_tsk_get (OS_TID task_id, RL_TASK_INFO *p_task_info) {
	/* Add your own code here. Change the following lines accordingly */
	
	P_TCB p_tcb;
	double task_stack;
	
	if (task_id == 0 ||task_id == os_tsk.run->task_id){
		p_tcb = os_tsk.run;
		task_stack = rt_get_PSP();
	}
	else if (task_id <= os_maxtaskrun && os_active_TCB[task_id-1] != NULL){
		p_tcb = os_active_TCB[task_id - 1];		
		task_stack = p_tcb->tsk_stack;
	}
	else{
		return (OS_R_NOK);
	}
	
	p_task_info->task_id     = task_id;
	p_task_info->state       = p_tcb->state;
	p_task_info->prio        = p_tcb->prio;
	p_task_info->stack_usage = 100.0 - (((task_stack - (U32)p_tcb->stack)/ (U16)os_stackinfo) * 100);
	p_task_info->ptask       = p_tcb->ptask;
	return (OS_R_OK);	
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
