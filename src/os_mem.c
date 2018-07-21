#include "../inlcudes/config.h"
#include "../inlcudes/os_mem_api.h"


typedef enum tagOS_MEM_POOL_IDX_E
{
    OS_MEM_POOL_COMMON = 0x0,
    OS_MEM_POOL_SIZE_64B,
}OS_MEM_POOL_IDX_E;


MEM_MGT_S g_memMgt = {0};

/*v_alignSize must be the power of 2*/
PTR OS_MemAlignToX(PTR v_addr, u32 v_alignSize)
{
    return ((v_addr + (v_alignSize -1)) & (~(v_alignSize -1)));
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
    LIST_NODE_S *blkListHead = &g_memMgt.freeBlkListHead;
    MEM_BLK_HEAD_S *blkHeadAddr = NULL;
    void *blkAddr = NULL;

    blkHeadAddr = (MEM_BLK_HEAD_S*)firstBlkAddr;
    blkAddr = firstBlkAddr;
    LIST_HEAD_INIT(blkListHead);
    
    /*
        first block uesd to provide memory of other block heads 
        note: first block has no block head info
    */
    if (totalBlkCnt > (blkSize / sizeof(MEM_BLK_HEAD_S)))
    {
        printf("%s(line:%d): unreasonable blkSize! \r\n",__func__,__LINE__);
        return;
    }

    blkAddr = (void*)((PTR)blkAddr + blkSize);
    printf("%s(line:%d):firstFreeBlkAddr=0x%p! \r\n",__func__,__LINE__,blkAddr);
    
    for (blkIdx = 0; blkIdx < (totalBlkCnt - 1); blkIdx++)
    {
        OS_MemBlkHeadInit(&blkHeadAddr[blkIdx],OS_MEM_POOL_COMMON,blkAddr);
        LIST_ADD_TAIL(&blkHeadAddr[blkIdx].blkNode, blkListHead);
        blkAddr = (void*)((PTR)blkAddr + blkSize);
    }
    
    return;
}


void OS_MemShowBlkList(void)
{
    u32 blkIdx = 0;
    u32 totalBlkCnt = g_memMgt.totalBlkCnt;
    MEM_BLK_HEAD_S *blkHead = NULL;
    LIST_NODE_S *listHead = &g_memMgt.freeBlkListHead;

    printf("self=0x%p next=0x%p pre=0x%p\r\n", listHead,listHead->next, listHead->pre);
    blkHead = (MEM_BLK_HEAD_S *)(listHead->next);
    for (blkIdx = 0; blkIdx < (totalBlkCnt - 1); blkIdx++)
    {
        
        printf("idx=0x%x: self=0x%p next=0x%p pre=0x%p poolIdx=0x%x blkAddr=0x%p \r\n", 
                blkIdx, blkHead,blkHead->blkNode.next, blkHead->blkNode.pre, blkHead->poolIdx, blkHead->pBlkAddr);
        blkHead = (MEM_BLK_HEAD_S *)(blkHead->blkNode.next);
    }
    return;
}

void OS_MemShowMnt(void)
{
    printf("MemMnt: pMemStartAddr=0x%p pMemAlignStartAddr=0x%p totalBlkCnt=0x%x memLen=0x%x blkSize=0x%x \r\n", 
        g_memMgt.pMemStartAddr, g_memMgt.pMemAlignStartAddr, g_memMgt.totalBlkCnt, g_memMgt.memLen, g_memMgt.blkSize);

    return;
}





void OS_MemCfgInit(void *v_pMemAddr, u32 v_len)
{
    g_memMgt.pMemStartAddr = v_pMemAddr;
    g_memMgt.memLen = v_len;
    g_memMgt.blkSize = MEM_BLK_SZIE;
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

    OS_MemShowMnt();
    OS_MemSplitToBlk();
    OS_MemShowBlkList();

    return;
}

void* OS_MemAlloc(u32 mid, u32 memSize, u32 alignSize)
{
    return NULL;
}

u32 OS_MemPagePoolCreate(u32 mid, u32 memSize, u32 alignSize)
{
    return 0;
}


void OS_MemFree(void *memAddr)
{
    return;
}
