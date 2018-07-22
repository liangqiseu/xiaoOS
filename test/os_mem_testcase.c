#include <stdio.h>
#include <stdlib.h>

#include "../inlcudes/os_mem_api.h"
#include "../inlcudes/config.h"

extern PTR OS_MemAlignToX(PTR v_addr, u32 v_alignSize);
u32 TEST_MemAlignToX(void)
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

    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}


extern u32 OS_MemCalcBlkCnt(void);
extern MEM_MGT_S g_memMgt;
u32 TEST_MemCalcBlkCnt(void)
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

    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}



extern void OS_MemBlkListInit(void);
extern void OS_MemSplitToBlk(void);
extern u32 OS_MemIsAlignToX(void *v_addrOrLen, u32 v_alignSize);
extern PTR OS_MemAlignToPtr(PTR v_len);
extern void OS_MemCfgInit(void *v_pMemAddr, u32 v_memLen, u32 v_blkSize);
extern void OS_MemShowMgt(void);

u32 TEST_MemSplitToblk(u32 v_memLen, u32 v_blkSize)
{
    u32 blkIdx = 0;
    u32 freeBlkCnt = 0;
    void *pMemStartAddr = NULL;
    PTR pTmpAddr = 0;
    LIST_NODE_S *pListHead = &g_memMgt.freeBlkListHead;
    MEM_BLK_HEAD_S *pBlkHead = NULL;

    pMemStartAddr = malloc(v_memLen);
    if (NULL == pMemStartAddr)
    {
        printf("%s(line:%d): malloc FAIL! \r\n",__func__,__LINE__);
        free(pMemStartAddr);
        return TEST_FAIL;
    }
    
    OS_MemCfgInit(pMemStartAddr, v_memLen, v_blkSize);
    
    if (TRUE == OS_MemIsAlignToX(pMemStartAddr,g_memMgt.blkSize))
    {
        freeBlkCnt++;
    }
    freeBlkCnt = (v_memLen / g_memMgt.blkSize  >= 2) ? (v_memLen / g_memMgt.blkSize - 2 + freeBlkCnt) : 0;

    //Excute
    OS_MemBlkListInit();
    OS_MemSplitToBlk();

    //Judgment result
    if (freeBlkCnt != g_memMgt.freeBlkCnt)
    {
        printf("%s(line:%d): malloc FAIL! \r\n",__func__,__LINE__);
        free(pMemStartAddr);
        return TEST_FAIL;
    }

    pBlkHead = (MEM_BLK_HEAD_S *)(pListHead->next);
    for (blkIdx = 0; blkIdx < freeBlkCnt; blkIdx++)
    {
        /*
        printf("idx=0x%x: self=0x%p next=0x%p pre=0x%p poolIdx=0x%x blkAddr=0x%p \r\n", 
                blkIdx, pBlkHead,pBlkHead->blkNode.next, pBlkHead->blkNode.pre, pBlkHead->poolIdx, pBlkHead->pBlkAddr);
        */        
        pTmpAddr = (PTR)g_memMgt.pMemAlignStartAddr + (blkIdx + 1) * g_memMgt.blkSize;
        if ((PTR)pTmpAddr != (PTR)pBlkHead->pBlkAddr)
        {
            printf("%s(line:%d): pTmpAddr=0x%p pBlkHead->pBlkAddr=0x%p !\r\n",__func__,__LINE__,(void*)pTmpAddr, pBlkHead->pBlkAddr);
            return TEST_FAIL;
        }
        pBlkHead = (MEM_BLK_HEAD_S *)(pBlkHead->blkNode.next);
    }

    free(pMemStartAddr);
    
    return TEST_PASS;
}


u32 TEST_MemSplitToblkCaseSet(void)
{
    u32 res = TEST_PASS;
    res &= TEST_MemSplitToblk(0x6000, 0x1000);
    //res &= TEST_MemSplitToblk(0x1000, 0x1000);

    if (TEST_PASS == res)
    {
        printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    }
    return res;
}



extern void OS_MemSplitBlkToPagePool(MEM_PAGE_POOL_MGT_S *v_pPagePoolMgt);

u32 TEST_MemSplitBlkToPagePool(u16 v_pageSize, u16 v_defaultPageNum)
{
    u32 memLen = 0x3000;
    u32 blkSize = 0x1000;
    u32 pageIdx = 0;
    u32 freeBlkCnt = 0;
    u32 freePageCnt = 0;
    void *pMemStartAddr = NULL;
    PTR pPageAddr = 0;
    PTR pPageHeadAddr = 0;
    MEM_BLK_HEAD_S *pUsedFirstBlk = NULL;
    MEM_PAGE_HEAD_S *pPageHead =NULL;
    MEM_PAGE_POOL_MGT_S tempPool = { 0 };

    tempPool.defaultPageNum = v_defaultPageNum;
    tempPool.pageSize = v_pageSize;

    LIST_HEAD_INIT(&tempPool.freePageListHead);
    LIST_HEAD_INIT(&tempPool.usedBlkListHead);
    pMemStartAddr = malloc(memLen);
    if (NULL == pMemStartAddr)
    {
        printf("%s(line:%d): malloc FAIL! \r\n",__func__,__LINE__);
        free(pMemStartAddr);
        return TEST_FAIL;
    }
    
    OS_MemCfgInit(pMemStartAddr, memLen, blkSize);

    //Excute
    OS_MemBlkListInit();
    OS_MemSplitToBlk();
    OS_MemSplitBlkToPagePool(&tempPool);

    //Judgment result
    freePageCnt = blkSize / (tempPool.pageSize + (u16)OS_MemAlignToPtr(sizeof(MEM_PAGE_HEAD_S)));
    if (freePageCnt != tempPool.freePageCnt)
    {
        printf("%s(line:%d): malloc FAIL! \r\n",__func__,__LINE__);
        free(pMemStartAddr);
        return TEST_FAIL;
    }

    pUsedFirstBlk = (MEM_BLK_HEAD_S*)((&tempPool.usedBlkListHead)->next);
    pPageHead = (MEM_PAGE_HEAD_S*)((&tempPool.freePageListHead)->next);

    for (pageIdx = 0; pageIdx < freePageCnt; pageIdx++)
    {          
        pPageHeadAddr = (tempPool.pageSize + (u16)OS_MemAlignToPtr(sizeof(MEM_PAGE_HEAD_S))) *pageIdx + (PTR)pUsedFirstBlk->pBlkAddr;
        pPageAddr = pPageHeadAddr + (u16)OS_MemAlignToPtr(sizeof(MEM_PAGE_HEAD_S));
        //printf("idx=0x%x: pPageHead=0x%p pPageHeadAddr=0x%p pPageAddr=0x%p pPageHead->pPageAddr=0x%p\r\n",
        //    pageIdx, (void*)pPageHead, (void*)pPageHeadAddr, (void*)pPageAddr, pPageHead->pPageAddr);

        if (!(((PTR)pPageHead == pPageHeadAddr) && (pPageAddr == (PTR)pPageHead->pPageAddr)))
        {
            printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
            return TEST_FAIL;
        }

        pPageHead = (MEM_PAGE_HEAD_S *)(pPageHead->pageNode.next);
        
    }

    free(pMemStartAddr);
    return TEST_PASS;
}


u32 TEST_MemSplitBlkToPagePoolCaseSet(void)
{
    u32 res = TEST_PASS;
    res &= TEST_MemSplitBlkToPagePool(1024, 2);

    if (TEST_PASS == res)
    {
        printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    }

    return res;
}


extern void OS_MemInit(void);


u32 TEST_MemInit(void)
{
    
    return TEST_PASS;
}



void Mem_Test(void)
{
    (void)TEST_MemAlignToX();
    (void)TEST_MemCalcBlkCnt();
    (void)TEST_MemSplitToblkCaseSet();
    (void)TEST_MemSplitBlkToPagePoolCaseSet();
    return;
}

