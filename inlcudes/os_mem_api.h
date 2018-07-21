#ifndef _OS_MEM_API_H_
#define _OS_MEM_API_H_

#include "list_api.h"
#include "config.h"

#define OS_MEM_INVALID_POOL 0xffffffff
#define OS_MEM_PAGE_POOL_MAX_NUM 16

typedef struct tagMEM_BLK_HEAD_S
{
    LIST_NODE_S blkNode;
    u32 poolIdx;
    void *pBlkAddr;
}MEM_BLK_HEAD_S;

typedef struct tagMEM_PAGE_HEAD_S
{
    LIST_NODE_S pageNode;
    u32 poolIdx;
    u32 blkIdx;
    void *pPageAddr;
}MEM_PAGE_HEAD_S;


typedef struct tagMEM_PAGE_POOL_S
{
    LIST_NODE_S freePageListHead;
    u32 freePageCnt;
    u32 mid;
}MEM_PAGE_POOL_S;

typedef struct tagMEM_MGT_S
{
    LIST_NODE_S freeBlkListHead;
    MEM_PAGE_POOL_S poolCtrl[OS_MEM_PAGE_POOL_MAX_NUM];
    void *pMemStartAddr;
    void *pMemAlignStartAddr;
    u32 totalBlkCnt;
    u32 memLen;
    u32 blkSize;
}MEM_MGT_S;

#endif

