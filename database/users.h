//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_USERS_H
#define COURSESYSTEM2023_USERS_H

#include "../models.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include "../common.h"
#include "../utils/avl.h"
#include "../utils/hash.h"
#include "../utils/wubi.h"
#include "database.h"
#include "selections.h"

int64 AUTO_INCREMENT_USER_ID = 1;   // �������û�ID

AVLNode *user_ID_Index = NULL;      // �û�ID���������û���Ϣ���浽�ڴ��У�ÿ���������¹���
AVLNode *user_file_Index = NULL;    // �û��ļ����������ļ��ж�ȡ
AVLNode *user_name_Index = NULL;    // �û��������������ļ��ж�ȡ
AVLNode *user_empId_Index = NULL;   // �û��������������ļ��ж�ȡ


/**
 * ����ID��ȡ�û�
 * @param userId
 * @return
 */
User *DB_getUserById(int64 userId) {
    IndexListNode *node = NULL; // AVL_searchExact(user_ID_Index, userId);
    if (node == NULL) {
        // �������ڣ�����ļ��ж�ȡ
        char *filePath = calloc(100, sizeof(char));
        sprintf(filePath, "data/user/%lld.dat", userId);
        FILE *fp = fopen(filePath, "rb");
        if (fp == NULL) {
            perror("[getUserById] ���û��ļ�ʧ��");
            return NULL;
        }
        User *user = calloc(1, sizeof(User));
        fread(user, sizeof(User), 1, fp);
        fclose(fp);
        // ��������
        // user_ID_Index = AVL_insertNode(user_ID_Index, userId, INDEX_TYPE_OBJECT, user);
        return user;
    }
    return (User *) node->index.data;
}

/**
 * ����ѧ���Ż�ȡ�û�
 * @param empId
 * @return
 */
User *DB_getUserByEmpId(char *empId) {
    IndexListNode *node = AVL_searchExact(user_empId_Index, Hash_String(empId));
    if (node == NULL) {
        return NULL;
    }
    // printf("%p\n", node);
    int64 userId = (int64) node->index.data;
    return DB_getUserById(userId);
}

/**
 * ����������ȡ�û�
 * @param name
 * @return
 */
NodeList *DB_getUsersByName(char *name) {
    NodeList *list = NULL;
    char *preprocessedName = Hash_PreprocessString(Wubi_chn2wubi(name));
    int64 hashStart = Hash_String(preprocessedName);
    preprocessedName[strlen(preprocessedName) - 1]++;
    int64 hashEnd = Hash_String(preprocessedName);
    free(preprocessedName);
    AVL_inOrderSearch(user_name_Index, hashStart, hashEnd, &list);
    return list;
}

/**
 * �����û�
 * @param user
 */
void DB_saveUser(User *user) {
    char *filePath = calloc(100, sizeof(char));
    sprintf(filePath, "data/user/%lld.dat", user->id);
    FILE *fp = fopen(filePath, "wb");
    if (fp == NULL) {
        perror("[saveUser] ���û��ļ�ʧ��");
        return;
    }
    fwrite(user, sizeof(User), 1, fp);
    fclose(fp);
}

/**
 * �����û�����
 */
void DB_saveUserIndex() {
    AVL_saveToFile(user_name_Index, "data/index/user_name.avl");
    AVL_saveToFile(user_empId_Index, "data/index/user_empId.avl");
    AVL_saveToFile(user_file_Index, "data/index/user_file.avl");
    DB_saveAutoIncrement();
}

/**
 * ע���û�
 * @param name ����
 * @param empId ѧ����
 * @param passwd ����
 * @param role ��ɫ
 * @param contact ��ϵ��ʽ
 * @return
 */
User *DB_registerUser(char *name, char *empId, char *passwd, int role, char *contact) {
    // ���empId�Ƿ��Ѿ�����
    if (DB_getUserByEmpId(empId) != NULL) {
        printf("[registerUser] ��ѧ�����Ѿ���ע��\n");
        return NULL;
    }

    // ����ע��
    User *user = calloc(1, sizeof(User));
    user->id = AUTO_INCREMENT_USER_ID++;
    strcpy(user->name, name);
    strcpy(user->empId, empId);
    char *pwd = calcHexHMACSHA256(passwd, SECRET_KEY);
    strcpy(user->passwd, pwd);
    free(pwd);
    user->role = role;
    strcpy(user->contact, contact);
    user->lastLoginTime = 0;
    DB_saveUser(user);
    int64 hash = Hash_String(Wubi_chn2wubi(name));
    user_file_Index = AVL_insertNode(user_file_Index, user->id, INDEX_TYPE_INT64, (void *) user->id);
    user_name_Index = AVL_insertNode(user_name_Index, hash, INDEX_TYPE_INT64,
                                     (void *) user->id);
    user_empId_Index = AVL_insertNode(user_empId_Index, Hash_String(empId), INDEX_TYPE_INT64, (void *) user->id);
    user_ID_Index = AVL_insertNode(user_ID_Index, user->id, INDEX_TYPE_OBJECT, user);
    DB_saveUserIndex();
    return user;
}


/**
 * �����û���Ϣ
 * @param user
 */
void DB_updateUser(User *user) {
    DB_saveUser(user);
    DB_saveUserIndex();
}


/**
 * ɾ���û�
 * @param userId
 */
void DB_deleteUser(int64 userId) {
    User *user = DB_getUserById(userId);
    if (user == NULL) {
        printf("[deleteUser] �û�������\n");
        return;
    }

    // ����ѧ����ɾ��ѡ�μ�¼
    if (user->role == 0) {
        // ɾ���û���ѡ�μ�¼
        IndexListNode *list = DB_getSelectionsByUserId(user->id);
        for (IndexListNode *p = list; p != NULL; p = p->next) {
            DB_withdrawCourse(user->id, ((CourseSelection *) p->index.data)->courseId);
        }
    }

    // ���ǽ�ʦ��ɾ���γ�
    if (user->role == 1) {
        IndexListNode *list = DB_getCoursesByTeacherId(user->id);
        for (IndexListNode *p = list; p != NULL; p = p->next) {
            DB_deleteCourse(((Course *) p->index.data)->id);
        }
    }

    user_name_Index = AVL_deleteNode(user_name_Index, Hash_String(Wubi_chn2wubi(user->name)));
    user_empId_Index = AVL_deleteNode(user_empId_Index, Hash_String(user->empId));
    user_ID_Index = AVL_deleteNode(user_ID_Index, user->id);
    user_file_Index = AVL_deleteNode(user_file_Index, user->id);
    char *filePath = calloc(100, sizeof(char));
    sprintf(filePath, "data/user/%lld.dat", user->id);
    remove(filePath);
    DB_saveUserIndex();
}


#endif //COURSESYSTEM2023_USERS_H
