#include <stdio.h>
#include <stdlib.h>

#include "../inlcudes/os_mem_api.h"
#include "../inlcudes/config.h"

extern PTR OS_MemAlignToX(PTR v_addr, u32 v_alignSize);
int TEST_MemAlignToX(void)
{
    PTR addrOne = 0x1;
    
    addrOne = OS_MemAlignToX(addrOne,4096);
    if (0x1000 != addrOne)
    {
        printf("%s(line:%d) FAIL! addrOne=%p! \r\n",__func__,__LINE__,(void*)addrOne);
        return TEST_FAIL;
    }

    addrOne = 0x2;
    addrOne = OS_MemAlignToX(addrOne,4096);
    if (0x1000 != addrOne)
    {
        printf("%s(line:%d) FAIL! addrOne=%p! \r\n",__func__,__LINE__, (void*)addrOne);
        return TEST_FAIL;
    }

    addrOne = 0x12000;
    addrOne = OS_MemAlignToX(addrOne,4096);
    if (0x12000 != addrOne)
    {
        printf("%s(line:%d) FAIL! addrOne=%p! \r\n",__func__,__LINE__, (void*)addrOne);
        return TEST_FAIL;
    }
    
    return TEST_PASS;
}


extern u32 OS_MemCalcBlkCnt(void);
extern MEM_MGT_S g_memMgt;
int TEST_MemCalcBlkCnt(void)
{
    u32 totalBlkCnt = 0;
    g_memMgt.pMemStartAddr = (void*)0x1;
    g_memMgt.memLen = 0x100;
    g_memMgt.blkSize = MEM_BLK_SZIE;

    g_memMgt.pMemAlignStartAddr = (void*)OS_MemAlignToX((PTR)g_memMgt.pMemStartAddr, g_memMgt.blkSize);
    totalBlkCnt = OS_MemCalcBlkCnt();
    if (0x0 != totalBlkCnt)
    {
        printf("%s(line:%d) FAIL! totalBlkCnt=%x! \r\n",__func__,__LINE__, totalBlkCnt);
        return TEST_FAIL;
    }

    g_memMgt.memLen = 0x1000;
    g_memMgt.pMemAlignStartAddr = (void*)OS_MemAlignToX((PTR)g_memMgt.pMemStartAddr, g_memMgt.blkSize);
    totalBlkCnt = OS_MemCalcBlkCnt();
    if (0x0 != totalBlkCnt)
    {
        printf("%s(line:%d) FAIL! totalBlkCnt=%x! \r\n",__func__,__LINE__, totalBlkCnt);
        return TEST_FAIL;
    }

    g_memMgt.memLen = 0x2000;
    g_memMgt.pMemAlignStartAddr = (void*)OS_MemAlignToX((PTR)g_memMgt.pMemStartAddr, g_memMgt.blkSize);
    totalBlkCnt = OS_MemCalcBlkCnt();
    if (0x1 != totalBlkCnt)
    {
        printf("%s(line:%d) FAIL! totalBlkCnt=%x! \r\n",__func__,__LINE__, totalBlkCnt);
        return TEST_FAIL;
    }

    g_memMgt.pMemStartAddr = (void*)0x1000;
    g_memMgt.memLen = 0x13000;
    g_memMgt.pMemAlignStartAddr = (void*)OS_MemAlignToX((PTR)g_memMgt.pMemStartAddr, g_memMgt.blkSize);
    totalBlkCnt = OS_MemCalcBlkCnt();
    if (0x13 != totalBlkCnt)
    {
        printf("%s(line:%d) FAIL! totalBlkCnt=%x! \r\n",__func__,__LINE__, totalBlkCnt);
        return TEST_FAIL;
    }

    
    return TEST_PASS;
}


extern void OS_MemInit(void);
extern void OS_MemCfgInit(void *v_pMemAddr, u32 v_len);


int TEST_MemInit(void)
{
    u32 mallocLen = 0x6000;
    void* pMemStartAddr = NULL;
    pMemStartAddr = malloc(mallocLen);
    if (NULL == pMemStartAddr)
    {
        printf("%s(line:%d): malloc FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }
    printf("%s(line:%d) malloc pMemStartAddr=0x%p! \r\n",__func__,__LINE__, pMemStartAddr);

    OS_MemCfgInit(pMemStartAddr,mallocLen);
    OS_MemInit();

    free(pMemStartAddr);
    return TEST_PASS;
}





void Mem_Test(void)
{
    //(void)TEST_MemAlignToX();
    //(void)TEST_MemCalcBlkCnt();
    (void)TEST_MemInit();
    return;
}

