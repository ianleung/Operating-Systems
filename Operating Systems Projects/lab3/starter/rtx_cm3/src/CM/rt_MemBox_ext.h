/*----------------------------------------------------------------------------
 *      ECE254 Lab3
 *----------------------------------------------------------------------------
 *      Name:    RT_MEMBOX_EXT.H
 *      Purpose: Interface functions for blocking 
 *               fixed memory block management system
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *      This code extends the RealView Run-Time Library.
 *      Created by University of Waterloo ECE254 Lab Staff.
 *---------------------------------------------------------------------------*/

/* Variables */
extern struct OS_XCB os_mem;

/* Functions */
extern void     *rt_alloc_box_s (void *mpool);
extern OS_RESULT rt_free_box_s  (void *mpool, void *ptr);

//custom init for wait_mem queue
extern void      rt_sys_init_s   (FUNCP first_task, U32 prio_stksz, void *stk);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

