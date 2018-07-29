#ifndef _OS_MEM_H_
#define _OS_MEM_H_

#include "../inlcudes/list_api.h"
#include "../inlcudes/config.h"

#define MEM_INVALID_POOL 0xff
#define MEM_PAGE_POOL_MAX_NUM 16

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
    LIST_NODE_S freeBlkListHead;
    MEM_PAGE_POOL_MGT_S poolMgt[MEM_PAGE_POOL_MAX_NUM];
    void *pMemStartAddr;
    void *pMemAlignStartAddr;
    u32 memLen;
    u32 blkSize;
    u16 totalBlkCnt;
    u16 freeBlkCnt;
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

#endif

