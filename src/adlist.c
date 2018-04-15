/*************************************************************************
	> File Name: adlist.c
	> Author: 
	> Mail: 
	> Created Time: Sun 15 Apr 2018 11:02:30 AM CST
 ************************************************************************/

#include <stdlib.h>
#include "adlist.h"

list *listCreate(void) {
    struct list *list;

    if ((list = malloc(sizeof(*list))) == NULL) {
        //一般可能是内存不足，申请空间失败，这里直接返回空指针
        return NULL;
    }

    //链表的头和尾都设置为空
    list->head = list->tail = NULL;
    list->len = 0;
    list->free = NULL;
    list->match = NULL;

    //返回创建好的双链表
    return list;
}

void listEmpty(list *list) {
    unsigned long len;
    listNode *current, *next;

    //从链表的头部开始，依次进行处理
    current = list->head;
    len = list->len;
    while(len--) {
        next = current->next;
        if (list->free) {
            list->free(current->value);
        } else {
            free(current);
        }
        current = next;
    }

    //链表的头和尾都设置为空
    list->head = list->tail = NULL;
    list->len = 0;
}

void listRease(list *list) {
    //先把链表设置为空
    listEmpty(list);
    free(list);
}

list *listAddNodeHead(list *list, void *value) {
    //创建新的node
    listNode *node;

    if ((node = malloc(sizeof(*node))) == NULL) {
        return NULL;
    }
    node->value = value;
    if (list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }

    list->len++;

    return list;
}

list *listAddNodeTail(list *list, void *value) {
    listNode *node;
    
    if ((node = malloc(sizeof(*node))) == NULL) {
        return NULL;
    }
    node->value = value;
    if (list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }

    list->len++;
    return list;
}

list *listInsertNode(list *list, listNode *old_node, void *value, int after) {
    listNode *node;
    
    if ((node = malloc(sizeof(*node))) == NULL) {
        return NULL;
    }

    if (after) {
        node->prev = old_node;
        node->next = old_node->next;
        if (list->tail == old_node) {
            list->tail = node;
        }
    } else {
        node->next = old_node;
        node->prev = old_node->prev;

        if (list->head == old_node) {
            list->head = node;
        }
    }

    //只有一个节点的情况
    if (node->prev != NULL) {
        node->prev->next = node;
    }
    if (node->next != NULL) {
        node->next->prev = node;
    }
    list->len++;

    //返回创建好的双链表
    return list;
}

void listDelNode(list *list, listNode *node) {
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }

    //释放节点
    if (list->free) {
        list->free(node->value);
    }

    free(node);
    list->len--;
}

listIter *listIterator(list *list, int direction) {
    listIter *iter;

    if ((iter = malloc(sizeof(*iter))) == NULL) {
        return NULL;
    }

    if (direction == AL_START_HEAD) {
        iter->next = list->head;
    } else {
        iter->next = list->tail;
    }
    iter->direction = direction;

    return iter;
}

void listReleaseIterator(listIter *iter) {
    free(iter);
}

void listRewind(list *list, listIter *li) {
    li->next = list->head;
    li->direction = AL_START_HEAD;
}

void listRewindTail(list *list, listIter *li) {
    li->next = list->head;
    li->direction = AL_START_TAIL;
}

listNode *listNext(listIter *iter) {
    listNode *current = iter->next;

    if (current != NULL) {
        if (iter->direction == AL_START_HEAD) {
            iter->next = current->next;
        } else {
            iter->next = current->prev;
        }
    }

    return current;
}

list *listDup(list *orig) {
    list *copy;
    listIter iter;
    listNode *node;

    if ((copy = listCreate()) == NULL) {
        return NULL;
    }
    //copy->dup = orig->dup;
    copy->free = orig->free;
    copy->match = orig->match;

    listRewind(orig, &iter);
    while((node = listNext(&iter)) != NULL) {
        void *value;
        if (copy->dup) {
            value = copy->dup(node->value);
            if (value == NULL) {
                listRease(copy);
                return NULL;
            }
        } else {
            value = node->value;

            if (listAddNodeTail(copy, value) == NULL) {
                listRease(copy);
                return NULL;
            }
        }
    } 

    return copy;
}

listNode *listSearchKey(list *list, void *key) {
    listIter iter;
    listNode *node;

    listRewind(list, &iter);

    while((node = listNext(&iter)) != NULL) {
        if (list->match) {
            if (list->match(node->value, key)) {
                return node;
            }
        } else {
            if (key == node->value) {
                return node;
            }
        }
    }

    return NULL;
}

listNode *listIndex(list *list, long index) {
    listNode *n;

    if (index < 0) {
        index = (-index) - 1;
        n = list->tail;
        while (index-- && n) {
            n = n->prev;
        }
    } else {
        n = list->head;
        while (index-- && n) {
            n = n ->next;
        }
    }

    return n;
}

void listRotate(list *list) {
    listNode *tail = list->tail;

    if (listLength(list) <= 1) {
        return;
    }

    list->tail = tail->prev;
    list->tail->next = NULL;
    list->head->prev = tail;
    tail->prev = NULL;
    tail->next = list->head;
    list->head = tail;
}

void listJoin(list *l, list *o) {
    if (o->head) {
        o->head->prev = l->tail;
    }

    if (l->tail) {
        l->tail->next = o->head;
    } else {
        l->head = o->head;
    }

    l->tail = o->tail;
    l->len += o->len;

    o->head = o->tail = NULL;
    o->len = 0;
}
