//
// Created by admin on 2022/6/28.
//

#ifndef PROJECT_LINK_LIST_OBJECT_H
#define PROJECT_LINK_LIST_OBJECT_H

// ����ڵ㶨��
typedef struct linkListNode__ {
    struct linkListNode__ *prev, *next; // ǰ��ָ��
    void *data; // ���ݴ�Ŵ�ָ��
} LinkList_Node;

// ����ṹ�嶨��
typedef struct linkListObject__ {
    LinkList_Node *head, *foot; // ͷβָ��
    int length; // ������
} LinkList_Object;

extern LinkList_Object *linkListObject_Init();

extern LinkList_Node *linkListObject_Append(LinkList_Object *list, void *data);

extern void linkListObject_Delete(LinkList_Object *list, char deleteData);

#endif //PROJECT_LINK_LIST_OBJECT_H
