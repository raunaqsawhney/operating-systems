/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    MAILBOX.C
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <RTL.h>                      /* RTX kernel functions & defines      */
#include <LPC21xx.H>                  /* LPC21xx definitions                 */
#include <stdio.h>
#include <stdlib.h>

OS_TID tsk1;                          /* assigned identification for task 1  */
OS_TID tsk2;                          /* assigned identification for task 2  */

int N = 10;
int B = 5;

os_mbx_declare (MsgBox,16);           /* Declare an RTX mailbox              */
_declare_box (mpool,sizeof(int),16);/* Dynamic memory pool                */

__task void send_task (void);
__task void rec_task (void);

U32 time_a, time_b, time_c;

/*----------------------------------------------------------------------------
 *        Initialize serial interface
 *---------------------------------------------------------------------------*/
void init_serial () {
  PINSEL0 = 0x00050000;               /* Enable RxD1 and TxD1                */
  U1LCR = 0x83;                       /* 8 bits, no Parity, 1 Stop bit       */
  U1DLL = 97;                         /* 9600 Baud Rate @ 15MHz VPB Clock    */
  U1LCR = 0x03;                       /* DLAB = 0                            */
}


/*----------------------------------------------------------------------------
 *  Task 1:  RTX Kernel starts this task with os_sys_init (send_task)
 *---------------------------------------------------------------------------*/
__task void send_task (void) {
  int* random_num_tx = NULL;
	int msg_counter = 0;
	
	
  tsk1 = os_tsk_self ();              /* get own task identification number  */
  
	//Get Time A
	time_a = os_time_get();
	
	tsk2 = os_tsk_create (rec_task, 0); /* start task 2                        */
  os_mbx_init (MsgBox, sizeof(MsgBox));/* initialize the mailbox             */
  os_dly_wait (5);                    /* Startup delay for MCB21xx           */
	
	//Get Time B
	time_b = os_time_get();
	
	printf("Time to initialize system: %d\n", (time_b - time_a) );
	
	while (msg_counter < N)
	{
		random_num_tx = _alloc_box(mpool);			/* Allocate a memory for the message   */
		*random_num_tx = rand() % 9;
		
		os_mbx_send (MsgBox, (void*) random_num_tx, 0xffff); /* Send the message to the mailbox     */
		printf("[send_task]: Sent %d\n", random_num_tx);
		IOSET1 = 0x10000;
		msg_counter++;
		os_dly_wait (100);
	}
	//Get Time C 
	time_c = os_time_get();
	
	printf("Time to transmit data: %d\n", (time_c - time_b) );
	
  os_tsk_delete_self ();              /* We are done here, delete this task  */
}

/*----------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (rec_task, 0)
 *---------------------------------------------------------------------------*/
__task void rec_task (void) {
  int* random_num_rx = NULL;

	int msg_counter = 0;
  if(B < N)
	{
		while( msg_counter < N )
		{
			os_mbx_wait (MsgBox, (void **)&random_num_rx, 0xffff); /* wait for the message    */
			printf("[rec_task]: Received %d\n", random_num_rx);
			_free_box (mpool, random_num_rx);           /* free memory allocated for message  */
			msg_counter++;
		}
	}
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {                     /* program execution starts here       */

  IODIR1 = 0xFF0000;                  /* P1.16..22 defined as Outputs        */
  init_serial ();                     /* initialize the serial interface     */
  _init_box (mpool, sizeof(mpool),    /* initialize the 'mpool' memory for   */
              sizeof(U32));        /* the membox dynamic allocation       */
  os_sys_init (send_task);            /* initialize and start task 1         */
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
