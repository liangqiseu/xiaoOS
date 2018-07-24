#include <stdio.h>
#include "../inlcudes/os_task_api.h"
#include "../inlcudes/config.h"


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


