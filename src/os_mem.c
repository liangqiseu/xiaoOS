#include <stdio.h>

#include "../inlcudes/config.h"
#include "../inlcudes/os_mem_api.h"


MEM_MGT_S g_memMgt = { 0 };

/*os kerbel pool id*/
u8 g_memOsPool1KB = 0;
u8 g_memOsPool256B = 0;
u8 g_memOsPool64B = 0;

/*v_alignSize must be the power of 2*/
PTR OS_MemAlignToX(PTR v_addr, u32 v_alignSize)
{
    return ((v_addr + (v_alignSize -1)) & (~(v_alignSize -1)));
}

PTR OS_MemAlignToPtr(PTR v_len)
{
    return OS_MemAlignToX(v_len,sizeof(void*));
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

void OS_MemBlkHeadInit(MEM_BLK_HEAD_S *v_pBlkHeadInfo, u32 v_poolIdx, void *v_pBlkAdr)
{
    LIST_HEAD_INIT(&v_pBlkHeadInfo->blkNode);
    v_pBlkHeadInfo->poolIdx = v_poolIdx;
    v_pBlkHeadInfo->pBlkAddr = v_pBlkAdr;
    
    return;
}


void OS_MemSplitToBlk(void)
{
    u32 blkIdx = 0;
    u32 blkSize = g_memMgt.blkSize;
    u32 totalBlkCnt = g_memMgt.totalBlkCnt;
    void* firstBlkAddr = g_memMgt.pMemAlignStartAddr;
    LIST_NODE_S *pBlkListHead = &g_memMgt.freeBlkListHead;
    MEM_BLK_HEAD_S *pBlkHeadAddr = NULL;
    void *pBlkAddr = NULL;

    pBlkHeadAddr = (MEM_BLK_HEAD_S*)firstBlkAddr;
    pBlkAddr = firstBlkAddr;
    LIST_HEAD_INIT(pBlkListHead);
    
    /*
        first block uesd to provide memory of other block heads 
        note: first block has no block head info
    */
    if (totalBlkCnt > (blkSize / sizeof(MEM_BLK_HEAD_S)))
    {
        printf("%s(line:%d): unreasonable blkSize! \r\n",__func__,__LINE__);
        return;
    }

    pBlkAddr = (void*)((PTR)pBlkAddr + blkSize);
    printf("%s(line:%d):firstFreeBlkAddr=0x%p! \r\n",__func__,__LINE__, pBlkAddr);
    
    for (blkIdx = 0; blkIdx < (totalBlkCnt - 1); blkIdx++)
    {
        OS_MemBlkHeadInit(&pBlkHeadAddr[blkIdx],OS_MEM_POOL_COMMON,pBlkAddr);
        LIST_ADD_TAIL(&pBlkHeadAddr[blkIdx].blkNode, pBlkListHead);
        pBlkAddr = (void*)((PTR)pBlkAddr + blkSize);
        g_memMgt.freeBlkCnt++;
    }
    
    return;
}


void OS_MemShowBlkList(void)
{
    u32 blkIdx = 0;
    u32 totalBlkCnt = g_memMgt.totalBlkCnt;
    MEM_BLK_HEAD_S *pBlkHead = NULL;
    LIST_NODE_S *pListHead = &g_memMgt.freeBlkListHead;

    printf("self=0x%p next=0x%p pre=0x%p\r\n", pListHead,pListHead->next, pListHead->pre);
    pBlkHead = (MEM_BLK_HEAD_S *)(pListHead->next);
    for (blkIdx = 0; blkIdx < (totalBlkCnt - 1); blkIdx++)
    {
        
        printf("idx=0x%x: self=0x%p next=0x%p pre=0x%p poolIdx=0x%x blkAddr=0x%p \r\n", 
                blkIdx, pBlkHead,pBlkHead->blkNode.next, pBlkHead->blkNode.pre, pBlkHead->poolIdx, pBlkHead->pBlkAddr);
        pBlkHead = (MEM_BLK_HEAD_S *)(pBlkHead->blkNode.next);
    }
    return;
}

void OS_MemShowMnt(void)
{
    printf("MemMnt: \r\npMemStartAddr=0x%p \r\npMemAlignStartAddr=0x%p \r\ntotalBlkCnt=0x%x \r\nfreeBlkCnt=0x%x \r\nmemLen=0x%x \r\nblkSize=0x%x \r\n", 
        g_memMgt.pMemStartAddr, g_memMgt.pMemAlignStartAddr, g_memMgt.totalBlkCnt, g_memMgt.freeBlkCnt, g_memMgt.memLen, g_memMgt.blkSize);

    return;
}


void OS_MemCfgInit(void *v_pMemAddr, u32 v_len)
{
    g_memMgt.pMemStartAddr = v_pMemAddr;
    g_memMgt.memLen = v_len;
    g_memMgt.blkSize = MEM_BLK_SZIE;
    return;
}

void OS_MemPageHeadInit(MEM_PAGE_HEAD_S *v_pPageHead, void* v_pPageAddr, u16 v_blkIdx)
{
    v_pPageHead->pPageAddr = v_pPageAddr;
    v_pPageHead->blkIdx = v_blkIdx;
    return;
}


void OS_MemSplitBlkToPagePool(MEM_PAGE_POOL_S *v_pPagePoolMgt)
{
    u16 pageIdx = 0;
    u16 pageActualSize = 0;
    u16 pageHeadAlignSize = 0;
    void *pPageAddr = NULL;
    MEM_BLK_HEAD_S *pBlkHead = NULL;
    MEM_PAGE_HEAD_S *pPageHead = NULL;
    LIST_NODE_S *pBlkListHead = &g_memMgt.freeBlkListHead;

    if (TRUE == LIST_IS_EMPTY(pBlkListHead))
    {
        v_pPagePoolMgt->poolId = (u8)OS_MEM_INVALID_POOL;
        return;
    }

    
    pBlkHead = (MEM_BLK_HEAD_S*)pBlkListHead->next;
    LIST_DEL_HEAD(pBlkListHead);
    g_memMgt.freeBlkCnt--;
    
    LIST_HEAD_INIT(&v_pPagePoolMgt->freePageListHead);
    
    pageHeadAlignSize = (u16)OS_MemAlignToPtr(sizeof(MEM_PAGE_HEAD_S));
    pageActualSize = v_pPagePoolMgt->pageSize + pageHeadAlignSize;
    printf("headSize=0x%x  headAlignSize=0x%x pageActualSize=0x%x\r\n", (u32)sizeof(MEM_PAGE_HEAD_S), pageHeadAlignSize, pageActualSize);

    /*
        Only custom one freeBlk when pool created. 
        If page number is smaller than defaultNum set by user, more pages will add to pool in runtime.
    */
    for (pageIdx = 0; pageIdx < (g_memMgt.blkSize / pageActualSize); pageIdx++)
    {
        pPageHead = (MEM_PAGE_HEAD_S*)((PTR)pBlkHead->pBlkAddr + (pageActualSize * pageIdx));
        pPageAddr = (void*)((PTR)pPageHead+pageHeadAlignSize);
        OS_MemPageHeadInit(pPageHead,pPageAddr,pBlkHead->blkIdx);
        LIST_ADD_TAIL(&pPageHead->pageNode, &v_pPagePoolMgt->freePageListHead);
        v_pPagePoolMgt->totalPageCnt++;
        v_pPagePoolMgt->freePageCnt++;
    }

    return;
}


/*v_defaultPageNum: moudle estimated memory needed in runtime, actually used number can be larger than v_defaultPageNum*/
u32 OS_MemPagePoolCreate(u32 v_mid, u32 v_pageSize, char* v_pPoolName, u32 v_defaultPageNum)
{
    static u8 poolIdx = 0;
    MEM_PAGE_POOL_S *pPagePoolMgt = NULL;
    
    if (poolIdx >= OS_MEM_PAGE_POOL_MAX_NUM)
    {
        printf("%s(line:%d): no more poolId! \r\n",__func__,__LINE__);
        return OS_MEM_INVALID_POOL;
    }

    pPagePoolMgt = &g_memMgt.poolCtrl[poolIdx];

    pPagePoolMgt->mid = v_mid;
    pPagePoolMgt->pPoolName = v_pPoolName;
    pPagePoolMgt->pageSize = v_pageSize;
    pPagePoolMgt->defaultPageNum = v_defaultPageNum;

    OS_MemSplitBlkToPagePool(pPagePoolMgt);
    poolIdx++;
    
    return pPagePoolMgt->poolId;
}


void OS_MemBuiltinPoolInit(void)
{
    g_memOsPool1KB = OS_MemPagePoolCreate(MID_OS,1024,"osPool1KB",2);
  //  g_memOsPool256B = OS_MemPagePoolCreate(MID_OS,256,"osPool256B",2);
  //  g_memOsPool64B = OS_MemPagePoolCreate(MID_OS,64,"osPool64B",2);
    
    if(OS_MEM_INVALID_POOL == g_memOsPool1KB || OS_MEM_INVALID_POOL == g_memOsPool256B || OS_MEM_INVALID_POOL == g_memOsPool64B)
    {
        printf("%s(line:%d) FAIL! %d %d %d \r\n",__func__, __LINE__, g_memOsPool1KB, g_memOsPool256B, g_memOsPool64B);
    }
    return;
}


void OS_MemInit(void)
{
    u32 totalBlkCnt = 0;
    
    g_memMgt.pMemAlignStartAddr = (void*)OS_MemAlignToX((PTR)g_memMgt.pMemStartAddr, g_memMgt.blkSize);
    totalBlkCnt = OS_MemCalcBlkCnt();
    if (0 == totalBlkCnt)
    {
        printf("%s(line:%d) FAIL! \r\n",__func__,__LINE__);
        return;
    }
    g_memMgt.totalBlkCnt = totalBlkCnt;

    OS_MemSplitToBlk();
    OS_MemShowMnt();
    OS_MemShowBlkList();
    OS_MemBuiltinPoolInit();

    return;
}

void* OS_MemAlloc(u32 mid, u32 memSize, u32 alignSize)
{
    return NULL;
}

void OS_MemFree(void *memAddr)
{
    return;
}
