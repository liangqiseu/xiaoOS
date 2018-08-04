#include <stdio.h>
#include <stdlib.h>

#include "../inlcudes/os_mem_api.h"
#include "../inlcudes/config.h"
#include "../src/os_mem.h"

#include "test_api.h"


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


extern u32 OS_MemCalcTotalBlkCnt(void);
extern MEM_MGT_S g_memMgt;
u32 TEST_MemCalcTotalBlkCnt(void)
{
    u32 totalBlkCnt = 0;
    g_memMgt.pMemStartAddr = (void*)0x1;
    g_memMgt.memLen = 0x100;
    g_memMgt.blkSize = MEM_BLK_SZIE;

    g_memMgt.pMemAlignStartAddr = (void*)OS_MemAlignToX((PTR)g_memMgt.pMemStartAddr, g_memMgt.blkSize);
    totalBlkCnt = OS_MemCalcTotalBlkCnt();
    if (0x0 != totalBlkCnt)
    {
        printf("%s(line:%d) FAIL! totalBlkCnt=%x! \r\n",__func__,__LINE__, totalBlkCnt);
        return TEST_FAIL;
    }

    g_memMgt.memLen = 0x1000;
    g_memMgt.pMemAlignStartAddr = (void*)OS_MemAlignToX((PTR)g_memMgt.pMemStartAddr, g_memMgt.blkSize);
    totalBlkCnt = OS_MemCalcTotalBlkCnt();
    if (0x0 != totalBlkCnt)
    {
        printf("%s(line:%d) FAIL! totalBlkCnt=%x! \r\n",__func__,__LINE__, totalBlkCnt);
        return TEST_FAIL;
    }

    g_memMgt.memLen = 0x2000;
    g_memMgt.pMemAlignStartAddr = (void*)OS_MemAlignToX((PTR)g_memMgt.pMemStartAddr, g_memMgt.blkSize);
    totalBlkCnt = OS_MemCalcTotalBlkCnt();
    if (0x1 != totalBlkCnt)
    {
        printf("%s(line:%d) FAIL! totalBlkCnt=%x! \r\n",__func__,__LINE__, totalBlkCnt);
        return TEST_FAIL;
    }

    g_memMgt.pMemStartAddr = (void*)0x1000;
    g_memMgt.memLen = 0x13000;
    g_memMgt.pMemAlignStartAddr = (void*)OS_MemAlignToX((PTR)g_memMgt.pMemStartAddr, g_memMgt.blkSize);
    totalBlkCnt = OS_MemCalcTotalBlkCnt();
    if (0x13 != totalBlkCnt)
    {
        printf("%s(line:%d) FAIL! totalBlkCnt=%x! \r\n",__func__,__LINE__, totalBlkCnt);
        return TEST_FAIL;
    }

    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}



extern void OS_MemBlkListHeadInit(void);
extern void OS_MemSplitToBlk(void);
extern u32 OS_MemIsAlignToX(void *v_addrOrLen, u32 v_alignSize);
extern PTR OS_MemAlignToPtr(PTR v_len);
extern void OS_MemCfgInit(void *v_pMemAddr, u32 v_memLen, u32 v_blkSize);
extern void OS_MemShowMgt(void);

u32 TEST_MemSplitToblk(u32 v_memLen, u32 v_blkSize)
{
    u32 blkHeadCnt = 0;
    u32 freeBlkCnt = 0;
    void *pMemStartAddr = NULL;
    LIST_NODE_S *pListHead = &g_memMgt.freeBlkListHead;
    LIST_NODE_S *pTempHead = NULL;
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
    OS_MemBlkListHeadInit();
    OS_MemSplitToBlk();

    //Judgment result
    if (freeBlkCnt != g_memMgt.freeBlkCnt)
    {
        printf("%s(line:%d): FAIL! freeBlkCnt:0x%x 0x%x! \r\n",__func__,__LINE__,freeBlkCnt,g_memMgt.freeBlkCnt);
        free(pMemStartAddr);
        return TEST_FAIL;
    }

    pTempHead = pListHead->next;
    //printf("idx=0x%x: self=0x%p next=0x%p pre=0x%p\r\n",
    //    blkHeadCnt, pListHead, pListHead->next, pListHead->pre);

    while(pTempHead != pListHead)
    {

        //printf("idx=0x%x: self=0x%p next=0x%p pre=0x%p\r\n",
        //    blkHeadCnt, pTempHead, pTempHead->next, pTempHead->pre);

        blkHeadCnt++;
        pTempHead = pTempHead->next;
    }

    if (blkHeadCnt != g_memMgt.blkSize / sizeof(MEM_BLK_HEAD_S))
    {
        printf("%s(line:%d): FAIL! blkHeadCnt:0x%x 0x%x! \r\n",__func__,__LINE__,blkHeadCnt,(u32)(g_memMgt.blkSize / sizeof(MEM_BLK_HEAD_S)));
        free(pMemStartAddr);
        return TEST_FAIL;
    }

    pBlkHead = (MEM_BLK_HEAD_S *)(pListHead->next);
    if ((pBlkHead->blkCnt != freeBlkCnt) || (pBlkHead->blkIdx != ((PTR)g_memMgt.pMemAlignStartAddr / g_memMgt.blkSize + 1)))
    {
        printf("%s(line:%d): FAIL!\r\n",__func__,__LINE__);
        free(pMemStartAddr);
        return TEST_FAIL;
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

void* OS_MemGetBlkHeadByCnt(u32 v_blkCnt);
u32 TEST_MemGetBlkNodeByCnt(void)
{
    MEM_BLK_HEAD_S blkNodeOne = {{0,0},0,0};
    MEM_BLK_HEAD_S blkNodeTwo = {{0,0},0,0};
    MEM_BLK_HEAD_S blkNodeThree = {{0,0},0,0};
    MEM_BLK_HEAD_S blkNodeFour = {{0,0},0,0};

    MEM_BLK_HEAD_S *testRes1 = NULL;
    MEM_BLK_HEAD_S *testRes3 = NULL;
    MEM_BLK_HEAD_S *testRes4 = NULL;
    MEM_BLK_HEAD_S *testRes6 = NULL;
    MEM_BLK_HEAD_S *testRes0 = NULL;

    //Pre-condition
    LIST_HEAD_INIT(&g_memMgt.freeBlkListHead);
    blkNodeOne.blkCnt = 1;
    blkNodeTwo.blkCnt = 3;
    blkNodeThree.blkCnt = 4;
    blkNodeFour.blkCnt = 0;

    LIST_ADD_TAIL(&(blkNodeOne.blkNode),&g_memMgt.freeBlkListHead);
    LIST_ADD_TAIL(&(blkNodeTwo.blkNode),&g_memMgt.freeBlkListHead);
    LIST_ADD_TAIL(&(blkNodeThree.blkNode),&g_memMgt.freeBlkListHead);
    LIST_ADD_TAIL(&(blkNodeFour.blkNode),&g_memMgt.freeBlkListHead);

    //Excute
    testRes1 = OS_MemGetBlkHeadByCnt(1);
    testRes3 = OS_MemGetBlkHeadByCnt(3);
    testRes4 = OS_MemGetBlkHeadByCnt(4);
    testRes6 = OS_MemGetBlkHeadByCnt(6);
    testRes0 = OS_MemGetBlkHeadByCnt(0);

    if (!((testRes1 == &blkNodeOne) \
        && (testRes3 == &blkNodeTwo) \
        && (testRes4 == &blkNodeThree) \
        && (testRes6 == NULL) \
        && (testRes0 == NULL)))
    {
        //printf("%s(%d):%p %p %p %p %p    %p %p %p %p!\r\n",__func__,__LINE__,
        //    testRes1,testRes3,testRes4,testRes6,testRes0,  &blkNodeOne.blkNode,&blkNodeTwo.blkNode,&blkNodeThree.blkNode,&blkNodeFour.blkNode);
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }
        
    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}

extern void OS_MemBlkNodeInsertToFreeList(MEM_BLK_HEAD_S *v_pBlkHead);
u32 TEST_MemBlkNodeInsertToFreeList(void)
{
    MEM_BLK_HEAD_S blkNodeOne = {{0,0},0,0};
    MEM_BLK_HEAD_S blkNodeTwo = {{0,0},0,0};
    MEM_BLK_HEAD_S blkNodeThree = {{0,0},0,0};
    MEM_BLK_HEAD_S blkNodeFour = {{0,0},0,0};
    MEM_BLK_HEAD_S blkNodeFive = {{0,0},0,0};
    MEM_BLK_HEAD_S insertNodeOne = {{0,0},0,0};
    MEM_BLK_HEAD_S insertNodeTwo = {{0,0},0,0};
    MEM_BLK_HEAD_S insertNodeThree = {{0,0},0,0 };

    //Pre-condition
    LIST_HEAD_INIT(&g_memMgt.freeBlkListHead);
    blkNodeOne.blkCnt = 1;
    blkNodeTwo.blkCnt = 3;
    blkNodeThree.blkCnt = 4;
    blkNodeFour.blkCnt = 6;
    blkNodeFive.blkCnt = 0;

    LIST_ADD_TAIL(&(blkNodeOne.blkNode),&g_memMgt.freeBlkListHead);
    LIST_ADD_TAIL(&(blkNodeTwo.blkNode),&g_memMgt.freeBlkListHead);
    LIST_ADD_TAIL(&(blkNodeThree.blkNode),&g_memMgt.freeBlkListHead);
    LIST_ADD_TAIL(&(blkNodeFour.blkNode),&g_memMgt.freeBlkListHead);
    LIST_ADD_TAIL(&(blkNodeFive.blkNode),&g_memMgt.freeBlkListHead);

    //printf("%s(%d):%p %p %p %p!\r\n",__func__,__LINE__,
    //    &blkNodeOne.blkNode,&blkNodeTwo.blkNode,&blkNodeThree.blkNode,&blkNodeFour.blkNode);

    //Excute
    insertNodeOne.blkCnt = 1;
    OS_MemBlkNodeInsertToFreeList(&insertNodeOne);
    insertNodeTwo.blkCnt = 3;
    OS_MemBlkNodeInsertToFreeList(&insertNodeTwo);
    insertNodeThree.blkCnt = 8;
    OS_MemBlkNodeInsertToFreeList(&insertNodeThree);

    //printf("%s(%d):%p %p %p %p!\r\n", __func__, __LINE__,
    //	(&blkNodeOne.blkNode)->pre, (&blkNodeTwo.blkNode)->pre, (&blkNodeThree.blkNode)->pre, (&blkNodeFour.blkNode)->pre);

    if (((&blkNodeOne.blkNode)->pre != (LIST_NODE_S*)&insertNodeOne) \
        || ((&blkNodeTwo.blkNode)->pre != (LIST_NODE_S*)&insertNodeTwo) \
        || (&blkNodeFour.blkNode)->next != (LIST_NODE_S*)&insertNodeThree)
    {
        //printf("%s(%d):%p %p \r\n",__func__,__LINE__,
        //    &insertNodeOne, &insertNodeTwo);
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}


u32 TEST_MemSplitOneBlkToBlkHeads(void)
{
    return TEST_PASS;
}


extern u32 OS_MemCalcOptimalBlkCnt(u16 v_pageSize, u16 v_refPageCnt);
u32 TEST_MemCalcOptimalBlkCnt(void)
{
    //Pre-condition
    g_memMgt.blkSize = 4096;

    //Excute
    if (!(1 == OS_MemCalcOptimalBlkCnt(1000,10) \
        && 2 == OS_MemCalcOptimalBlkCnt(1025,5)))
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}

extern void OS_MemSplitBlkToPagePool(MEM_PAGE_POOL_MGT_S *v_pPagePoolMgt);
u32 TEST_MemSplitBlkToPagePool(u16 v_pageSize, u16 v_refPageNum)
{
    u16 reqBlkCnt = 0;
    u16 pageActualSize = 0;
    u32 memLen = 0x3000;
    u32 blkSize = 0x1000;
    u32 pageIdx = 0;
    u32 freePageCnt = 0;
    void *pMemStartAddr = NULL;
    PTR pPageAddr = 0;
    PTR pPageHeadAddr = 0;
    MEM_PAGE_HEAD_S *pPageHead = NULL;
    MEM_PAGE_HEAD_S *pFirstPageHead = NULL;
    MEM_PAGE_POOL_MGT_S tempPool = { 0 };

    tempPool.refPageNum = v_refPageNum;
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
    pageActualSize = (u16)OS_MemAlignToPtr(sizeof(MEM_PAGE_HEAD_S));
    reqBlkCnt = OS_MemCalcOptimalBlkCnt(pageActualSize, (tempPool.refPageNum - tempPool.freePageCnt));

    //Excute
    OS_MemBlkListHeadInit();
    OS_MemSplitToBlk();
    OS_MemSplitBlkToPagePool(&tempPool);

    //Judgment result
    freePageCnt = (blkSize * reqBlkCnt) / (tempPool.pageSize + (u16)OS_MemAlignToPtr(sizeof(MEM_PAGE_HEAD_S)));
    if (freePageCnt != tempPool.freePageCnt)
    {
        printf("%s(line:%d): malloc FAIL! \r\n",__func__,__LINE__);
        free(pMemStartAddr);
        return TEST_FAIL;
    }

    //pUsedFirstBlk = (MEM_BLK_HEAD_S*)((&tempPool.usedBlkListHead)->next);
    pFirstPageHead = (MEM_PAGE_HEAD_S*)((&tempPool.freePageListHead)->next);
    pPageHead = (MEM_PAGE_HEAD_S*)((&tempPool.freePageListHead)->next);
    //blkAddr = (PTR)(pUsedFirstBlk->blkIdx * g_memMgt.blkSize);

    for (pageIdx = 0; pageIdx < freePageCnt; pageIdx++)
    {          
        pPageHeadAddr = (tempPool.pageSize + (u16)OS_MemAlignToPtr(sizeof(MEM_PAGE_HEAD_S))) *pageIdx + (PTR)pFirstPageHead;
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
    res |= TEST_MemSplitBlkToPagePool(1024, 2);

    if (TEST_PASS == res)
    {
        printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    }

    return res;
}


extern u8 OS_MemFindOptimalPagePool(u32 v_memSize);
u32 TEST_MemFindOptimalPagePool(void)
{
    MEM_PAGE_POOL_MGT_S *poolMgt = &g_memMgt.poolMgt[0];

    //Pre-condition
    poolMgt->mid = MID_OS;
    poolMgt->pageSize = 64;
    poolMgt++;
    poolMgt->mid = MID_OS;
    poolMgt->pageSize = 128;
    poolMgt++;
    poolMgt->mid = MID_OS;
    poolMgt->pageSize = 1024;
    poolMgt++;
    poolMgt->mid = MID_TEST;

    //Excute
    if (!(2 == OS_MemFindOptimalPagePool(512) \
        && 1 == OS_MemFindOptimalPagePool(128) \
        && 0 == OS_MemFindOptimalPagePool(32) \
        && MEM_INVALID_POOL == OS_MemFindOptimalPagePool(1025)))
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}


extern void *OS_MemBlkFindMergeableNode(u32 v_blkCnt, u32 v_blkIdx);
u32 TEST_MemBlkFindMergeableNode(void)
{
    MEM_BLK_HEAD_S *pBlkHead = NULL;
    MEM_BLK_HEAD_S blkHeadOne = { 0 };
    MEM_BLK_HEAD_S blkHeadTwo = { 0 };
    MEM_BLK_HEAD_S blkHeadThree = { 0 };
    LIST_NODE_S *pBlkListHead = &g_memMgt.freeBlkListHead;

    //1
    LIST_HEAD_INIT(pBlkListHead);
    pBlkHead = (MEM_BLK_HEAD_S*)OS_MemBlkFindMergeableNode(1,1);
    if (NULL != pBlkHead)
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    //2
    LIST_HEAD_INIT(pBlkListHead);
    blkHeadOne.blkIdx = 5;
    blkHeadOne.blkCnt = 2;
    LIST_ADD_TAIL(&blkHeadOne.blkNode, pBlkListHead);
    blkHeadTwo.blkIdx = 9;
    blkHeadTwo.blkCnt = 3;
    LIST_ADD_TAIL(&blkHeadTwo.blkNode, pBlkListHead);
    blkHeadThree.blkIdx = 0;
    blkHeadThree.blkCnt = 0;
    LIST_ADD_TAIL(&blkHeadThree.blkNode, pBlkListHead);

    pBlkHead = (MEM_BLK_HEAD_S*)OS_MemBlkFindMergeableNode(1,4);
    if (pBlkHead != &blkHeadOne)
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }
    pBlkHead = (MEM_BLK_HEAD_S*)OS_MemBlkFindMergeableNode(7,2);
    if (pBlkHead != &blkHeadOne)
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }
    pBlkHead = (MEM_BLK_HEAD_S*)OS_MemBlkFindMergeableNode(12,2);
    if (pBlkHead != &blkHeadTwo)
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }
    pBlkHead = (MEM_BLK_HEAD_S*)OS_MemBlkFindMergeableNode(13,2);
    if (NULL != pBlkHead)
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}


extern void* OS_MemGetUnusedBlkHead(void);
u32 TEST_MemGetUnusedBlkHead(void)
{
    MEM_BLK_HEAD_S *pBlkHead = NULL;
    LIST_NODE_S *pBlkListHead = &g_memMgt.freeBlkListHead;

    //1
    LIST_HEAD_INIT(pBlkListHead);
    pBlkHead = OS_MemGetUnusedBlkHead();
    if (NULL != pBlkHead)
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }


    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}


extern void OS_MemBlkFree(u32 v_blkIdx, u32 v_blkCnt);
u32 TEST_MemBlkFree(void)
{
    LIST_NODE_S *pNode = NULL;
    MEM_BLK_HEAD_S *pBlkHead = NULL;
    MEM_BLK_HEAD_S blkHeadOne = { 0 };
    MEM_BLK_HEAD_S blkHeadTwo = { 0 };
    MEM_BLK_HEAD_S blkHeadThree = { 0 };
    MEM_BLK_HEAD_S blkHeadFour = { 0 };
    MEM_BLK_HEAD_S blkHeadFive = { 0 };
    LIST_NODE_S *pBlkListHead = &g_memMgt.freeBlkListHead;

    //Pre-condition
    LIST_HEAD_INIT(pBlkListHead);
    blkHeadOne.blkIdx = 5;
    blkHeadOne.blkCnt = 2;
    LIST_ADD_TAIL(&blkHeadOne.blkNode, pBlkListHead);
    blkHeadTwo.blkIdx = 9;
    blkHeadTwo.blkCnt = 3;
    LIST_ADD_TAIL(&blkHeadTwo.blkNode, pBlkListHead);
    blkHeadThree.blkIdx = 13;
    blkHeadThree.blkCnt = 4;
    LIST_ADD_TAIL(&blkHeadThree.blkNode, pBlkListHead);
    blkHeadFour.blkIdx = 0;
    blkHeadFour.blkCnt = 0;
    LIST_ADD_TAIL(&blkHeadFour.blkNode, pBlkListHead);
    blkHeadFive.blkIdx = 0;
    blkHeadFive.blkCnt = 0;
    LIST_ADD_TAIL(&blkHeadFive.blkNode, pBlkListHead);

    OS_MemBlkFree(18,1);
    if (!((1 == blkHeadFive.blkCnt) && (&blkHeadFive == (MEM_BLK_HEAD_S*)pBlkListHead->next)))
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    OS_MemBlkFree(17,1);
    pNode = &blkHeadTwo.blkNode;
    if (!((6 == blkHeadThree.blkCnt) && (&blkHeadThree == (MEM_BLK_HEAD_S*)pNode->next)))
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}


extern void* OS_MemAlloc(u32 v_mid, u32 v_memSize);
extern void OS_MemInit(void);
extern void OS_MemShowPool(u8 v_poolId);
void OS_MemShowPagePoolList(MEM_PAGE_POOL_MGT_S *v_pPoolMgt);
u32 TEST_MemAllocAndFree(void)
{
    u32 memLen = 0;
    void *pMemAddr = NULL;
    u32 freeBlkCnt = g_memMgt.freeBlkCnt;
    LIST_NODE_S *pBlkListHead = &g_memMgt.freeBlkListHead;

    //
    void *pAllocPage64BOne = NULL;
    void *pAllocPage1KBOne = NULL;
    void *pAllocPage1KBTwo = NULL;
    void *pAllocPage1KBThree = NULL;
    void *pAllocPage1KBFour = NULL;
    void *pAllocPageFour = NULL;
    void *pAllocBlkOne = NULL;

    //Pre-condition
    memLen = 0x7000;
    pMemAddr = malloc(memLen);
    if (NULL == pMemAddr)
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }
    OS_MemCfgInit(pMemAddr, memLen, 0x1000);
    OS_MemInit();
    //OS_MemShowBlkList(pBlkListHead);
    //OS_MemShowPool(MEM_PAGE_POOL_MAX_NUM);
    //OS_MemShowPagePoolList(&g_memMgt.poolMgt[2]);

    //Excute
    pAllocPage1KBOne = OS_MemAlloc(MID_OS,512);
    pAllocPage1KBTwo = OS_MemAlloc(MID_OS,512);
    pAllocPage1KBThree = OS_MemAlloc(MID_OS,512);
    pAllocPage1KBFour = OS_MemAlloc(MID_OS,512);
    
    //OS_MemShowMgt();
    //OS_MemShowPool(MEM_PAGE_POOL_MAX_NUM);
    if (!((1 == g_memMgt.freeBlkCnt) && (2 == g_memMgt.poolMgt[2].freePageCnt)))
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    OS_MemFree(pAllocPage1KBOne);
    OS_MemFree(pAllocPage1KBTwo);
    OS_MemFree(pAllocPage1KBThree);
    OS_MemFree(pAllocPage1KBFour);
    //OS_MemShowPagePoolList(&g_memMgt.poolMgt[2]);
    if (!((1 == g_memMgt.freeBlkCnt) && (6 == g_memMgt.poolMgt[2].freePageCnt)))
    {
        printf("%s(line:%d): FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    free(pMemAddr);
    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;

}



u32 Mem_Test(void)
{
    u32 res = TEST_PASS;
    res |= TEST_MemAlignToX();
    res |= TEST_MemCalcTotalBlkCnt();
    res |= TEST_MemSplitToblkCaseSet();
    res |= TEST_MemSplitBlkToPagePoolCaseSet();
    res |= TEST_MemGetBlkNodeByCnt();
    res |= TEST_MemBlkNodeInsertToFreeList();
    res |= TEST_MemFindOptimalPagePool();
    res |= TEST_MemCalcOptimalBlkCnt();
    res |= TEST_MemBlkFindMergeableNode();
    res |= TEST_MemGetUnusedBlkHead();
    res |= TEST_MemBlkFree();
    res |= TEST_MemAllocAndFree();
    return res;
}

