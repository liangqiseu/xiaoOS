#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../inlcudes/config.h"
#include "../inlcudes/os_mem_api.h"
#include "os_mem.h"

MEM_MGT_S g_memMgt = { 0 };
MEM_BUILDIN_POOL_S g_memOsPool = { 0 };
MEM_ALLOC_MGT_S g_memAllocMgt = { 0 };

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

void OS_MemTotalFreeBlkCntInit(u32 v_cnt)
{
    g_memMgt.totalBlkCnt = v_cnt;
    g_memMgt.freeBlkCnt = v_cnt;
    return;
}

u32 OS_MemCalcTotalBlkCnt(void)
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

    totalBlkCnt = OS_MemCalcTotalBlkCnt();
    OS_MemTotalFreeBlkCntInit(totalBlkCnt);
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



void OS_MemShowBlkList(void);
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
        else
        {
            if (0 == pBlkHeadNode->blkCnt)
            {
                LIST_ADD(&v_pBlkHead->blkNode, (&pBlkHeadNode->blkNode)->pre, &pBlkHeadNode->blkNode);
                return;
            }
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

void *OS_MemBlkFindMergeableNode(u32 v_blkIdx, u32 v_blkCnt)
{
    u32 blkEndIdx = v_blkIdx + v_blkCnt;
    MEM_BLK_HEAD_S *pBlkHead = NULL;
    LIST_NODE_S *pFreeBlkListHead = &g_memMgt.freeBlkListHead;

    pBlkHead = (MEM_BLK_HEAD_S*)pFreeBlkListHead->next;
    while (pFreeBlkListHead != (LIST_NODE_S*)pBlkHead)
    {
        if (0 == pBlkHead->blkCnt)
        {
            return NULL;
        }

        if ((blkEndIdx == pBlkHead->blkIdx) 
            || (v_blkIdx == (pBlkHead->blkIdx + pBlkHead->blkCnt)))
        {
            return pBlkHead;
        }
        pBlkHead = (MEM_BLK_HEAD_S*)pBlkHead->blkNode.next;
    }

    return NULL;
}


void OS_MemBlkMerge(u32 v_blkIdx, u32 v_blkCnt, MEM_BLK_HEAD_S *v_pMergeBlkHead)
{
    u32 newBlkIdx = 0;
    u32 newBlkCnt = 0;
    u32 blkEndIdx = v_blkIdx + v_blkCnt;
    LIST_NODE_S *pFreeBlkListHead = &g_memMgt.freeBlkListHead;

    if (blkEndIdx == v_pMergeBlkHead->blkIdx)
    {
        v_pMergeBlkHead->blkCnt += v_blkCnt;
        v_pMergeBlkHead->blkIdx -= v_blkCnt;
    }
    else
    {
        v_pMergeBlkHead->blkCnt += v_blkCnt;
        v_pMergeBlkHead->blkIdx = v_pMergeBlkHead->blkIdx;
    }
    newBlkIdx = v_pMergeBlkHead->blkIdx;
    newBlkCnt = v_pMergeBlkHead->blkCnt;

    //del old node and add this node to list tail
    LIST_DEL(&v_pMergeBlkHead->blkNode, v_pMergeBlkHead->blkNode.pre, v_pMergeBlkHead->blkNode.next);
    v_pMergeBlkHead->blkIdx = 0;
    v_pMergeBlkHead->blkCnt = 0;
    LIST_ADD_TAIL(&v_pMergeBlkHead->blkNode, pFreeBlkListHead);
    OS_MemBlkFree(newBlkIdx, newBlkCnt);

    return;
}


void* OS_MemGetUnusedBlkHead(void)
{
    void *pBlkAddr = NULL;
    MEM_BLK_HEAD_S *pTailNode = NULL;
    LIST_NODE_S *pFreeBlkListHead = &g_memMgt.freeBlkListHead;

    pTailNode = (MEM_BLK_HEAD_S*)(pFreeBlkListHead->pre);
    if ((!LIST_IS_EMPTY(pFreeBlkListHead)) && (0 == pTailNode->blkCnt))
    {
        LIST_DEL_TAIL(pFreeBlkListHead);
        return (void*)pTailNode;
    }

    pBlkAddr = OS_MemBlkAlloc(1);
    if (NULL == pBlkAddr)
    {
        return NULL;
    }
    
    OS_MemSplitOneBlkToBlkHeads((PTR)pBlkAddr);
    pTailNode = (MEM_BLK_HEAD_S*)(pFreeBlkListHead->pre);
    if ((!LIST_IS_EMPTY(pFreeBlkListHead)) && (0 == pTailNode->blkCnt))
    {
        LIST_DEL_TAIL(pFreeBlkListHead);
        return (void*)pTailNode;
    }

    return NULL;
}

void OS_MemBlkFree(u32 v_blkIdx, u32 v_blkCnt)
{
    MEM_BLK_HEAD_S *pBlkHead = NULL;

    pBlkHead = (MEM_BLK_HEAD_S*)OS_MemBlkFindMergeableNode(v_blkIdx, v_blkCnt);
    if (NULL != pBlkHead)
    {
        OS_MemBlkMerge(v_blkIdx, v_blkCnt, pBlkHead);
    }
    else
    {
        pBlkHead = (MEM_BLK_HEAD_S*)OS_MemGetUnusedBlkHead();
        pBlkHead->blkIdx = v_blkIdx;
        pBlkHead->blkCnt = v_blkCnt;
        OS_MemBlkNodeInsertToFreeList(pBlkHead);
    }

    return ;
}


void OS_MemShowBlkList(void)
{
    u32 blkIdx = 0;
    u32 totalBlkCnt = g_memMgt.totalBlkCnt;
    MEM_BLK_HEAD_S *pBlkHead = NULL;
    LIST_NODE_S *pListHead = &g_memMgt.freeBlkListHead;

    printf("BlockList:\r\n           self=0x%p next=0x%p pre=0x%p\r\n",
            pListHead,
            pListHead->next,
            pListHead->pre);
    pBlkHead = (MEM_BLK_HEAD_S *)(pListHead->next);
    while ((LIST_NODE_S*)pBlkHead != pListHead)
    {
        
        printf("  idx=0x%x: self=0x%p next=0x%p pre=0x%p blkCnt=0x%x blkIdx=0x%x \r\n", 
                blkIdx, pBlkHead,
                pBlkHead->blkNode.next,
                pBlkHead->blkNode.pre,
                pBlkHead->blkCnt,
                pBlkHead->blkIdx);
        pBlkHead = (MEM_BLK_HEAD_S *)(pBlkHead->blkNode.next);
    }
    return;
}

void OS_MemShowMgt(void)
{
    printf("MemMnt:\r\n  pMemStartAddr=0x%p\r\n  pMemAlignStartAddr=0x%p\r\n  totalBlkCnt=0x%x\r\n  freeBlkCnt=0x%x\r\n  memLen=0x%x\r\n  blkSize=0x%x\r\n", 
            g_memMgt.pMemStartAddr,
            g_memMgt.pMemAlignStartAddr,
            g_memMgt.totalBlkCnt,
            g_memMgt.freeBlkCnt,
            g_memMgt.memLen,
            g_memMgt.blkSize);

    return;
}


void OS_MemShowPagePoolList(MEM_PAGE_POOL_MGT_S *v_pPoolMgt)
{
    u32 pageIdx = 0;
    MEM_PAGE_HEAD_S *pPageHead = NULL;
    LIST_NODE_S *pListHead = &v_pPoolMgt->freePageListHead;
    pPageHead = (MEM_PAGE_HEAD_S*)pListHead->next;

    while ((LIST_NODE_S*)pPageHead != pListHead)
    {
        
        printf("  idx=0x%x: self=0x%p next=0x%p pre=0x%p poolIdx=0x%x blkIdx=0x%x \r\n", 
                pageIdx, pPageHead,
                pPageHead->pageNode.next,
                pPageHead->pageNode.pre,
                pPageHead->poolIdx,
                pPageHead->blkIdx);
        pPageHead = (MEM_PAGE_HEAD_S*)(pPageHead->pageNode.next);
        pageIdx++;
    }
    return;
}



/*if v_poolId larger than OS_MEM_PAGE_POOL_MAX_NUM, means show all pool info*/
void OS_MemShowPool(u8 v_poolId)
{
    u8 poolIdx = 0;
    u8 poolIdxMax = MEM_PAGE_POOL_MAX_NUM;
    MEM_PAGE_POOL_MGT_S *poolMgt = NULL;

    if (MEM_PAGE_POOL_MAX_NUM > v_poolId)
    {
        poolIdx = v_poolId;
        poolIdxMax = v_poolId;
    }

    printf("Pool:\r\n");
    for (; poolIdx < poolIdxMax; poolIdx++)
    {
        poolMgt = &g_memMgt.poolMgt[poolIdx];
        printf("  idx:0x%x poolName=%s defaultPageNum=0x%x pageSize=0x%x totalPageCnt=0x%x freePageCnt=0x%x mid=0x%x \r\n", 
                poolIdx,
                poolMgt->pPoolName,
                poolMgt->refPageNum,
                poolMgt->pageSize,
                poolMgt->totalPageCnt,
                poolMgt->freePageCnt,
                poolMgt->mid);
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

void* OS_MemPageAlloc(u8 v_poolId)
{
    void *pAddr = NULL;
    MEM_PAGE_HEAD_S *pPageHead = NULL;
    MEM_PAGE_POOL_MGT_S *pPagePoolMgt = NULL;
    
    pPagePoolMgt = &g_memMgt.poolMgt[v_poolId];
    if (0 == pPagePoolMgt->freePageCnt)
    {
        if (MEM_OK != OS_MemSplitBlkToPagePool(pPagePoolMgt))
        {
            return NULL;
        }
    }

    pPageHead = (MEM_PAGE_HEAD_S*)(pPagePoolMgt->freePageListHead.next);
    LIST_DEL_HEAD(&pPagePoolMgt->freePageListHead);
    pPagePoolMgt->freePageCnt--;
    pAddr = (void*)((PTR)pPageHead + OS_MemAlignToPtr(sizeof(MEM_PAGE_HEAD_S)));
    //printf("%s(line:%d) page alloc: %p \r\n",__func__, __LINE__, pAddr);
    return pAddr;
}

void OS_MemPageFree(u8 v_poolId, void *v_pMemAddr)
{
    MEM_PAGE_HEAD_S *pPageHead = NULL;
    MEM_PAGE_POOL_MGT_S *pPagePoolMgt = NULL;

    //printf("%s(line:%d) page free: %p \r\n",__func__, __LINE__, v_pMemAddr);
    pPagePoolMgt = &g_memMgt.poolMgt[v_poolId];
    pPageHead = (MEM_PAGE_HEAD_S*)((PTR)v_pMemAddr - OS_MemAlignToPtr(sizeof(MEM_PAGE_HEAD_S)));
    LIST_ADD_TAIL(&pPageHead->pageNode, &pPagePoolMgt->freePageListHead);
    pPagePoolMgt->freePageCnt++;
    return;
}

void OS_MemBuiltinPoolInit(void)
{
    g_memOsPool.memOsPool_64B = OS_MemPagePoolCreate(MID_OS,64,"osPool_64B",2);
    g_memOsPool.memOsPool_128B = OS_MemPagePoolCreate(MID_OS,128,"osPool_128B",2);
    //g_memOsPool.memOsPool_512B = OS_MemPagePoolCreate(MID_OS,64,"osPool_1KB",2);
    g_memOsPool.memOsPool_1KB = OS_MemPagePoolCreate(MID_OS,1024,"osPool_1KB",2);
    g_memOsPool.memOsPoolMaxSize = 1024;

    if(MEM_INVALID_POOL == g_memOsPool.memOsPool_64B || MEM_INVALID_POOL == g_memOsPool.memOsPool_128B || MEM_INVALID_POOL == g_memOsPool.memOsPool_1KB)
    {
        printf("%s(line:%d) FAIL! %d %d %d \r\n",__func__, __LINE__, 
                g_memOsPool.memOsPool_64B,
                g_memOsPool.memOsPool_128B,

                g_memOsPool.memOsPool_1KB);
    }
    
    return;
}

void OS_MemCfgInit(void *v_pMemAddr, u32 v_memLen, u32 v_blkSize)
{
    u8 poolIdx = 0;
    
    g_memMgt.pMemStartAddr = v_pMemAddr;
    g_memMgt.memLen = v_memLen;
    g_memMgt.blkSize = v_blkSize;
    g_memMgt.pMemAlignStartAddr = (void*)OS_MemAlignToX((PTR)g_memMgt.pMemStartAddr, g_memMgt.blkSize);
    g_memMgt.freeBlkCnt = 0;
    g_memMgt.totalBlkCnt = 0;
    
    memset(&g_memMgt.poolMgt[0],0,(sizeof(MEM_PAGE_POOL_MGT_S) * MEM_PAGE_POOL_MAX_NUM));
    for (poolIdx = 0; poolIdx < MEM_PAGE_POOL_MAX_NUM; poolIdx ++)
    {
        g_memMgt.poolMgt[poolIdx].mid = MID_MAX;
    }
    
    return;
}

void OS_MemStatsInit(void)
{
    u8 mid = 0;
    MEM_STATS_S *pMemStats = NULL;

    for (mid = 0; mid < MID_MAX; mid++)
    {
        pMemStats = &g_memAllocMgt.allocStats[mid];
        LIST_HEAD_INIT(&pMemStats->usedMemList);
    }

    return;
}

void OS_MemInit(void)
{
    OS_MemSplitToBlk();
    OS_MemShowMgt();
    OS_MemShowBlkList();
    OS_MemBuiltinPoolInit();
    OS_MemShowPool(MEM_PAGE_POOL_MAX_NUM);
    OS_MemStatsInit();

    return;
}


u8 OS_MemFindOptimalPagePool(u32 v_memSize)
{
    u8 poolId = 0;
    MEM_PAGE_POOL_MGT_S *poolMgt = &g_memMgt.poolMgt[0];

    while (MID_OS == poolMgt->mid)
    {
        if (v_memSize <= poolMgt->pageSize)
        {
            return poolId;
        }
        poolMgt++;
        poolId++;
    }

    return MEM_INVALID_POOL;
}

void OS_MemUpdateAllocMgt(u32 v_mid, u32 v_actualSize,void* v_pMemAddr)
{
    MEM_ALLOC_HEAD_S *pAllocHead = NULL;
    MEM_STATS_S *pMemStats = &g_memAllocMgt.allocStats[v_mid];

    pAllocHead = (MEM_ALLOC_HEAD_S*)v_pMemAddr;
    pAllocHead->actualSize = v_actualSize;
    pAllocHead->mid = v_mid;
    LIST_ADD_TAIL(&pAllocHead->node, &pMemStats->usedMemList);
    
    pMemStats->totalAllocTime++;
    pMemStats->curCumulSize += v_actualSize;
    return;
}


void OS_MemUpdateFreeMgt(MEM_ALLOC_HEAD_S* v_pAllocHead)
{
    MEM_STATS_S *pMemStats = &g_memAllocMgt.allocStats[v_pAllocHead->mid];

    LIST_NODE_DEL(&v_pAllocHead->node);
    pMemStats->curCumulSize -= v_pAllocHead->actualSize;
    pMemStats->totalFreeTime++;
    return;
}


void* OS_MemAlloc(u32 v_mid, u32 v_memSize)
{
    u8 poolId = 0;
    u8 blkCnt = 0;
    u32 actualSize = 0;
    u32 blkSize = g_memMgt.blkSize;
    void *pAddr = NULL;

    actualSize = (u32)OS_MemAlignToPtr(sizeof(MEM_ALLOC_HEAD_S)) + v_memSize;

    /*alloc from build-in pool or blk list*/
    poolId = OS_MemFindOptimalPagePool(actualSize);
    if (MEM_INVALID_POOL != poolId)
    {
        pAddr = OS_MemPageAlloc(poolId);
    }
    else
    {
        blkCnt = actualSize / blkSize;
        if (0 != actualSize / blkSize)
        {
            blkCnt++;
        }
        pAddr = OS_MemBlkAlloc(blkCnt);
    }
    if (NULL == pAddr)
    {
        return NULL;
    }

    OS_MemUpdateAllocMgt(v_mid,actualSize,pAddr);
    pAddr = (void*)((PTR)pAddr + (u32)OS_MemAlignToPtr(sizeof(MEM_ALLOC_HEAD_S)));
    
    return pAddr;
    
}

void OS_MemFree(void *v_pMemAddr)
{
    u8 mid = 0;
    u8 poolId = 0;
    u32 blkIdx = 0;
    u32 blkCnt = 0;
    MEM_ALLOC_HEAD_S *pAllocHead = NULL;

    pAllocHead = (MEM_ALLOC_HEAD_S*)((PTR)v_pMemAddr - OS_MemAlignToPtr(sizeof(MEM_ALLOC_HEAD_S)));
    //printf("%s(line:%d) page free: %p \r\n", __func__, __LINE__, v_pMemAddr);
    OS_MemUpdateFreeMgt(pAllocHead);
    
    poolId = OS_MemFindOptimalPagePool(pAllocHead->actualSize);
    if (MEM_INVALID_POOL != poolId)
    {
        OS_MemPageFree(poolId, (void*)pAllocHead);
    }
    else
    {
        /*no need to care about align*/
        blkIdx = (PTR)v_pMemAddr / g_memMgt.blkSize;
        blkCnt = pAllocHead->actualSize / g_memMgt.blkSize;
        OS_MemBlkFree(blkIdx, blkCnt);
    }

    return;
}
