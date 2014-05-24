#define INACTIVE			0
#define READY					1
#define	RUNNING				2
#define	WAIT_DLY			3
#define	WAIT_ITV			4
#define	WAIT_OR				5
#define WAIT_AND			6
#define WAIT_SEM			7
#define	WAIT_MBX			8
#define	WAIT_MUT			9
#define WAIT_MEM			10


typedef struct rl_task_info	{
	U8	state;
	U8 	prio;
	U8	task_id;
	U8	stack_usage;
	void (*ptask) ();
} RL_TASK_INFO;

#define __SVC_0				__svc_indirect(0)

//Custom Active Task Count API

 
extern void* rt_mem_alloc(void* box_mem);
#define os_mem_alloc(box_mem)						_os_mem_alloc((U32)rt_mem_alloc, box_mem)
extern void* _os_mem_alloc(U32 p, void* box_mem)											 __SVC_0;
 
extern OS_RESULT rt_mem_free(void *box_mem, void *box);
#define os_mem_free(box_mem, box)						_os_mem_free((U32)rt_mem_free, box_mem, box)
extern OS_RESULT _os_mem_free(U32 p, void	*box_mem, void	*box)				 __SVC_0;

extern OS_TID 	rt_tsk_count_get	(void);
#define os_tsk_count_get()						_os_tsk_count_get((U32)rt_tsk_count_get)
extern OS_TID 	_os_tsk_count_get		(U32 p)													 	 __SVC_0;

extern OS_RESULT 	rt_tsk_get	(OS_TID task_id, RL_TASK_INFO *buffer);
#define os_tsk_get(task_id, buffer)						_os_tsk_get((U32) rt_tsk_get, task_id, buffer)
extern OS_RESULT 	_os_tsk_get		(U32 p, OS_TID task_id, RL_TASK_INFO *buffer)								__SVC_0;
