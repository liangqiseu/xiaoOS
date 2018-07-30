/************************************************************************/
/* name:main.c                                                          */
/* create data:2017/8/15                                                */
/************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "../inlcudes/config.h"
#include "../inlcudes/list_api.h"
#include "../inlcudes/os_task_api.h"
#include "../inlcudes/os_mem_api.h"
#include "../test/test_api.h"


void TEST_Start(void)
{
    u32 testRes = TEST_PASS;

    /** all testcase **/
    testRes |= LIST_Test();
    testRes |= Mem_Test();
    if (TEST_PASS == testRes)
    {
        printf("TEST PASS!\r\n");
    }
    else
    {
        printf("TEST FAIL!\r\n");
    }
    
    return;
}

int main(int argc,char *argv[])
{

    if (1 == argc)
    {
#ifndef WIN32
        OS_MemInit();
        printf("OS MEM INIT OK!\r\n");
#else
        TEST_Start();
#endif
    }
    else
    {
        if (0 == strcmp(argv[1], "test"))
        {
            TEST_Start();
        }
    }

    //OS_Schedule();
    return OS_OK;
}



