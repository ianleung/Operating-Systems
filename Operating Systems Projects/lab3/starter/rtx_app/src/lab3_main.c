/**
 * @brief: ECE254 lab3 starter file that calls os_tsk_get()
 * @file: lab3_main.c
 * @date: 2014/05/28
 */
/* NOTE: This release is for students to use */

#include <LPC17xx.h>
#include "uart_polling.h" 
#include <RTL.h>
#include "../../RTX_CM3/INC/RTL_ext.h" /* extended interface header file */
#include <stdio.h>
#include <string.h>

#define NUM_FNAMES 9

struct func_info {
  void (*p)();      /* function pointer */
  char name[16];    /* name of the function */
};

extern void os_idle_demon(void);
__task void task1(void);
__task void task2(void);
__task void task3(void);
__task void task4(void);
__task void task5(void);


__task void init0 (void);
__task void init1 (void);
__task void init2 (void);
 
char *state2str(unsigned char state, char *str);
char *fp2name(void (*p)(), char *str);

OS_MUT g_mut_uart;
OS_TID g_tid = 255;

int  g_counter = 0;  // a global counter
char g_str[16];
char g_tsk_name[16];
void *g_ptr;


_declare_box(mympool, 32, 20);


struct func_info g_task_map[NUM_FNAMES] = \
{
  /* os_idle_demon function ptr to be initialized in main */
  {NULL,  "os_idle_demon"}, \
	{task1, "task_ghost"},   \
	{task2, "task_print"},   \
	{task3, "task_allocate_0"},   \
	{task4, "task_allocate_1"},   \
	{task5, "task_free"},   \
	{init0, "init0"},	\
	{init1, "init1"},  	\
	{init2, "init2"},
};

/* no local variables defined, use one global var */
__task void task1(void)
{
	for (;;) {
		g_counter++;
	}
}

/*--------------------------- task2 -----------------------------------*/
/* checking states of all tasks in the system                          */
/*---------------------------------------------------------------------*/
__task void task2(void)
{
	U8 i=1;
	RL_TASK_INFO task_info;
	
  	for(;;){
		os_mut_wait(g_mut_uart, 0xFFFF);
		printf("TID\tNAME\t\tPRIO\tSTATE   \t%%STACK\n");
		os_mut_release(g_mut_uart);
	    
		for(i = 0; i <6; i++) { // this is a lazy way of doing loop.
			if (os_tsk_get(i+1, &task_info) == OS_R_OK) {
				os_mut_wait(g_mut_uart, 0xFFFF);  
				printf("%d\t%s\t\t%d\t%s\t%d%%\n", \
				       task_info.task_id, \
				       fp2name(task_info.ptask, g_tsk_name), \
				       task_info.prio, \
				       state2str(task_info.state, g_str),  \
				       task_info.stack_usage);
				os_mut_release(g_mut_uart);
			} 
		}
	    
		if (os_tsk_get(0xFF, &task_info) == OS_R_OK) {
			os_mut_wait(g_mut_uart, 0xFFFF);  
			printf("%d\t%s\t\t%d\t%s\t%d%%\n", \
			       task_info.task_id, \
			       fp2name(task_info.ptask, g_tsk_name), \
			       task_info.prio, \
			       state2str(task_info.state, g_str),  \
			       task_info.stack_usage);
			os_mut_release(g_mut_uart);
		} 
  		os_dly_wait(2);
	}
    
}

/*--------------------------- task3 -----------------------------------*/
/* Try to allocate a fixed size of memory.                             */
/*---------------------------------------------------------------------*/
__task void task3(void)
{
	
	for (;;) {
		g_ptr = os_mem_alloc(mympool);
		os_dly_wait(4);	
	}
}

/*--------------------------- task4 -----------------------------------*/
/* Try to allocate a fixed size of memory.                             */
/*---------------------------------------------------------------------*/
__task void task4(void)
{
	// increase stack usage
	int array[20];
	int i = 0 ; 
	for ( ; i < 10; i++) { array[i] = i; }

	for (;;) {
		g_ptr = os_mem_alloc(mympool);
		os_dly_wait(4);	
	}
}

/*--------------------------- task5 -----------------------------------*/
/* Frees the memory and releases the processes                         */
/*---------------------------------------------------------------------*/
__task void task5(void)
{
	for (;;) {
		os_mem_free(mympool, g_ptr);
		os_dly_wait(8);
	}
}


/*--------------------------- init0 ------------------------------------*/
/* Continuously allocate memory until not enough memory is available    */
/*----------------------------------------------------------------------*/
__task void init0(void)
{
	os_mut_init(&g_mut_uart);
  
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init0: TID = %d\n", os_tsk_self());
	os_mut_release(g_mut_uart);


	if (_init_box(mympool, sizeof(mympool), 32) == 0)
		printf("Box memory allocation OK.\n");
	else
		printf("Box memory allocation failed.\n");
      
  
	g_tid = os_tsk_create(task2, 1);  /* task 2 at priority 1 */
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init0: created task2 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);

	
	//Task will continuously allocate memory until not enough memory is available
	g_tid = os_tsk_create(task3, 1);  /* task 3 at priority 1 */
  	os_mut_wait(g_mut_uart, 0xFFFF);
  	printf("init: created task3 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);

	//Task will continuously allocate memory until not enough memory is available
	g_tid = os_tsk_create(task4, 1);  /* task 4 at priority 1 */
  	os_mut_wait(g_mut_uart, 0xFFFF);
  	printf("init: created task3 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
 
  
	os_tsk_delete_self();     /* task MUST delete itself before exiting */
}

/*--------------------------- init1 ------------------------------------*/
/* Test for blocking, free memory according to wait time               	*/
/*----------------------------------------------------------------------*/
__task void init1(void)
{
	os_mut_init(&g_mut_uart);
  
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init1: TID = %d\n", os_tsk_self());
	os_mut_release(g_mut_uart);


	if (_init_box(mympool, sizeof(mympool), 32) == 0)
		printf("Box memory allocation OK.\n");
	else
		printf("Box memory allocation failed.\n");
      
  
	g_tid = os_tsk_create(task2, 1);  /* task 2 at priority 1 */
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init1: created task2 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);

	
	//Task3 periodically allocates memory
	g_tid = os_tsk_create(task3, 1); 
  	os_mut_wait(g_mut_uart, 0xFFFF);
  	printf("init1: created task3 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
 
	//Task4 periodically allocates memory
	g_tid = os_tsk_create(task4, 1); 
 	 os_mut_wait(g_mut_uart, 0xFFFF);
 	 printf("init1: created task4 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
	
	//Task5 periodically frees memory
	g_tid = os_tsk_create(task5, 1); 
 	os_mut_wait(g_mut_uart, 0xFFFF);
  	printf("init1: created task5 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
  
	os_tsk_delete_self();     /* task MUST delete itself before exiting */
}

/*--------------------------- init2 ------------------------------------*/
/* Test for blocking, free memory according to priority 								*/
/*----------------------------------------------------------------------*/
__task void init2(void)
{
	os_mut_init(&g_mut_uart);
  
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init2: TID = %d\n", os_tsk_self());
	os_mut_release(g_mut_uart);


	if (_init_box(mympool, sizeof(mympool), 32) == 0)
		printf("Box memory allocation OK.\n");
	else
		printf("Box memory allocation failed.\n");
      
  
	g_tid = os_tsk_create(task2, 1);  /* task 2 at priority 1 */
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init2: created task2 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);

	
	//Task3 periodically allocates memory
	g_tid = os_tsk_create(task3, 1);  /* task 3 at priority 1 */
  	os_mut_wait(g_mut_uart, 0xFFFF);
  	printf("init2: created task3 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
 
	//Task3 periodically allocates memory
	g_tid = os_tsk_create(task4, 2);  /* task 4 at priority 2 */
 	 os_mut_wait(g_mut_uart, 0xFFFF);
 	 printf("init2: created task4 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
	
	//Task5 periodically frees memory
	g_tid = os_tsk_create(task5, 1);  /* task 5 at priority 1 */
 	os_mut_wait(g_mut_uart, 0xFFFF);
  	printf("init2: created task5 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
  
	os_tsk_delete_self();     /* task MUST delete itself before exiting */
}



/**
 * @brief: convert state numerical value to string represenation      
 * @param: state numerical value (macro) of the task state
 * @param: str   buffer to save the string representation, 
 *               buffer to be allocated by the caller
 * @return:the string starting address
 */
char *state2str(unsigned char state, char *str)
{
	switch (state) {
	case INACTIVE:
		strcpy(str, "INACTIVE");
		break;
	case READY:
		strcpy(str, "READY   ");
		break;
	case RUNNING:
		strcpy(str, "RUNNING ");
		break;
	case WAIT_DLY:
		strcpy(str, "WAIT_DLY");
		break;
	case WAIT_ITV:
		strcpy(str, "WAIT_ITV");
		break;
	case WAIT_OR:
		strcpy(str, "WAIT_OR");
		break;
	case WAIT_AND:
		strcpy(str, "WAIT_AND");
		break;
	case WAIT_SEM:
		strcpy(str, "WAIT_SEM");
		break;
	case WAIT_MBX:
		strcpy(str, "WAIT_MBX");
		break;
	case WAIT_MUT:
		strcpy(str, "WAIT_MUT");
		break;
	case WAIT_MEM:
		strcpy(str, "WAIT_MEM");
		break;
	default:
		strcpy(str, "UNKNOWN");    
	}
	return str;
}

/** 
 * @brief: get function name by function pointer
 * @param: p the entry point of a function (i.e. function pointer)
 * @param: str the buffer to return the function name
 * @return: the function name string starting address
 */
char *fp2name(void (*p)(), char *str)
{
	int i;
	unsigned char is_found = 0;
  
	for ( i = 0; i < NUM_FNAMES; i++) {
		if (g_task_map[i].p == p) {
			str = strcpy(str, g_task_map[i].name);  
			is_found = 1;
			break;
		}
	}
	if (is_found == 0) {
		strcpy(str, "ghost");
	}
	return str;
}

int main(void)
{
	SystemInit();         /* initialize the LPC17xx MCU */
	uart0_init();         /* initilize the first UART */
  
	/* fill the fname map with os_idle_demon entry point */
	g_task_map[0].p = os_idle_demon;
  
	printf("Calling os_sys_init()...\n");
	//os_sys_init_s(init0);    /* initilize the OS and start the first task */
	//os_sys_init_s(init1);    /* test for blocking and freeing based on wait time*/
	os_sys_init_s(init2);    /* test for blocking and freeing based on priority*/
}
