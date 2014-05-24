/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    MAILBOX.C
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/
#include <LPC17xx.h>
#include "uart_polling.h"
#include <RTL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


OS_MUT g_mut_uart;										/* UART printing mutex for proper output */
OS_MUT procon;												/* Mutex for sharing mem between pro and con*/
OS_TID g_tid = 255;

const U32 N = 20;											/* Total number of integers */
const U32 B = 2;											/* Max size of the message Q*/
const U32 P = 3;											/* Create P Producers */
const U32 C = 6;											/* Create C Consumers */

U32 cur_producer = 0;
U32 cur_consumer = 0;

U32 task_creator_ctr = 0;

/*
U32 sendTaskCtr = 0;
U32 recvTaskCtr = 0;
*/

U32 msg_counter_rec = 0;
U32 msg_counter_send = 0;

//int N_arr[] = { 20, 40, 80, 160, 320 };
//int B_arr[] = { 1, 2, 4, 8, 10 };


typedef struct {                      /* Message object structure            */
  U32 number;                      		/* Random number */
} T_NUM;

os_mbx_declare (MsgBox,320);           /* Declare an RTX mailbox              */
_declare_box (mpool,sizeof(T_NUM),320);/* Dynamic memory pool                */



__task void producer_task (void);
__task void consumer_task (void);
//__task void updater_task (void);
__task void init(void);

U32 time_a, time_b, time_c;



/*----------------------------------------------------------------------------
 *  Task Producer:  RTX Kernel starts this task with os_sys_init (producer_task)
 *---------------------------------------------------------------------------*/
__task void producer_task (void) {
  
	OS_TID producer_tskid;                          /* assigned identification for producer  */

	T_NUM *random_num_tx;
	
	//Get Time A
	//time_a = os_time_get();
	
	//Get current task ID
	producer_tskid = os_tsk_self ();
	
	// fork the child process
	os_tsk_create (consumer_task, 0); /* start task 2                        */
  
	os_mut_wait(procon, 0xFFFF);
	if(cur_producer == 0)
	{
		os_mbx_init (MsgBox, sizeof(MsgBox));/* initialize the mailbox             */
 
		os_mut_wait(g_mut_uart, 0xFFFF);
		printf("[producer_task [pid:(%d)] ]: Mailbox Created\n", producer_tskid);
		os_mut_release(g_mut_uart);
		
		cur_producer = 1;
	}
	os_mut_release(procon);
		
	//Get Time B
	//time_b = os_time_get() ;

	// Check if the number is the required one
	// .. And also make sure that we still have to send numbers.
	while (msg_counter_send < N && ( (msg_counter_send % P) == producer_tskid ) )
	{
		random_num_tx = _alloc_box(mpool);			/* Allocate a memory for the message   */
	
		if( random_num_tx == NULL )
		{
			os_mut_wait(g_mut_uart, 0xFFFF);
			printf("_alloc_box failed because of no mem available!\n");
			os_mut_release(g_mut_uart);
			exit(1);
		}
		
		random_num_tx->number = (U32) ( msg_counter_send );
		
		os_mbx_send (MsgBox, random_num_tx, 0xffff); /* Send the message to the mailbox     */
		
		os_mut_wait(g_mut_uart, 0xFFFF);
		printf("[producer_task [pid:(%d)] ]: Sent %u\n", producer_tskid, random_num_tx->number);
		msg_counter_send++;
		os_mut_release(g_mut_uart);
		
		
		// os_dly_wait (100);
	}
	
	//msg_counter_send = 0;
		
	os_tsk_delete_self ();              /* We are done here, delete this task  */
}

/*----------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (consumer_task, 0)
 *---------------------------------------------------------------------------*/
__task void consumer_task (void) {

	OS_TID consumer_tskid;                          /* assigned identification for consumer */
	
	consumer_tskid = os_tsk_self();									/* get it's own task ID */
	
	T_NUM *random_num_rx;

	while( msg_counter_rec < N )
	{
		os_mbx_wait (MsgBox, (void **)&random_num_rx, 0xffff); /* wait for the message    */
		
		os_mut_wait(g_mut_uart, 0xFFFF);
		printf("[consumer_task [pid:(%d)] ]: Received %u\n", consumer_tskid, random_num_rx->number);
		msg_counter_rec += 1;
		os_mut_release(g_mut_uart);
		
		if( _free_box (mpool, random_num_rx) )           /* free memory allocated for message  */
		{
			os_mut_wait(g_mut_uart, 0xFFFF);
			printf("_free_box failed because memory couldn't be freed\n");
			os_mut_release(g_mut_uart);
			exit(1);
			
		}
	}


	//Get Time C 
	time_c = os_time_get();
	
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("Time to initialize system: %0.6f\n", (float)(((float)time_b - time_a)/1000000) );
	printf("Time to transmit data: %0.6f\n", (float)(((float)time_c - time_b)/1000000) );
	os_mut_release(g_mut_uart);

	// os_dly_wait(10);
	os_tsk_delete_self ();              /* We are done here, delete this task  */
}


/*
__task void updater_task (void) {
	
	int i = 0;
	int j = 0;
	
	for(; i < 5; i++)
	{
		for(; j < 5; j++)
		{
			os_mut_wait(g_mut_uart, 0xFFFF);
			N = N_arr[i];
			B = B_arr[j];
			printf("N = %d\t B = %d\n", N, B);
			os_mut_release(g_mut_uart);
			os_dly_wait(100000);
			
		}
	}
}
*/
		
__task void init(void)
{
	os_mut_init(&g_mut_uart);
	os_mut_init(&procon);
	
	for( task_creator_ctr = 0; task_creator_ctr < P; task_creator_ctr++ )
	{
		os_tsk_create(producer_task, 0);
	}
	
	task_creator_ctr = 0;
	
	for( task_creator_ctr = 0; task_creator_ctr < C; task_creator_ctr++ )
	{
		os_tsk_create(consumer_task, 0);
	}
	
	// os_tsk_create(updater_task, 10);
	
	os_tsk_delete_self();
}
	
	
/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {                     /* program execution starts here       */

 	SystemInit();         /* initialize the LPC17xx MCU */
  uart0_init();         /* initilize the first UART */
  
	

  printf("Calling os_sys_init()...\n");

	
	_init_box (mpool, sizeof(mpool),    /* initialize the 'mpool' memory for   */
              sizeof(T_NUM));        /* the membox dynamic allocation       */
	os_sys_init(init);	  /* initilize the OS and start the first task */
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
