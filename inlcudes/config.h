/************************************************************************/
/* name:config.h                                                        */
/* create data:2017/8/15                                                */
/************************************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include<stddef.h>

/* customized items*/
#define MEM_LEN
#define MEM_ADDR
#define MEM_BLK_SZIE  (1 << 12)   //4KB


typedef enum tagMOUDLE_ID_E
{
    MID_OS = 0x0,
    MID_TEST,
    MID_MAX,
}MOUDLE_ID_E;


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#ifdef STM32
typedef unsigned int PTR;   //32bit addr
#else
typedef unsigned long long PTR;  //64bit addr
#endif

#define  TRUE 1
#define  FALSE 0


#endif
