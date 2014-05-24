#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_System.h"
#include "rt_Task.h"
#include "rt_List.h"
#include "rt_MemBox.h"
#include "rt_Robin.h"
#include "rt_HAL_CM.h"
#include "rt_user.h"

/*--------------------------- Global Scheduler List -----------------------------------*/


struct OS_XCB wait_list = {HCB, NULL, NULL, NULL, NULL, 0};
P_TCB first;

/*--------------------------- *rt_mem_alloc -----------------------------------*/

void *rt_mem_alloc (void *box_mem)
{
	void *ptr;
	//printf("Entered malloc\n");
	ptr = rt_alloc_box(box_mem);
	if(ptr != NULL)	
		return ptr;
	else	{
		// add task to waiting list.
		rt_put_prio(&wait_list, os_tsk.run);
		rt_block (0xffff, (U8)os_tsk.run);
		return NULL;
	}
	
	/*
	void *ptr;
	ptr = rt_alloc_box(box_mem);
	
	printf("wait_count: %d\n", wait_count);
	
	if (ptr != NULL)
	{
		if (wait_count > 0)
		{
			if (os_tsk.run->prio <= highest_prio)
			{
				highest_prio = os_tsk.run->prio;
				printf("highest_prio: %d\n", highest_prio);  
				return ptr;
			}
		}
		else if (wait_count == 0)	//Empty list, with mem available, just add task
		{
			return ptr;
		}
	}
	else
	{
		rt_put_prio(&wait_list, os_tsk.run);
		//increment counter when a task is added to wait list
		wait_count++;
		printf("wait_count, if mem no available: %d\n", wait_count);
		rt_block (0xffff, (U8)os_tsk.run);
	}	
*/
}

/*--------------------------- os_mem_free -----------------------------------*/
OS_RESULT rt_mem_free (void *box_mem, void *box)
{
	
	first = rt_get_first(&wait_list);
	if( first != NULL )
	{
		first->ret_val = (U32)(box);
		rt_dispatch(first);
		return OS_R_OK;
	}
	else
	{
		return (OS_RESULT)rt_free_box(box_mem, box);
	}
}

/*--------------------------- rt_tsk_count_get -----------------------------------*/

OS_TID rt_tsk_count_get(void) 
{
	U32 tid;
	P_TCB p_task;
	int count = 0;

	for (tid = 0; tid < os_maxtaskrun; tid++)	
	{
		if (os_active_TCB[tid] != NULL)
		{
			p_task = os_active_TCB[tid-1];
			if(p_task->state != INACTIVE)
				count++;
		}
	}
	return count;

}


OS_RESULT rt_tsk_get(OS_TID	task_id, RL_TASK_INFO *buffer)
{
	U16	stack_size;
	U32 start_stack_ptr;
	U32 current_stack_ptr;
	U32 end_stack_ptr;
	
	P_TCB p_task;
	
	p_task = os_active_TCB[task_id-1];
	
	if(p_task->state == 0 || p_task->prio == 0 || p_task->tsk_stack == 0 || p_task->task_id == 0)
		return OS_R_NOK;
	
	buffer->state = p_task->state;
	buffer->prio = p_task->prio;
	buffer->task_id = p_task->task_id;
	
	end_stack_ptr = (U32) p_task->stack;
	
	stack_size =  (U16)os_stackinfo;
	if(p_task->state == 2)
	{
		current_stack_ptr = rt_get_PSP();
	}
	else
	{
		current_stack_ptr = p_task->tsk_stack;
	}
	
	start_stack_ptr = end_stack_ptr + stack_size;

	
	buffer->stack_usage = ((( start_stack_ptr - current_stack_ptr ) * 100) / ( stack_size));
	buffer->ptask = p_task->ptask;
	
	/*
	printf("(start_stack_ptr - current_stack_ptr) : %d\n",( start_stack_ptr - current_stack_ptr ));
	printf("(( start_stack_ptr - current_stack_ptr ) * 100) : %d\n",(( start_stack_ptr - current_stack_ptr ) * 100));
	printf("((( start_stack_ptr - current_stack_ptr ) * 100) / ( stack_size)) : %d\n",((( start_stack_ptr - current_stack_ptr ) * 100) / ( stack_size)));
	printf("100 - ((( start_stack_ptr - current_stack_ptr ) * 100) / ( stack_size)) : %d\n",100-((( start_stack_ptr - current_stack_ptr ) * 100) / ( stack_size)));

	
	printf("start_stack_ptr = %d\n", start_stack_ptr);
	printf("stack_size = %d\n", stack_size );
	printf("current_stack_ptr = %d\n", current_stack_ptr);
	printf("end_stack_ptr = %d\n", end_stack_ptr);
*/
	
	// printf("( rt_get_PSP() - (p_task->tsk_stack) ) : %d\n", ( rt_get_PSP() - (p_task->tsk_stack) ) ); 
	// printf("(( ( rt_get_PSP() - (p_task->tsk_stack) )  * 100 ) : %d\n", ( ( rt_get_PSP() - (p_task->tsk_stack) )  * 100 ) );
	// printf("( ( ( rt_get_PSP() - (p_task->tsk_stack) )  * 100 ) / (stack_size) ): %d\n", ( ( ( rt_get_PSP() - (p_task->tsk_stack) )  * 100 ) / (stack_size) ) );
	
	//printf("State = %d\n Priority = %d\n task_id = %d\n stack_usage = %d\n", buffer->state, buffer->prio, buffer->task_id, buffer->stack_usage);
	return OS_R_OK;
}


