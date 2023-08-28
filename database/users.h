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

int64 AUTO_INCREMENT_USER_ID = 1;   // 自增的用户ID

AVLNode *user_ID_Index = NULL;      // 用户ID索引，将用户信息缓存到内存中，每次启动重新构建
AVLNode *user_name_Index = NULL;    // 用户姓名索引，从文件中读取
AVLNode *user_empId_Index = NULL;   // 用户工号索引，从文件中读取

void DB_saveAutoIncrement();

/**
 * 根据ID获取用户
 * @param userId
 * @return
 */
User *DB_getUserById(int64 userId) {
    IndexListNode *node = AVL_searchExact(user_ID_Index, userId);
    if (node == NULL) {
        // 若不存在，则从文件中读取
        char *filePath = calloc(100, sizeof(char));
        sprintf(filePath, "data/user/%lld.dat", userId);
        FILE *fp = fopen(filePath, "rb");
        if (fp == NULL) {
            perror("[getUserById] 打开用户文件失败");
            return NULL;
        }
        User *user = calloc(1, sizeof(User));
        fread(user, sizeof(User), 1, fp);
        fclose(fp);
        // 插入索引
        user_ID_Index = AVL_insertNode(user_ID_Index, userId, INDEX_TYPE_OBJECT, user);
        return user;
    }
    return (User *) node->index.data;
}

/**
 * 根据学工号获取用户
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
 * 根据姓名获取用户
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
 * 保存用户
 * @param user
 */
void DB_saveUser(User *user) {
    char *filePath = calloc(100, sizeof(char));
    sprintf(filePath, "data/user/%lld.dat", user->id);
    FILE *fp = fopen(filePath, "wb");
    if (fp == NULL) {
        perror("[saveUser] 打开用户文件失败");
        return;
    }
    fwrite(user, sizeof(User), 1, fp);
    fclose(fp);
}

/**
 * 保存用户索引
 */
void DB_saveUserIndex() {
    AVL_saveToFile(user_name_Index, "data/index/user_name.avl");
    AVL_saveToFile(user_empId_Index, "data/index/user_empId.avl");
    DB_saveAutoIncrement();
}

/**
 * 注册用户
 * @param name 姓名
 * @param empId 学工号
 * @param passwd 密码
 * @param role 角色
 * @param contact 联系方式
 * @return
 */
User *DB_registerUser(char *name, char *empId, char *passwd, int role, char *contact) {
    // 检查empId是否已经存在
    if (DB_getUserByEmpId(empId) != NULL) {
        printf("[registerUser] 该学工号已经被注册\n");
        return NULL;
    }

    // 继续注册
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
    user_name_Index = AVL_insertNode(user_name_Index, Hash_String(Wubi_chn2wubi(name)), INDEX_TYPE_INT64,
                                     (void *) user->id);
    user_empId_Index = AVL_insertNode(user_empId_Index, Hash_String(empId), INDEX_TYPE_INT64, (void *) user->id);
    user_ID_Index = AVL_insertNode(user_ID_Index, user->id, INDEX_TYPE_OBJECT, user);
    DB_saveUserIndex();
    return user;
}


/**
 * 更新用户信息
 * @param user
 */
void DB_updateUser(User *user) {
    DB_saveUser(user);
    DB_saveUserIndex();
}

#endif //COURSESYSTEM2023_USERS_H
