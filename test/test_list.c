#include <stdio.h>
#include "../inlcudes/list_api.h"
#include "../inlcudes/config.h"

#include "test_api.h"


u32 TEST_ListInit(void)
{
    LIST_NODE_S tmpHead = {(void*)0x1000,(void*)0x1000};

    LIST_HEAD_INIT(&tmpHead);

    if (!((&tmpHead == tmpHead.next) && (&tmpHead == tmpHead.pre)))
    {
        printf("%s(line:%d) FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }
    
    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}


u32 TEST_ListAddHead(void)
{
    LIST_NODE_S tmpHead = {0,0};
    LIST_NODE_S nodeOne = {0,0};
    LIST_NODE_S nodeTwo = {0,0};
    LIST_NODE_S nodeThree = {0,0};
    LIST_NODE_S nodeFour = {0,0};

    LIST_HEAD_INIT(&tmpHead);
    LIST_ADD_HEAD(&nodeThree, &tmpHead);
    LIST_ADD_HEAD(&nodeTwo, &tmpHead);
    LIST_ADD_HEAD(&nodeOne, &tmpHead);

    if (!((&nodeOne == tmpHead.next) && (&nodeTwo == nodeOne.next) \
        && (&nodeThree == nodeTwo.next) && (&tmpHead == nodeThree.next)))
    {
        printf("%s(line:%d) FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }
    if (!((&nodeThree == tmpHead.pre) && (&tmpHead == nodeOne.pre) \
        && (&nodeOne == nodeTwo.pre) && (&nodeTwo == nodeThree.pre)))
    {
        printf("%s(line:%d) FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    LIST_ADD_TAIL(&nodeFour, &tmpHead);
    if (!((&nodeOne == tmpHead.next) && (&nodeTwo == nodeOne.next) \
        && (&nodeThree == nodeTwo.next) && (&nodeFour == nodeThree.next) && (&tmpHead == nodeFour.next)))
    {
        printf("%s(line:%d) FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }
    if (!((&nodeFour == tmpHead.pre) && (&tmpHead == nodeOne.pre) \
        && (&nodeOne == nodeTwo.pre) && (&nodeTwo == nodeThree.pre) && (&nodeThree == nodeFour.pre)))
    {
        printf("%s(line:%d) FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}


u32 TEST_ListDel(void)
{
    LIST_NODE_S tmpHead = {0,0};
    LIST_NODE_S nodeOne = {0,0};
    LIST_NODE_S nodeTwo = {0,0};
    LIST_NODE_S nodeThree = {0,0};
    LIST_NODE_S nodeFour = {0,0};

    LIST_HEAD_INIT(&tmpHead);
    LIST_ADD_TAIL(&nodeOne, &tmpHead);
    LIST_ADD_TAIL(&nodeTwo, &tmpHead);
    LIST_ADD_TAIL(&nodeThree, &tmpHead);
    LIST_ADD_TAIL(&nodeFour, &tmpHead);

    LIST_DEL_HEAD(&tmpHead);
    if (!((&nodeTwo == tmpHead.next) && (&nodeThree == nodeTwo.next) \
         && (&nodeFour == nodeThree.next) && (&tmpHead == nodeFour.next)))
    {
        printf("%s(line:%d) FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }
    if (!((&nodeFour == tmpHead.pre) && (&tmpHead == nodeTwo.pre) \
        && (&nodeTwo == nodeThree.pre) && (&nodeThree == nodeFour.pre)))
    {
        printf("%s(line:%d) FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    LIST_DEL_TAIL(&tmpHead);
    if (!((&nodeTwo == tmpHead.next) && (&nodeThree == nodeTwo.next) \
         && (&tmpHead == nodeThree.next)))
    {
        printf("%s(line:%d) FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }
    if (!((&nodeThree == tmpHead.pre) && (&tmpHead == nodeTwo.pre) \
        && (&nodeTwo == nodeThree.pre)))
    {
        printf("%s(line:%d) FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    LIST_DEL_TAIL(&tmpHead);
        
    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}

typedef struct tagLIST_TMP_ENTRY
{
    LIST_NODE_S list;
    unsigned int value1;
    void *data;
}LIST_TMP_ENTRY_S;

u32 TEST_ListGetEntry(void)
{
    LIST_TMP_ENTRY_S entry = {0};
    LIST_TMP_ENTRY_S *entryAddr = NULL;
    unsigned int *valueAddr = &entry.value1;
    
    entryAddr = (LIST_TMP_ENTRY_S*)LIST_GET_ENTRY(valueAddr, LIST_TMP_ENTRY_S, value1);
    if (!(entryAddr == &entry))
    {
        printf("%s(line:%d) FAIL! \r\n",__func__,__LINE__);
        return TEST_FAIL;
    }

    printf("%s(line:%d): PASS! \r\n",__func__,__LINE__);
    return TEST_PASS;
}

u32 LIST_Test(void)
{
    u32 res = TEST_PASS;
    res |= TEST_ListInit();
    res |= TEST_ListAddHead();
    res |= TEST_ListDel();
    res |= TEST_ListGetEntry();
    return res;
}


