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

extern void LIST_Test(void);
extern void Mem_Test(void);


int main(int argc,char *argv[])
{
    int i = 0;
    //int testRst = 0; 
    for(i=0;i<argc;i++)
    {
        printf("arcv[%d] is %s\n",i,argv[i]);
    }

    if (2 == argc)
    {
        /** all testcase **/
        LIST_Test();
        Mem_Test();
        printf("TEST OK!\r\n");
    }
    else
    {
        OS_MemInit();
        printf("OS MEM INIT OK!\r\n");
    }

    while(0){
    }
    //OS_Schedule();
    return OS_OK;
}



