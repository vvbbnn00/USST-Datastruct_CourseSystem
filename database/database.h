//
// Created by vvbbnn00 on 2023/08/28.
//
#ifndef COURSESYSTEM2023_DATABASE_H
#define COURSESYSTEM2023_DATABASE_H

#include "../models.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include "../common.h"
#include "../utils/avl.h"
#include "../utils/hash.h"
#include "../utils/wubi.h"
#include "users.h"
#include "courses.h"
#include "selections.h"


AVLNode *global_counter_Index = NULL; // 全局计数器索引，计算Index的数量，从文件中读取


/**
 * 初始化数据库
 */
void DB_Init() {
    // 检查文件夹是否存在，若不存在则创建
    if (access("data", 0) == -1) {
        mkdir("data");
    }
    if (access("data/index", 0) == -1) {
        mkdir("data/index");
    }
    if (access("data/user", 0) == -1) {
        mkdir("data/user");
    }
    if (access("data/course", 0) == -1) {
        mkdir("data/course");
    }
    if (access("data/selection", 0) == -1) {
        mkdir("data/selection");
    }

    if (access("data/index/auto_increment.dat", 0) == -1) {
        FILE *fp = fopen("data/index/auto_increment.dat", "wb");
        fwrite(&AUTO_INCREMENT_USER_ID, sizeof(int64), 1, fp);
        fwrite(&AUTO_INCREMENT_COURSE_ID, sizeof(int64), 1, fp);
        fwrite(&AUTO_INCREMENT_SELECTION_ID, sizeof(int64), 1, fp);
        fclose(fp);
    } else {
        FILE *fp = fopen("data/index/auto_increment.dat", "rb");
        fread(&AUTO_INCREMENT_USER_ID, sizeof(int64), 1, fp);
        fread(&AUTO_INCREMENT_COURSE_ID, sizeof(int64), 1, fp);
        fread(&AUTO_INCREMENT_SELECTION_ID, sizeof(int64), 1, fp);
        fclose(fp);
    }

    // 加载用户索引
    user_name_Index = AVL_loadFromFile("data/index/user_name.avl");
    user_empId_Index = AVL_loadFromFile("data/index/user_empId.avl");
    user_file_Index = AVL_loadFromFile("data/index/user_file.avl");

    // 加载课程索引
    course_name_Index = AVL_loadFromFile("data/index/course_name.avl");
    course_teacherId_Index = AVL_loadFromFile("data/index/course_teacherId.avl");
    course_file_Index = AVL_loadFromFile("data/index/course_file.avl");

    // 加载选课索引
    selection_userId_Index = AVL_loadFromFile("data/index/selection_userId.avl");
    selection_courseId_Index = AVL_loadFromFile("data/index/selection_courseId.avl");
    selection_userId_courseId_Index = AVL_loadFromFile("data/index/selection_userId_courseId.avl");
    selection_file_Index = AVL_loadFromFile("data/index/selection_file.avl");

    // 加载计数器索引
    global_counter_Index = AVL_loadFromFile("data/index/global_counter.avl");
}

/**
 * 保存自增ID信息
 */
void DB_saveAutoIncrement() {
    FILE *fp = fopen("data/index/auto_increment.dat", "wb");
    fwrite(&AUTO_INCREMENT_USER_ID, sizeof(int64), 1, fp);
    fwrite(&AUTO_INCREMENT_COURSE_ID, sizeof(int64), 1, fp);
    fwrite(&AUTO_INCREMENT_SELECTION_ID, sizeof(int64), 1, fp);
    fclose(fp);
}

/**
 * 获取计数
 *
 * @param indexName Index名称，最大不超过12个字符
 * @return
 */
int64 DB_getCountByIndex(char *indexName) {
    IndexListNode *node = AVL_searchExact(global_counter_Index, Hash_String(indexName));
    if (node == NULL) {
        return -1;
    }
    return (int64) node->index.data;
}


/**
 * 保存计数器索引
 */
void DB_saveGlobalCounterIndex() {
    AVL_saveToFile(global_counter_Index, "data/index/global_counter.avl");
}


/**
 * 设置计数
 *
 * @param indexName Index名称，最大不超过12个字符
 * @param count 计数
 */
void DB_setCountByIndex(char *indexName, int64 count) {
    if (count < 0) {
        printf("[setCountByIndex] count不能小于0\n");
        return;
    }

    IndexListNode *node = AVL_searchExact(global_counter_Index, Hash_String(indexName));
    if (node == NULL) {
        global_counter_Index = AVL_insertNode(global_counter_Index, Hash_String(indexName), INDEX_TYPE_INT64, count);
    } else {
        node->index.data = (void *) count;
    }
    DB_saveGlobalCounterIndex();
}


#endif