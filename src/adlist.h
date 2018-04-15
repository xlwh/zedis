/*************************************************************************
	> File Name: adlist.h
	> Author: zhanghongbin01 
	> Mail: zhanghongbin01@baidu.com
	> Created Time: Sun 15 Apr 2018 09:27:58 AM CST
 ************************************************************************/

#ifndef _ADLIST_H
#define _ADLIST_H

//双链表的节点的定义
typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;   //可以放任意类型的值
} listNode;

//定义双链表的迭代器
typedef struct listIter {
    listNode *next;
    int direction;   //迭代器的方向
} listIter;

//定义双链表的结构
typedef struct list {
    listNode *head;
    listNode *tail;
    void *(*dup)(void *ptr);    //复制函数指针，可以从外部传入
    void (*free)(void *ptr);
    int (*match)(void *ptr, void *key); //对比函数
    unsigned long len;    //双链表的长度定义
} list;

//定义一些宏，可以方便的取链表的一些属性
#define listLength(l) ((l)->len)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)
#define listPrevNode(n) ((n)->prev)
#define listNextNode(n) ((n)->next)
#define listNodeValue(n) ((n)->value)
#define listSetDupMethod(l, m) ((l)->dup = (m))
#define listSetFreeMethod(l, m) ((l)->free = (m))
#define listSetMatchMethod(l, m) ((l)->match = (m))

#define listGetDupMethod(l) ((l)->dup)
#define listGetFreeMethod(l) ((l)->free)
#define listGetMatchMethod(l) ((l)->match)


//对外的接口函数定义
list *listCreate(void);
void listRelease(list *list);
void listEmpty(list *list);
void *listAddNodeHead(list *list, void *value);
void *listAddNodeTail(list *list, void *value);
list *listInsertNode(list *list, void *value);
void listDelNode(list *list, listNode *node);
listIter *listGetIterator(list *list, int direction);
listNode *listNext(listIter *iter);
void listReaseIterator(listIter *iter);
list *listDup(list *orig);
listNode *listSearchKey(list *list, void *key);
listNode *listIndex(list *list, long index);
void listRewind(list *list, listIter *li);
void listRewindTail(list *list, listIter *li);
void listRotate(list *list);
void listJoin(list *l, list *o);

//宏定义，定义双链表的迭代方向等
#define AL_START_HEAD 0
#define AL_START_TAIL 1

#endif
