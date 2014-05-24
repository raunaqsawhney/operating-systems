/**
* @file: helloworld.c
* @brief: Two simple tasks running pseduo-parallelly
*/

#include <LPC17xx.h>
#include "uart_polling.h"
#include "../../../RTX_CM3/INC/RTL.h"
#include <stdio.h>

__task void task1()
{
	unsigned int i = 0;
	
	for (;; i++)
	{
		printf("Task1: %d\n", i);
		os_dly_wait(10);
	}
}

__task void task2()
{
	while(1)
	{
		printf("Task2: HelloWorld!\n");
		os_dly_wait(3);
	}
}

__task void task3()
{
	while(1)
	{
		printf("Task3: Printing Task 3!\n");
		os_dly_wait(3);
	}
}

__task void task4()
{
	while(1)
	{
		printf("Task4: Printing Task 4!\n");
		os_dly_wait(3);
	}
}


__task void getActiveTasks()
{
	// U32 tsk1;
	// tsk1 = os_tsk_self();

	OS_TID returnValue;
	returnValue = os_tsk_count_get();
	printf("Number of Active Tasks: %d\n", returnValue);
	//os_dly_wait(5);
	
}


__task void init(void)			// Task #1
{
	
	os_tsk_create(task1, 1);	//task1 at priority 1
	os_tsk_create(task2, 1); 	//task2 at priority 1
	os_tsk_create(task3, 1);	//task3 at priority 1
	os_tsk_create(task4, 1); 	//task4 at priority 1
	
	os_tsk_create(getActiveTasks, 1);
	
	os_tsk_delete_self();			//must delete itself before exiting

}

int main()
{
  SystemInit();							//Init system
  uart0_init();							//Start UART
	os_sys_init(init);				//Start OS Processes
	return 0;
}
