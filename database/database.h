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


#endif