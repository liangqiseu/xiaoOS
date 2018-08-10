#ifndef _OS_MEM_H_
#define _OS_MEM_H_

#include "../inlcudes/list_api.h"
#include "../inlcudes/config.h"

#define MEM_INVALID_POOL 0xff
#define MEM_PAGE_POOL_MAX_NUM 16


typedef struct tagMEM_BLK_MGT_S
{
    LIST_NODE_S freeBlkListHead;
    LIST_NODE_S unusedBlkNodeListHead;
    void *pMemStartAddr;
    void *pMemAlignStartAddr;
    u32 memLen;
    u32 blkSize;
    u16 totalBlkCnt;
    u16 freeBlkCnt;
}MEM_BLK_MGT_S;


typedef struct tagMEM_BLK_HEAD_S
{
    LIST_NODE_S blkNode;
    u32 blkIdx;
    u32 blkCnt;
}MEM_BLK_HEAD_S;

typedef struct tagMEM_PAGE_HEAD_S
{
    LIST_NODE_S pageNode;
    void *pPageAddr;
    u16 blkIdx;
    u8 poolIdx;
}MEM_PAGE_HEAD_S;


typedef struct tagMEM_PAGE_POOL_MGT_S
{
    LIST_NODE_S freePageListHead;
    LIST_NODE_S usedBlkListHead;
    char *pPoolName;
    u16 refPageNum;
    u16 pageSize;
    u16 totalPageCnt;
    u16 freePageCnt;
    u8 mid;
    u8 poolId;
}MEM_PAGE_POOL_MGT_S;

typedef struct tagMEM_MGT_S
{
    MEM_BLK_MGT_S blkMgt;
    MEM_PAGE_POOL_MGT_S poolMgt[MEM_PAGE_POOL_MAX_NUM];
}MEM_MGT_S;


typedef struct tagMEM_ALLOC_STATIC_S
{
    u8 mid;
    u32 allocTimes;
    u32 freeTimes;
}MEM_ALLOC_STATIC_S;

typedef struct tagMEM_BUILDIN_POOL_S
{
    u8 memOsPool_64B;
    u8 memOsPool_128B;
    u8 memOsPool_512B;
    u8 memOsPool_1KB;
    u16 memOsPoolMaxSize;
}MEM_BUILDIN_POOL_S;

typedef struct tagMEM_ALLOC_HEAD_S
{
    LIST_NODE_S node;
    u8 mid;
    u32 actualSize;
}MEM_ALLOC_HEAD_S;

typedef struct tagMEM_STATS_S
{
    LIST_NODE_S usedMemList;
    u32 totalAllocTime;
    u32 totalFreeTime;
    u32 curCumulSize;
}MEM_STATS_S;


typedef struct tagMEM_ALLOC_MGT_S
{
    MEM_STATS_S allocStats[MID_MAX];
}MEM_ALLOC_MGT_S;


typedef enum tagOS_MEM_POOL_IDX_E
{
    OS_MEM_POOL_COMMON = 0x0,
    OS_MEM_POOL_SIZE_64B,
}OS_MEM_POOL_IDX_E;

typedef enum tagMEM_RTN_E
{
    MEM_OK = 0x0,
    MEM_FAIL,
}MEM_RTN_E;


void OS_MemBlkFree(u32 v_blkIdx, u32 v_blkCnt);
void* OS_MemBlkAlloc(u32 v_blkCnt);

#endif

