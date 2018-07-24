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

extern void LIST_Test(void);
extern void Mem_Test(void);


int main(void)
{
    u32 taskAId = 0;
    u32 taskBId = 0;

    void *pMemAddr = NULL;
    u32 memLen = 0x7000;

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



