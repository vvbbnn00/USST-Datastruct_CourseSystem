//
// Created by admin on 2022/6/28.
//


#ifndef PROJECT_LINK_LIST_OBJECT_C
#define PROJECT_LINK_LIST_OBJECT_C

#include <stdio.h>
#include <stdlib.h>
#include "link_list_object.h"


/**
 * �����ʼ��
 */
LinkList_Object *linkListObject_Init() {
    LinkList_Object *pt = calloc(1, sizeof(LinkList_Object));
    return pt;
}

/**
 * ��������׷������
 *
 * @param list �������
 * @param data ����������
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
 * �ͷ�����������������ݣ����ͷ�����
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

#endif //PROJECT_LINK_LIST_OBJECT_C