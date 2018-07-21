#ifndef _OS_MEM_API_H_
#define _OS_MEM_API_H_

#include "list_api.h"
#include "config.h"

#define OS_MEM_INVALID_POOL 0xff
#define OS_MEM_PAGE_POOL_MAX_NUM 16

typedef struct tagMEM_BLK_HEAD_S
{
    LIST_NODE_S blkNode;
    void *pBlkAddr;
    u16 blkIdx;
    u8 poolIdx;
}MEM_BLK_HEAD_S;

typedef struct tagMEM_PAGE_HEAD_S
{
    LIST_NODE_S pageNode;
    void *pPageAddr;
    u16 blkIdx;
    u8 poolIdx;
}MEM_PAGE_HEAD_S;


typedef struct tagMEM_PAGE_POOL_S
{
    LIST_NODE_S freePageListHead;
    char *pPoolName;
    u16 defaultPageNum;
    u16 pageSize;
    u16 totalPageCnt;
    u16 freePageCnt;
    u8 mid;
    u8 poolId;
}MEM_PAGE_POOL_S;

typedef struct tagMEM_MGT_S
{
    LIST_NODE_S freeBlkListHead;
    MEM_PAGE_POOL_S poolCtrl[OS_MEM_PAGE_POOL_MAX_NUM];
    void *pMemStartAddr;
    void *pMemAlignStartAddr;
    u32 memLen;
    u32 blkSize;
    u16 totalBlkCnt;
    u16 freeBlkCnt;
}MEM_MGT_S;

typedef enum tagOS_MEM_POOL_IDX_E
{
    OS_MEM_POOL_COMMON = 0x0,
    OS_MEM_POOL_SIZE_64B,
}OS_MEM_POOL_IDX_E;

#endif

