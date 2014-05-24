/**
 * @brief: ECE254 lab1 starter file that calls os_tsk_get()
 * @file: lab1_main.c
 * @author: Yiqing Huang
 * @date: 2013/05/28
 */
 
#include <LPC17xx.h>
#include "uart_polling.h"
#include "../../../RTX_CM3/INC/RTL.h" /* modified RTL.h file. NOT the default one! */
#include <stdio.h>
#include <string.h>
#include "../../../RTX_CM3/SRC/CM/rt_user.h"


//Declare a memory pool
//void *box_mem;
_declare_box(box_mem,4,32);


// First memeber in waiting list
void* first_wl_mem_ptr;

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

__task void init (void);
 
char *state2str(unsigned char state, char *str);
char *fp2name(void (*p)(), char *str);

OS_MUT g_mut_uart;
OS_TID g_tid = 255;

int  g_counter = 0;  // a global counter
char g_str[16];
char g_tsk_name[16];

struct func_info g_task_map[] = \
{
  /* os_idle_demon function ptr to be initialized in main */
  {NULL,  "os_idle_demon"}, \
  {task1, "task1"},   \
  {task2, "task2"},   \
	{task3, "task3"},		\
	{task4, "task4"},		\
	{task5, "task5"},		\
  {init,  "init" }
};

/*--------------------------- task1 -----------------------------------*/
/* simple counter task                         												 */
/*---------------------------------------------------------------------*/
__task void task1(void)
{
	for (;;)
	{
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
	
  
  os_mut_wait(g_mut_uart, 0xFFFF);
	printf("TID\tNAME\t\tPRIO\tSTATE   \t%%STACK\n");
	os_mut_release(g_mut_uart);
    
  for(i = 0; i <7; i++) { // this is a lazy way of doing loop.
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
	  printf("%d\t%s\t%d\t%s\t%d%%\n", \
           task_info.task_id, \
           fp2name(task_info.ptask, g_tsk_name), \
           task_info.prio, \
           state2str(task_info.state, g_str),  \
           task_info.stack_usage);
		os_mut_release(g_mut_uart);
  } 
  os_dly_wait(20);
	os_tsk_delete_self(); 
}

/*--------------------------- task3 -----------------------------------*/
/* A task can allocate a fixed size of memory.                     		 */
/*---------------------------------------------------------------------*/
__task void task3(void){

		void *pointer;
		pointer = os_mem_alloc(box_mem);
		
		if (pointer == NULL)
			printf("Not enough memory\n");
		else 
			printf("malloc'd(): %d \n", pointer);
		os_dly_wait(50);
		if(os_mem_free(box_mem, pointer) == OS_R_OK){
			printf("dealloc'd(): OS_R_OK \n");
		}else{
			printf("dealloc'd(): OS_R_NOK \n");
		}
		os_dly_wait(50);
		os_tsk_delete_self(); 
}


/*--------------------------- task4 -----------------------------------*/
/* A task periodically prints task status of each task in the system.  */
/*---------------------------------------------------------------------*/
__task void task4(void)
{
	for(;;){
	U8 i=1;
  RL_TASK_INFO task_info;
  
  os_mut_wait(g_mut_uart, 0xFFFF);
	printf("TID\tNAME\t\tPRIO\tSTATE   \t%%STACK\n");
	os_mut_release(g_mut_uart);
    
  for(i = 0; i <7; i++) { // this is a lazy way of doing loop.
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
	  printf("%d\t%s\t%d\t%s\t%d%%\n", \
           task_info.task_id, \
           fp2name(task_info.ptask, g_tsk_name), \
           task_info.prio, \
           state2str(task_info.state, g_str),  \
           task_info.stack_usage);
		os_mut_release(g_mut_uart);
  }
	 os_dly_wait(20);	
	}
	
}

/*--------------------------- task5 -----------------------------------*/
/* Simple delay task.  																								 */
/*---------------------------------------------------------------------*/

__task void task5(void)
{
	os_dly_wait(100);
	os_tsk_delete_self();
}


/*--------------------------- init ------------------------------------*/
/* initialize system resources and create other tasks                  */
/*---------------------------------------------------------------------*/
__task void init(void)
{
	//Initialize the memory pool
	_init_box(box_mem, sizeof(box_mem), 4);
	
  os_mut_init(&g_mut_uart);
  
  os_mut_wait(g_mut_uart, 0xFFFF);
  printf("init: TID = %d\n", os_tsk_self());
  os_mut_release(g_mut_uart);
  
	g_tid = os_tsk_create(task1, 1);  /* task 1 at priority 1 */
	os_mut_wait(g_mut_uart, 0xFFFF);
  printf("init: created task1 with TID %d\n", g_tid);
  os_mut_release(g_mut_uart);
  
  g_tid = os_tsk_create(task2, 4);  /* task 2 at priority 2 */
  os_mut_wait(g_mut_uart, 0xFFFF);
  printf("init: created task2 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
	
	g_tid = os_tsk_create(task3, 3);  /* task 3 at priority 3 */
  os_mut_wait(g_mut_uart, 0xFFFF);
  printf("init: created task3 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
	
	g_tid = os_tsk_create(task4, 4);  /* task 4 at priority 4 */
  os_mut_wait(g_mut_uart, 0xFFFF);
  printf("init: created task4 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
	
	g_tid = os_tsk_create(task5, 1);  /* task 5 at priority 1 */
  os_mut_wait(g_mut_uart, 0xFFFF);
  printf("init: created task5 with TID %d\n", g_tid);
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
  
  for ( i = 0; i < 8; i++) {
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
	os_sys_init(init);	  /* initilize the OS and start the first task */
}
