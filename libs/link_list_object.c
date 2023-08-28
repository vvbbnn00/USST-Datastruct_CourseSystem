//
// Created by admin on 2022/6/28.
//

/*
 * 实现链表
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "link_list_object.h"


/**
 * 链表初始化
 */
LinkList_Object *linkListObject_Init() {
    LinkList_Object *pt = malloc(sizeof(LinkList_Object));
    memset(pt, 0, sizeof(LinkList_Object));
    return pt;
}

/**
 * 链表往后追加数据
 *
 * @param list 链表对象
 * @param data 待插入数据
 * @return
 */
LinkList_Node *linkListObject_Append(LinkList_Object *list, void *data) {
    LinkList_Node *pt = calloc(1, sizeof(LinkList_Node));
    if (pt == NULL) return NULL;
    pt->data = data;
//    printf("Length:%d\n", list->length);
    if (list->length == 0) {
        list->head = pt;
        list->foot = pt;
        list->length = 1;
        return pt;
    }
    list->foot->next = pt;
    pt->prev = list->foot;
    list->foot = pt;
    list->length++;
    return pt;
}

/**
 * 释放链表包含的所有数据（不释放自身）
 * @param list
 */
void linkListObject_Delete(LinkList_Object *list, char deleteData) {
    LinkList_Node *node = list->head;
    while (node != NULL) {
        LinkList_Node *pt = node->next;
        if (node->data != NULL && deleteData) free(node->data);
        free(node);
        node = pt;
    }
}