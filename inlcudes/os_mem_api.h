#ifndef _OS_MEM_API_H_
#define _OS_MEM_API_H_

#include "config.h"
//
void OS_MemInit(void);
void OS_MemCfgInit(void *v_pMemAddr, u32 v_memLen, u32 v_blkSize);
void* OS_MemAlloc(u32 v_mid, u32 v_memSize);
void OS_MemFree(void *memAddr);


#endif

