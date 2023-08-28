//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_SELECTIONS_H
#define COURSESYSTEM2023_SELECTIONS_H

#include "../models.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <time.h>
#include "../common.h"
#include "../utils/avl.h"
#include "../utils/hash.h"
#include "../utils/wubi.h"
#include "users.h"
#include "courses.h"
#include "../utils/math_ext.h"

int64 AUTO_INCREMENT_SELECTION_ID = 1;  // ������ѡ��ID

AVLNode *selection_ID_Index = NULL;                 // ѡ��ID��������ѡ����Ϣ���浽�ڴ��У�ÿ���������¹���
AVLNode *selection_file_Index = NULL;               // ѡ���ļ����������ļ��ж�ȡ
AVLNode *selection_userId_Index = NULL;             // ѡ���û�ID���������ļ��ж�ȡ
AVLNode *selection_courseId_Index = NULL;           // ѡ�ογ�ID���������ļ��ж�ȡ
AVLNode *selection_userId_courseId_Index = NULL;    // ѡ���û�ID+�γ�ID���������ļ��ж�ȡ


typedef struct courseTime {
    int weekStart, weekEnd;
} CourseTime;


/**
 * ����ID��ȡѡ��
 * @param selectionId
 * @return
 */
CourseSelection *DB_getSelectionById(int64 selectionId) {
    IndexListNode *node = NULL; // AVL_searchExact(selection_ID_Index, selectionId);
    if (node == NULL) {
        // �������ڣ�����ļ��ж�ȡ
        char *filePath = calloc(100, sizeof(char));
        sprintf(filePath, "data/selection/%lld.dat", selectionId);
        FILE *fp = fopen(filePath, "rb");
        if (fp == NULL) {
            perror("[getSelectionById] ��ѡ���ļ�ʧ��");
            return NULL;
        }
        CourseSelection *selection = calloc(1, sizeof(CourseSelection));
        fread(selection, sizeof(CourseSelection), 1, fp);
        fclose(fp);
        selection->student = DB_getUserById(selection->studentId);
        selection->course = DB_getCourseById(selection->courseId);
        // ��������
        selection_ID_Index = AVL_insertNode(selection_ID_Index, selectionId, INDEX_TYPE_OBJECT, selection);
        return selection;
    }
    return (CourseSelection *) node->index.data;
}


/**
 * �����û�ID��ȡѡ���б�
 * @param userId
 * @return
 */
IndexListNode *DB_getSelectionsByUserId(int64 userId) {
    IndexListNode *node = AVL_searchExact(selection_userId_Index, userId);
    return node;
}

/**
 * ���ݿγ�ID��ȡѡ���б�
 * @param courseId
 * @return
 */
IndexListNode *DB_getSelectionsByCourseId(int64 courseId) {
    IndexListNode *node = AVL_searchExact(selection_courseId_Index, courseId);
    return node;
}

/**
 * �����û�ID�Ϳγ�ID��ȡѡ��
 * @param userId
 * @param courseId
 * @return
 */
CourseSelection *DB_getSelectionByUserIdAndCourseId(int64 userId, int64 courseId) {
    char *string = calloc(100, sizeof(char));
    sprintf(string, "%llda%lld", userId, courseId);
    IndexListNode *node = AVL_searchExact(selection_userId_courseId_Index, Hash_String(string));
    if (node == NULL) {
        return NULL;
    }
    return DB_getSelectionById((int64) node->index.data);
}

/**
 * ����ѡ����Ϣ
 * @param selection
 */
void DB_saveSelection(CourseSelection *selection) {
    // ���浽�ļ�
    char *filePath = calloc(100, sizeof(char));
    sprintf(filePath, "data/selection/%lld.dat", selection->id);
    FILE *fp = fopen(filePath, "wb");
    fwrite(selection, sizeof(CourseSelection), 1, fp);
    fclose(fp);
    DB_saveAutoIncrement();
}


/**
 * ����ѡ������
 */
void DB_saveSelectionIndex() {
    AVL_saveToFile(selection_userId_Index, "data/index/selection_userId.avl");
    AVL_saveToFile(selection_courseId_Index, "data/index/selection_courseId.avl");
    AVL_saveToFile(selection_userId_courseId_Index, "data/index/selection_userId_courseId.avl");
    AVL_saveToFile(selection_file_Index, "data/index/selection_file.avl");
    DB_saveAutoIncrement();
}

/**
 * ����Ƿ���ʱ���ͻ
 * @param courses
 * @param course
 * @return
 */
char __checkHaveTime(IndexListNode* courses, Course *course){
    if (course == NULL) {
        return 0;
    }
    if (courses == NULL) {
        return 1;
    }
    CourseTime table[7][13] = {0};
    for (IndexListNode *n = courses; n!=NULL; n=n->next){
        CourseSelection *selection = DB_getSelectionById((int64) n->index.data);
        Course *c = selection->course;
        if (c == NULL) {
            continue;
        }
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 13; j++) {
                if (c->schedule[i][j] == 1) {
                    table[i][j].weekStart = min(table[i][j].weekStart, c->weekStart);
                    table[i][j].weekEnd = max(table[i][j].weekEnd, c->weekEnd);
                }
            }
        }
    }

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 13; j++) {
            if (course->schedule[i][j] == 1) {
                if (!(course->weekStart > table[i][j].weekEnd || course->weekEnd < table[i][j].weekStart)) {
                    return 0;
                }
            }
        }
    }

    return 1;
}


/**
 * ѡ��
 * @param userId
 * @param courseId
 * @return
 */
CourseSelection *DB_selectCourse(int64 userId, int64 courseId) {
    // ����Ƿ��Ѿ�ѡ��
    char *string = calloc(100, sizeof(char));
    sprintf(string, "%llda%lld", userId, courseId);
    IndexListNode *node = AVL_searchExact(selection_userId_courseId_Index, Hash_String(string));
    if (node != NULL) {
        printf("[DB_selectCourse] �γ��ѱ�ѡ��\n");
        return NULL;
    }

    // ���γ��Ƿ����
    // printf("[DB_selectCourse] courseId = %lld\n", courseId);
    Course *course = DB_getCourseById(courseId);
    if (course == NULL) {
        printf("[DB_selectCourse] �γ̲�����\n");
        return NULL;
    }

    // ���γ��Ƿ��Ѿ���Ա
    if (course->currentMembers >= course->maxMembers) {
        printf("[DB_selectCourse] �γ�����Ա\n");
        return NULL;
    }

    // ����Ƿ���ʱ���ͻ
    IndexListNode *courses = DB_getSelectionsByUserId(userId);
    if (!__checkHaveTime(courses, course)) {
        printf("[DB_selectCourse] ѡ��ʱ����ڳ�ͻ\n");
        return NULL;
    }


    User *student = DB_getUserById(userId);
    if (student == NULL) {
        printf("[DB_selectCourse] �û�������\n");
        return NULL;
    }

    if (student->role != 0) {
        printf("[DB_selectCourse] ���û�����ѧ��\n");
        return NULL;
    }

    // ѡ��
    CourseSelection *selection = calloc(1, sizeof(CourseSelection));
    selection->id = AUTO_INCREMENT_SELECTION_ID++;
    selection->student = DB_getUserById(userId);
    selection->studentId = userId;
    selection->courseId = courseId;
    selection->score = -1;
    selection->selectionTime = time(NULL);
    DB_saveSelection(selection);

    // ��������
    selection_ID_Index = AVL_insertNode(selection_ID_Index, selection->id, INDEX_TYPE_OBJECT, selection);
    selection_userId_courseId_Index = AVL_insertNode(selection_userId_courseId_Index, Hash_String(string),
                                                     INDEX_TYPE_INT64, (void *) selection->id);
    selection_userId_Index = AVL_insertNode(selection_userId_Index, userId, INDEX_TYPE_INT64, (void *) selection->id);
    selection_courseId_Index = AVL_insertNode(selection_courseId_Index, courseId, INDEX_TYPE_INT64,
                                              (void *) selection->id);
    selection_file_Index = AVL_insertNode(selection_file_Index, selection->id, INDEX_TYPE_INT64, (void *) selection->id);

    // ���¿γ���Ϣ
    course->currentMembers++;
    DB_saveCourse(course);
    DB_saveSelectionIndex();

    return selection;
}


/**
 * �˿�
 * @param userId
 * @param courseId
 * @return
 */
CourseSelection *DB_withdrawCourse(int64 userId, int64 courseId) {
    // ����Ƿ��Ѿ�ѡ��
    char *string = calloc(100, sizeof(char));
    sprintf(string, "%llda%lld", userId, courseId);
    IndexListNode *node = AVL_searchExact(selection_userId_courseId_Index, Hash_String(string));
    if (node == NULL) {
        printf("[DB_withdrawCourse] �γ�δ��ѡ��\n");
        return NULL;
    }

    // �˿�
    CourseSelection *selection = DB_getSelectionById((int64) node->index.data);
    if (selection == NULL) {
        printf("[DB_withdrawCourse] ѡ�μ�¼������\n");
        return NULL;
    }
    selection_ID_Index = AVL_deleteNode(selection_ID_Index, selection->id);
    selection_courseId_Index = AVL_deleteNode(selection_courseId_Index, courseId);
    char *filePath = calloc(100, sizeof(char));

    // �������� - userId
    IndexListNode *node1 = AVL_searchExact(selection_userId_Index, userId);
    for (IndexListNode *n = node1; n != NULL; n = n->next) {
        if (n->index.data == (void *) selection->id) {
            if (n->next) {
                n->index = n->next->index;
                n->next = n->next->next;
                free(n->next);
            } else {
                // �˿κ���û�������ѡ��
                selection_userId_Index = AVL_deleteNode(selection_userId_Index, userId);
            }
            break;
        }
    }

    // �������� - courseId
    IndexListNode *node2 = AVL_searchExact(selection_courseId_Index, courseId);
    for (IndexListNode *n = node2; n != NULL; n = n->next) {
        if (n->index.data == (void *) selection->id) {
            if (n->next) {
                n->index = node->next->index;
                n->next = node->next->next;
                free(node->next);
            } else {
                // �˿κ�ÿγ�������ѡ��
                selection_courseId_Index = AVL_deleteNode(selection_courseId_Index, courseId);
            }
            break;
        }
    }

    // �������� - userId+courseId
    selection_userId_courseId_Index = AVL_deleteNode(selection_userId_courseId_Index, Hash_String(string));

    // �������� - file
    selection_file_Index = AVL_deleteNode(selection_file_Index, selection->id);

    // ���¿γ���Ϣ
    Course *course = DB_getCourseById(courseId);
    course->currentMembers--;

    sprintf(filePath, "data/selection/%lld.dat", selection->id);
    remove(filePath);
    DB_saveCourse(course);

    DB_saveSelectionIndex();

    return selection;
}


char DB_updateSelectionScore(int64 selectionId, int score) {
    CourseSelection *selection = DB_getSelectionById(selectionId);
    if (selection == NULL) {
        printf("[DB_updateSelectionScore] ѡ�μ�¼������\n");
        return 0;
    }
    selection->score = score;
    DB_saveSelection(selection);
    return 1;
}


#endif //COURSESYSTEM2023_SELECTIONS_H
