/************************************************************************/
/* name:list_api.h                                                      */
/* create data:2017/8/15                                                */
/************************************************************************/
#ifndef _LIST_API_H_
#define _LIST_API_H_

#ifndef NULL 
#define NULL 0
#endif



typedef struct tagLIST_NODE_S
{
	struct tagLIST_NODE_S *pre;
	struct tagLIST_NODE_S *next;
}LIST_NODE_S;

static inline void LIST_HEAD_INIT(LIST_NODE_S *head)
{
	head->next = NULL;
	head->pre = NULL;
	return;
}

static inline void LIST_ADD(LIST_NODE_S *newNode, LIST_NODE_S *pre, LIST_NODE_S *next)
{
	newNode->next = next;
	newNode->pre = pre;
	pre->next = newNode;
	next->pre = newNode;
	return;
}

static inline void LIST_ADD_HEAD(LIST_NODE_S *newNode, LIST_NODE_S *head)
{
	LIST_ADD(newNode,head,head->next);
	return;
}

static inline void LIST_ADD_TAIL(LIST_NODE_S *newNode, LIST_NODE_S *head)
{
	LIST_ADD(newNode,head->pre,head);
	return;
}

static inline void LIST_DEL(LIST_NODE_S *delNode, LIST_NODE_S *pre, LIST_NODE_S *next)
{
	pre->next = delNode->next;
	next->pre = delNode->pre;
	delNode->next = NULL;
	delNode->pre = NULL;
	return;
}

static inline void LIST_DEL_HEAD(LIST_NODE_S *delNode, LIST_NODE_S *head)
{
	LIST_DEL(delNode,head,delNode->next);
	return;
}

static inline void LIST_DEL_TAIL(LIST_NODE_S *delNode, LIST_NODE_S *head)
{
	LIST_DEL(delNode,delNode->pre,head);
	return;
}

#define TEMP_ADDR 0x10000
#define LIST_GET_ENTRY(nodeAddr, entryType, nodeLabel) \
	((entryType*)(((char*)(nodeAddr) + TEMP_ADDR) - (char*)(&(entryType*)TEMP_ADDR->nodeLabel)))

#endif  