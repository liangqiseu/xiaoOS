/************************************************************************/
/* name:os_api.h                                                        */
/* create data:2017/8/15                                                */
/************************************************************************/
#ifndef OS_TASK_API_H
#define OS_TASK_API_H

#include "config.h"


#define OS_OK 0
#define OS_ERR 1


#define TRUE 1
#define FALSE 0

#define OS_TASK_MAX_NUM 32


#define OS_TASK_ACTIVE 0xff


typedef enum tagOS_TASK_ID
{
	OS_TASK_ID_IDLE=0x0,
	OS_TASK_ID_PRINT_A=0x3,
	OS_TASK_ID_PRINT_B=0x14    
}OS_TASK_ID_E;



typedef enum tagOS_TASK_RETURN_E
{
	OS_TASK_IDLE = 0x0,  
	OS_TASK_BUSY       
}OS_TASK_RETURN_E;


typedef enum tagOS_MSG_RETRY_E
{
	OS_MSG_NEED_NO_RETRY = 0x0, 
	OS_MSG_NEED_RETRY
}OS_MSG_RETRY_E;


typedef OS_TASK_RETURN_E(*OS_TASK_FUNCTION)(void);


typedef OS_MSG_RETRY_E (*OS_MSG_FUNCTION)(void* v_pArgs);


typedef struct tagOS_TASK_INFO
{
	u8 enableBitmap;  
	u8 regFlag;       
	u32 weight;      
	u32 runtimes;     
	u32 taskId;      
	char* name;      
	OS_TASK_FUNCTION taskFunction;  
}OS_TASK_INFO_S;


typedef struct tagOS_SCHEDULE_CTRL
{
	u32 schedulaBitap;      
	OS_TASK_INFO_S taskinfo[OS_TASK_MAX_NUM];    
}OS_SCHEDULE_CTRL_S;


typedef struct tagOS_MSG_RETRY_S
{
	OS_MSG_FUNCTION fun;      
	void* pArgs;               
}OS_MSG_RETRY_S;


void OS_Schedule(void); 
void OS_Init(void);    
u32 OS_CreateTask(OS_TASK_FUNCTION v_fun, char* v_name);  
OS_MSG_RETRY_E OS_MsgSend(OS_MSG_FUNCTION v_fun, void* v_pArgs);
#endif
