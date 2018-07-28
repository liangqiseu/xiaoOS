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

    /** all testcase **/
    LIST_Test();
    Mem_Test();


    OS_MemInit();
    
    printf("TEST OK!\r\n");
    while(0){
    }
    //OS_Schedule();
    return OS_OK;
}



