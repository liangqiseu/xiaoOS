#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../inlcudes/config.h"
#include "../inlcudes/os_mem_api.h"
#include "os_mem.h"

MEM_MGT_S g_memMgt = { 0 };

/*os kerbel pool id*/
u8 g_memOsPool_1KB = MEM_INVALID_POOL;
u8 g_memOsPool_256B = MEM_INVALID_POOL;
u8 g_memOsPool_64B = MEM_INVALID_POOL;

/*v_alignSize must be the power of 2*/
PTR OS_MemAlignToX(PTR v_addrOrLen, u32 v_alignSize)
{
    return ((v_addrOrLen + (v_alignSize -1)) & (~(v_alignSize -1)));
}

PTR OS_MemAlignToPtr(PTR v_len)
{
    return OS_MemAlignToX(v_len,sizeof(void*));
}

u32 OS_MemIsAlignToX(void *v_addrOrLen, u32 v_alignSize)
{
    return (0 == (PTR)v_addrOrLen % v_alignSize);
}


void OS_MemDecFreeBlkCnt(u32 v_blkCnt)
{
    g_memMgt.freeBlkCnt -= v_blkCnt;
    return;
}

void OS_MemIncFreeBlkCnt(u32 v_blkCnt)
{
    g_memMgt.freeBlkCnt += v_blkCnt;
    return;
}

void OS_MemSetTotalFreeBlkCnt(u32 v_cnt)
{
    g_memMgt.totalBlkCnt = v_cnt;
    g_memMgt.freeBlkCnt = v_cnt;
    return;
}

u32 OS_MemCalcBlkCnt(void)
{
    u32 blkCnt;
    u32 blkSize = g_memMgt.blkSize;
    PTR startAddr = (PTR)g_memMgt.pMemStartAddr;
    PTR alignStartAddr = (PTR)g_memMgt.pMemAlignStartAddr;
    u32 memLen = g_memMgt.memLen;
    
    memLen = (u32)((memLen >= (alignStartAddr - startAddr)) ? (memLen - (alignStartAddr - startAddr)) : 0);
    blkCnt = memLen / blkSize;

    return blkCnt;
}

void OS_MemBlkListHeadInit(void)
{
    LIST_NODE_S *pBlkListHead = &g_memMgt.freeBlkListHead;
    LIST_HEAD_INIT(pBlkListHead);
    
    return;
}

void OS_MemSplitOneBlkToBlkHeads(PTR v_pBlkAddr)
{
    u32 headCnt = 0;
    u32 blkHeadIdx = 0;
    u32 blkSize = g_memMgt.blkSize;
    LIST_NODE_S *pFreeBlkListHead = &g_memMgt.freeBlkListHead;
    MEM_BLK_HEAD_S *pBlkHead = NULL;

    headCnt = (blkSize / sizeof(MEM_BLK_HEAD_S));
    if (0 == headCnt)
    {
        return;
    }

    for (blkHeadIdx = 0; blkHeadIdx < (blkSize / sizeof(MEM_BLK_HEAD_S)); blkHeadIdx ++)
    {
        pBlkHead = (MEM_BLK_HEAD_S*)(v_pBlkAddr + blkHeadIdx * sizeof(MEM_BLK_HEAD_S));
        LIST_ADD_TAIL(&pBlkHead->blkNode, pFreeBlkListHead);
        //printf("idx=0x%x: self=0x%p next=0x%p pre=0x%p\r\n",
        //    blkHeadIdx, pBlkHead, pBlkHead->blkNode.next, pBlkHead->blkNode.pre);

        pBlkHead->blkIdx = 0;
        pBlkHead->blkCnt = 0;
    }
    
    return;
}




void OS_MemSplitToBlk(void)
{
    u32 totalBlkCnt = 0;
    u32 blkSize = g_memMgt.blkSize;
    PTR pBlkHeadStartAddr = 0;
    MEM_BLK_HEAD_S *pFirstBlkHead = NULL;
    LIST_NODE_S *freeBlkListHead = &g_memMgt.freeBlkListHead;

    totalBlkCnt = OS_MemCalcBlkCnt();
    OS_MemSetTotalFreeBlkCnt(totalBlkCnt);
    if (0 == g_memMgt.totalBlkCnt)
    {
        printf("%s(line:%d) FAIL! totalBlkCnt=0! \r\n", __func__, __LINE__);
        return;
    }

    /*
        first block uesd to provide memory of other block heads 
        note: first block has no block head info
    */
    pBlkHeadStartAddr = (PTR)g_memMgt.pMemAlignStartAddr;
    OS_MemBlkListHeadInit();
    OS_MemSplitOneBlkToBlkHeads(pBlkHeadStartAddr);
    if (TRUE == LIST_IS_EMPTY(freeBlkListHead))
    {
        printf("%s(line:%d): unreasonalbe blkSize! blkSize=0x%x! \r\n", __func__, __LINE__,blkSize);
        return;
    }
    OS_MemDecFreeBlkCnt(1);

    /* add all free blk to first blkNode */
    pFirstBlkHead = (MEM_BLK_HEAD_S*)(freeBlkListHead->next);
    pFirstBlkHead->blkIdx = ((PTR)g_memMgt.pMemAlignStartAddr / blkSize) + 1;
    pFirstBlkHead->blkCnt = totalBlkCnt - 1;

    return;
}


void* OS_MemGetBlkHeadByCnt(u32 v_blkCnt)
{
    MEM_BLK_HEAD_S *pBlkHeadNode = NULL;
    LIST_NODE_S *pFreeBlkListHead = &g_memMgt.freeBlkListHead;

    if (0 == v_blkCnt)
    {
        return NULL;
    }

    pBlkHeadNode = (MEM_BLK_HEAD_S*)(pFreeBlkListHead->next);
    
    while(pBlkHeadNode != (MEM_BLK_HEAD_S*)pFreeBlkListHead)
    {
        if(v_blkCnt <= pBlkHeadNode->blkCnt)
        {
            return (void*)pBlkHeadNode;
        }
        pBlkHeadNode = (MEM_BLK_HEAD_S*)((&(pBlkHeadNode->blkNode))->next);
    }
    return NULL;
}



void OS_MemBlkNodeInsertToFreeList(MEM_BLK_HEAD_S *v_pBlkHead)
{
    MEM_BLK_HEAD_S *pBlkHeadNode = NULL;
    LIST_NODE_S *pFreeBlkListHead = &g_memMgt.freeBlkListHead;
    

    if (0 == v_pBlkHead->blkCnt)
    {
        LIST_ADD_TAIL(&v_pBlkHead->blkNode,pFreeBlkListHead);
        return;
    }

    pBlkHeadNode = (MEM_BLK_HEAD_S*)pFreeBlkListHead->next;

    while(v_pBlkHead->blkCnt > pBlkHeadNode->blkCnt)
    {
        pBlkHeadNode = (MEM_BLK_HEAD_S*)(&pBlkHeadNode->blkNode)->next;
        if (pFreeBlkListHead == (LIST_NODE_S*)pBlkHeadNode)
        {
            LIST_ADD_TAIL(&v_pBlkHead->blkNode, pFreeBlkListHead);
            return;
        }
    }
    LIST_ADD(&v_pBlkHead->blkNode, (&pBlkHeadNode->blkNode)->pre, &pBlkHeadNode->blkNode);

    return;
    
}

void* OS_MemBlkAlloc(u32 v_blkCnt)
{
    void* pBlkAddr = 0;
    MEM_BLK_HEAD_S *pBlkHead = NULL;
    

    pBlkHead = OS_MemGetBlkHeadByCnt(v_blkCnt);
    if (NULL == pBlkHead)
    {
        return NULL;
    }

    pBlkAddr = (void*)(PTR)(pBlkHead->blkIdx * g_memMgt.blkSize);

    LIST_NODE_DEL(&pBlkHead->blkNode);
    pBlkHead->blkCnt -= v_blkCnt;
    pBlkHead->blkIdx += v_blkCnt;
    OS_MemDecFreeBlkCnt(v_blkCnt);
    OS_MemBlkNodeInsertToFreeList(pBlkHead);

    return pBlkAddr;
}


void* OS_MemGetUnusedBlkHead(void)
{
    MEM_BLK_HEAD_S *pTailNode = NULL;
    LIST_NODE_S *pFreeBlkListHead = &g_memMgt.freeBlkListHead;

    pTailNode = (MEM_BLK_HEAD_S*)(pFreeBlkListHead->pre);
    if (0 == pTailNode->blkCnt)
    {
        return (void*)pTailNode;
    }

    OS_MemSplitOneBlkToBlkHeads((PTR)OS_MemBlkAlloc(1));

    pTailNode = (MEM_BLK_HEAD_S*)(pFreeBlkListHead->pre);
    if (0 == pTailNode->blkCnt)
    {
        return (void*)pTailNode;
    }

    return NULL;
}



void OS_MemShowBlkList(void)
{
    u32 blkIdx = 0;
    u32 totalBlkCnt = g_memMgt.totalBlkCnt;
    MEM_BLK_HEAD_S *pBlkHead = NULL;
    LIST_NODE_S *pListHead = &g_memMgt.freeBlkListHead;

    printf("BlockList:\r\n           self=0x%p next=0x%p pre=0x%p\r\n", pListHead,pListHead->next, pListHead->pre);
    pBlkHead = (MEM_BLK_HEAD_S *)(pListHead->next);
    for (blkIdx = 0; blkIdx < (totalBlkCnt - 1); blkIdx++)
    {
        
        printf("  idx=0x%x: self=0x%p next=0x%p pre=0x%p blkCnt=0x%x blkIdx=0x%x \r\n", 
                blkIdx, pBlkHead, pBlkHead->blkNode.next, pBlkHead->blkNode.pre, pBlkHead->blkCnt, pBlkHead->blkIdx);
        pBlkHead = (MEM_BLK_HEAD_S *)(pBlkHead->blkNode.next);
    }
    return;
}

void OS_MemShowMgt(void)
{
    printf("MemMnt:\r\n  pMemStartAddr=0x%p\r\n  pMemAlignStartAddr=0x%p\r\n  totalBlkCnt=0x%x\r\n  freeBlkCnt=0x%x\r\n  memLen=0x%x\r\n  blkSize=0x%x\r\n", 
        g_memMgt.pMemStartAddr, g_memMgt.pMemAlignStartAddr, g_memMgt.totalBlkCnt, g_memMgt.freeBlkCnt, g_memMgt.memLen, g_memMgt.blkSize);

    return;
}

/*if v_poolId larger than OS_MEM_PAGE_POOL_MAX_NUM, means show all pool info*/
void OS_MemShowPool(u8 v_poolId)
{
    u8 poolIdx = 0;
    u8 poolIdxMax = MEM_PAGE_POOL_MAX_NUM - 1;
    MEM_PAGE_POOL_MGT_S *poolMgt = NULL;

    if (MEM_PAGE_POOL_MAX_NUM >= v_poolId)
    {
        poolIdx = v_poolId;
        poolIdxMax = v_poolId;
    }

    printf("Pool:\r\n");
    for (poolIdx = poolIdx; poolIdx <= poolIdxMax; poolIdx++)
    {
        poolMgt = &g_memMgt.poolMgt[poolIdx];
        printf("  idx:0x%x poolName=%s defaultPageNum=0x%x pageSize=0x%x totalPageCnt=0x%x freePageCnt=0x%x mid=0x%x \r\n", 
            poolIdx, poolMgt->pPoolName, poolMgt->refPageNum, poolMgt->pageSize, poolMgt->totalPageCnt, poolMgt->freePageCnt, poolMgt->mid);
    }
    
    return;
}


void OS_MemPageHeadInit(MEM_PAGE_HEAD_S *v_pPageHead, void* v_pPageAddr)
{
    v_pPageHead->pPageAddr = v_pPageAddr;
    return;
}


u32 OS_MemCalcOptimalBlkCnt(u16 v_pageSize, u16 v_refPageCnt)
{
    u8 cnt = 1;
    u32 totalSize = 0;
    u32 blkSize = g_memMgt.blkSize;

    while(TRUE)

    {
        totalSize = cnt * blkSize;
        if ((totalSize % v_pageSize <= blkSize / 8) \
            || (totalSize / v_pageSize >= v_refPageCnt))
        {
            break;
        }
        cnt ++;
    }
    return cnt;
}

u32 OS_MemSplitBlkToPagePool(MEM_PAGE_POOL_MGT_S *v_pPagePoolMgt)
{
    u16 reqBlkCnt = 0;
    u16 pageIdx = 0;
    u16 pageActualSize = 0;
    u16 pageHeadAlignSize = 0;
    void *pPageAddr = NULL;
    PTR blkAddr = 0;
    MEM_PAGE_HEAD_S *pPageHead = NULL;
    LIST_NODE_S *pBlkListHead = &g_memMgt.freeBlkListHead;

    if (TRUE == LIST_IS_EMPTY(pBlkListHead))
    {
        return MEM_FAIL;
    }
    
    pageHeadAlignSize = (u16)OS_MemAlignToPtr(sizeof(MEM_PAGE_HEAD_S));
    pageActualSize = v_pPagePoolMgt->pageSize + pageHeadAlignSize;
    reqBlkCnt = OS_MemCalcOptimalBlkCnt(pageActualSize,(v_pPagePoolMgt->refPageNum - v_pPagePoolMgt->freePageCnt));
    blkAddr = (PTR)OS_MemBlkAlloc(reqBlkCnt);
    
    //printf("headSize=0x%x  headAlignSize=0x%x pageActualSize=0x%x\r\n", (u32)sizeof(MEM_PAGE_HEAD_S), pageHeadAlignSize, pageActualSize);
    
    for (pageIdx = 0; pageIdx < ((g_memMgt.blkSize * reqBlkCnt)/ pageActualSize); pageIdx++)
    {
        pPageHead = (MEM_PAGE_HEAD_S*)(blkAddr + (pageActualSize * pageIdx));
        pPageAddr = (void*)((PTR)pPageHead+pageHeadAlignSize);
        OS_MemPageHeadInit(pPageHead,pPageAddr);
        LIST_ADD_TAIL(&pPageHead->pageNode, &v_pPagePoolMgt->freePageListHead);
        v_pPagePoolMgt->totalPageCnt++;
        v_pPagePoolMgt->freePageCnt++;
    }

    return MEM_OK;
}


/*v_defaultPageNum: moudle estimated memory needed in runtime, actually used number can be larger than v_defaultPageNum*/
u32 OS_MemPagePoolCreate(u32 v_mid, u32 v_pageSize, char* v_pPoolName, u32 v_refPageNum)
{
    static u8 poolIdx = 0;
    MEM_PAGE_POOL_MGT_S *pPagePoolMgt = NULL;
    
    if (poolIdx >= MEM_PAGE_POOL_MAX_NUM)
    {
        printf("%s(line:%d): no more poolId! \r\n",__func__,__LINE__);
        return MEM_INVALID_POOL;
    }

    pPagePoolMgt = &g_memMgt.poolMgt[poolIdx];

    pPagePoolMgt->mid = v_mid;
    pPagePoolMgt->pPoolName = v_pPoolName;
    pPagePoolMgt->pageSize = v_pageSize;
    pPagePoolMgt->refPageNum = v_refPageNum;

    LIST_HEAD_INIT(&pPagePoolMgt->freePageListHead);
    LIST_HEAD_INIT(&pPagePoolMgt->usedBlkListHead);
    if (MEM_FAIL == OS_MemSplitBlkToPagePool(pPagePoolMgt))
    {
        return MEM_INVALID_POOL;
    }

    pPagePoolMgt->poolId = poolIdx;
    poolIdx++;

    return pPagePoolMgt->poolId;
}


void OS_MemBuiltinPoolInit(void)
{
    g_memOsPool_1KB = OS_MemPagePoolCreate(MID_OS,1024,"osPool_1KB",2);
    g_memOsPool_256B = OS_MemPagePoolCreate(MID_OS,256,"osPool256B",2);
    g_memOsPool_64B = OS_MemPagePoolCreate(MID_OS,64,"osPool64B",2);
    
    if(MEM_INVALID_POOL == g_memOsPool_1KB || MEM_INVALID_POOL == g_memOsPool_256B || MEM_INVALID_POOL == g_memOsPool_64B)
    {
        printf("%s(line:%d) FAIL! %d %d %d \r\n",__func__, __LINE__, g_memOsPool_1KB, g_memOsPool_256B, g_memOsPool_64B);
    }
    
    return;
}

void OS_MemCfgInit(void *v_pMemAddr, u32 v_memLen, u32 v_blkSize)
{
    g_memMgt.pMemStartAddr = v_pMemAddr;
    g_memMgt.memLen = v_memLen;
    g_memMgt.blkSize = v_blkSize;
    g_memMgt.pMemAlignStartAddr = (void*)OS_MemAlignToX((PTR)g_memMgt.pMemStartAddr, g_memMgt.blkSize);
    g_memMgt.freeBlkCnt = 0;
    g_memMgt.totalBlkCnt = 0;
    memset(&g_memMgt.poolMgt[0],0,(sizeof(MEM_PAGE_POOL_MGT_S) * MEM_PAGE_POOL_MAX_NUM));
    return;
}


void OS_MemInit(void)
{
    u32 memLen = 0x7000;
    void *pMemAddr = NULL;

    pMemAddr = malloc(memLen);
    if (NULL == pMemAddr)
    {
        return;
    }
    OS_MemCfgInit(pMemAddr, memLen, 0x1000);
    OS_MemSplitToBlk();
    OS_MemShowMgt();
    OS_MemShowBlkList();
    OS_MemBuiltinPoolInit();
    OS_MemShowPool(0);
    free(pMemAddr);

    return;
}

void* OS_MemAlloc(u32 v_mid, u32 v_memSize)
{
    u32 blkSize = g_memMgt.blkSize;

    return NULL;
}

void OS_MemFree(void *memAddr)
{
    return;
}
