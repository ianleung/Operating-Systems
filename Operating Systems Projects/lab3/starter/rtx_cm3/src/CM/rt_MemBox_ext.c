/*----------------------------------------------------------------------------
 *      ECE254 Lab3
 *----------------------------------------------------------------------------
 *      Name:    RT_MEMBOX_ext.C
 *      Purpose: Interface functions for blocking 
 *               fixed memory block management system
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *      This code is extends the RealView Run-Time Library.
 *      Created by University of Waterloo ECE254 Lab Staff.
 *---------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------
 *      Includes
 *---------------------------------------------------------------------------*/

#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_System.h"
#include "rt_MemBox.h"
#include "rt_HAL_CM.h"
#include "rt_Task.h"       /* added in ECE254 lab3 keil_proc */ 
#include "rt_MemBox_ext.h" /* added in ECE254 lab3 keil_proc */  
#include "rt_List.h"
#include "rt_Task_ext.h"   /* added in ECE254 lab3 keil_proc */ 




/* ECE254 Lab3 Comment: You may need to include more header files */

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/
struct OS_XCB os_mem;

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/
/*---------------- rt_sys_init_s, initialize OS with wait_mem queue-----------*/

/**  
   @brief: initialize OS with wait_mem queue
 */
void rt_sys_init_s (FUNCP first_task, U32 prio_stksz, void *stk){
	rt_sys_init (first_task, prio_stksz, stk); 

	/* Set up memory wait list: initially empty */
	os_mem.cb_type = HCB;
	os_mem.p_lnk = NULL;
}


/*==========================================================================*/
/*  The following are added for ECE254 Lab3 Proc Management Assignmet       */
/*==========================================================================*/

/*---------------- rt_alloc_box_s, task blocks when out of memory-----------*/

/**  
   @brief: Blocking memory allocation routine.
 */
void *rt_alloc_box_s (void *p_mpool) {
	void *ptr;

	ptr = rt_alloc_box(p_mpool);
	if (ptr == NULL){
		rt_put_prio(&os_mem, os_tsk.run);
		rt_block(0xffff, WAIT_MEM);
	}
	return ptr;
}


/*----------- rt_free_box_s, pair with _s memory allocators ----------------*/
/**
 * @brief: free memory pointed by ptr, it will unblock a task that is waiting
 *         for memory.
 * @return: OS_R_OK on success and OS_R_NOK if ptr does not belong to gp_mpool 
 */
OS_RESULT rt_free_box_s (void *p_mpool, void *box) {
	/* Add your own code here. Change the following line accordingly */
	P_TCB p_TCB;
	if(os_mem.p_lnk != NULL){
		p_TCB = rt_get_first(&os_mem);
		p_TCB->ret_val = (U32) box;
		rt_dispatch(p_TCB);
		return (OS_R_OK);
	}
	return rt_free_box(p_mpool, box) ? OS_R_NOK : OS_R_OK;
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
