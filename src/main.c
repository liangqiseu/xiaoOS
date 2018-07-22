/************************************************************************/
/* name:main.c                                                          */
/* create data:2017/8/15                                                */
/************************************************************************/

#include<stdio.h>
#include<stdlib.h>

#include "../inlcudes/config.h"
#include "../inlcudes/list_api.h"
#include "../inlcudes/os_task_api.h"
#include "../inlcudes/os_mem_api.h"





OS_TASK_RETURN_E OS_TaskPrintA(void)
{
    //printf("AAAAAAAAAAAAAAA\r\n");
    
    return OS_TASK_BUSY;
}

OS_TASK_RETURN_E OS_TaskPrintB(void)
{
    //printf("BBBBBBBBBBBBBBBB\r\n");
    return OS_TASK_BUSY;
}

u32 g_test = 1;
OS_MSG_RETRY_E test_list(void* v_pArgs)
{
    g_test++;
    if (g_test%1000!=0)
    {
        return OS_MSG_NEED_RETRY;
    }

    printf("LIST TEST OK!\r\n");
    return OS_MSG_NEED_NO_RETRY;
}


extern void LIST_Test(void);
extern void Mem_Test(void);


int main(void)
{
    u32 taskAId = 0;
    u32 taskBId = 0;

    void *pMemAddr = NULL;
    u32 memLen = 0x7000;
    //OS_Init();


    //taskAId = OS_CreateTask(OS_TaskPrintA, "OS_TaskPrintA");
    //taskBId = OS_CreateTask(OS_TaskPrintB, "OS_TaskPrintB");

    //OS_MsgSend(test_list,NULL);
    LIST_Test();
    Mem_Test();

    pMemAddr = malloc(memLen);
    if (NULL == pMemAddr)
    {
        return OS_ERR;
    }
    OS_MemCfgInit(pMemAddr, memLen, 0x1000);
    OS_MemInit();
    
    printf("TEST OK!\r\n");
    while(1){
    }
    //OS_Schedule();
    return OS_OK;
}



