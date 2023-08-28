//
// Created by admin on 2022/6/28.
//

#ifndef PROJECT_LINK_LIST_OBJECT_H
#define PROJECT_LINK_LIST_OBJECT_H

// 链表节点定义
typedef struct linkListNode__ {
    struct linkListNode__ *prev, *next; // 前后指针
    void *data; // 数据存放处指针
} LinkList_Node;

// 链表结构体定义
typedef struct linkListObject__ {
    LinkList_Node *head, *foot; // 头尾指针
    int length; // 链表长度
} LinkList_Object;

extern LinkList_Object *linkListObject_Init();

extern LinkList_Node *linkListObject_Append(LinkList_Object *list, void *data);

extern void linkListObject_Delete(LinkList_Object *list, char deleteData);

#endif //PROJECT_LINK_LIST_OBJECT_H
