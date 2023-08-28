//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_COURSES_H
#define COURSESYSTEM2023_COURSES_H

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

IndexListNode *DB_getSelectionsByCourseId(int64 courseId);

User *DB_getUserById(int64 userId);

void DB_saveAutoIncrement();

CourseSelection *DB_withdrawCourse(int64 userId, int64 courseId);


int64 AUTO_INCREMENT_COURSE_ID = 1; // 自增的课程ID

AVLNode *course_ID_Index = NULL;            // 课程ID索引，将课程信息缓存到内存中，每次启动重新构建
AVLNode *course_file_Index = NULL;          // 课程文件索引，从文件中读取
AVLNode *course_name_Index = NULL;          // 课程名称索引，从文件中读取
AVLNode *course_teacherId_Index = NULL;     // 课程教师ID索引，从文件中读取

/**
 * 根据ID获取课程
 * @param courseId
 * @return
 */
Course *DB_getCourseById(int64 courseId) {
    IndexListNode *node = NULL; // AVL_searchExact(course_ID_Index, courseId);
    if (node == NULL) {
        // 若不存在，则从文件中读取
        char *filePath = calloc(100, sizeof(char));
        sprintf(filePath, "data/course/%lld.dat", courseId);
        FILE *fp = fopen(filePath, "rb");
        if (fp == NULL) {
            perror("[getCourseById] 打开课程文件失败");
            return NULL;
        }
        Course *course = calloc(1, sizeof(Course));
        fread(course, sizeof(Course), 1, fp);
        fclose(fp);
        course->teacher = DB_getUserById(course->teacherId);

        IndexListNode *selections = DB_getSelectionsByCourseId(courseId);
        course->currentMembers = 0;
        for (IndexListNode *p = selections; p != NULL; p = p->next) {
            course->currentMembers++;
        }

        // 插入索引
        // course_ID_Index = AVL_insertNode(course_ID_Index, courseId, INDEX_TYPE_OBJECT, course);
        return course;
    }
    return (Course *) node->index.data;
}

/**
 * 根据课程名称获取课程
 * @param courseName
 * @return
 */
NodeList *DB_getCoursesByName(char *courseName) {
    char *preprocessedName = Hash_PreprocessString(Wubi_chn2wubi(courseName));
    int64 hashStart = Hash_String(preprocessedName);
    preprocessedName[strlen(preprocessedName) - 1]++;
    int64 hashEnd = Hash_String(preprocessedName);

    // printf("%lld %lld\n", hashStart, hashEnd);
    // system("pause");

    NodeList *result = NULL;
    AVL_inOrderSearch(course_name_Index, hashStart, hashEnd, &result);
    return result;
}

/**
 * 根据教师ID获取课程
 * @param teacherId
 * @return
 */
IndexListNode *DB_getCoursesByTeacherId(int64 teacherId) {
    return AVL_searchExact(course_teacherId_Index, teacherId)->next;
}

/**
 * 保存课程信息
 * @param course
 */
void DB_saveCourse(Course *course) {
    // 保存课程信息
    char *filePath = calloc(100, sizeof(char));
    sprintf(filePath, "data/course/%lld.dat", course->id);
    FILE *fp = fopen(filePath, "wb");
    if (fp == NULL) {
        perror("[saveCourse] 打开课程文件失败");
        return;
    }
    fwrite(course, sizeof(Course), 1, fp);
    fclose(fp);
}

/**
 * 保存课程索引
 */
void DB_saveCourseIndex() {
    AVL_saveToFile(course_name_Index, "data/index/course_name.avl");
    AVL_saveToFile(course_teacherId_Index, "data/index/course_teacherId.avl");
    AVL_saveToFile(course_file_Index, "data/index/course_file.avl");
    DB_saveAutoIncrement();
}


/**
 * 创建课程
 * @param courseName
 * @param description
 * @param teacherId
 * @param type
 * @param weekStart
 * @param weekEnd
 * @param currentMembers
 * @param maxMembers
 * @param location
 * @param points
 * @param schedule
 * @return
 */
Course *DB_createCourse(char *courseName, char *description, int64 teacherId, int type, int weekStart, int weekEnd,
                        int currentMembers, int maxMembers, char *location, double points, int schedule[7][13]) {
    User *teacher = DB_getUserById(teacherId);
    if (teacher == NULL) {
        printf("[createCourse] 教师不存在\n");
        return NULL;
    }
    if (teacher->role != 1) {
        printf("[createCourse] 该用户不是教师\n");
        return NULL;
    }

    Course *course = calloc(1, sizeof(Course));
    course->id = AUTO_INCREMENT_COURSE_ID++;
    strcpy(course->courseName, courseName);
    strcpy(course->description, description);
    course->teacherId = teacherId;
    course->teacher = teacher;
    course->type = type;
    course->weekStart = weekStart;
    course->weekEnd = weekEnd;
    course->currentMembers = currentMembers;
    course->maxMembers = maxMembers;
    strcpy(course->location, location);
    course->points = points;
    memcpy(course->schedule, schedule, sizeof(int) * 7 * 13);
    DB_saveCourse(course);
    // 插入索引
    int64 hash = Hash_String(Wubi_chn2wubi(courseName));
    course_ID_Index = AVL_insertNode(course_ID_Index, course->id, INDEX_TYPE_OBJECT, course);
    course_name_Index = AVL_insertNode(course_name_Index, hash, INDEX_TYPE_INT64, (void *) course->id);
    course_teacherId_Index = AVL_insertNode(course_teacherId_Index, course->teacherId, INDEX_TYPE_INT64,
                                            (void *) course->id);
    course_file_Index = AVL_insertNode(course_file_Index, course->id, INDEX_TYPE_INT64, (void *) course->id);

    DB_saveCourseIndex();

    return course;
}


/**
 * 更新课程信息
 * @param course
 */
void DB_updateCourse(Course *course) {
    int64 hashOld = Hash_String(Wubi_chn2wubi(DB_getCourseById(course->id)->courseName));
    int64 hashNew = Hash_String(Wubi_chn2wubi(course->courseName));

    course_name_Index = AVL_deleteNode(course_name_Index, hashOld);
    course_name_Index = AVL_insertNode(course_name_Index, hashNew, INDEX_TYPE_INT64,
                                       (void *) course->id);

    // printf("Hash: %lld %lld\n", hashOld, hashNew);

    DB_saveCourse(course);
    DB_saveCourseIndex();
}


/**
 * 删除课程
 * @param courseId
 */
void DB_deleteCourse(int64 courseId) {
    Course *course = DB_getCourseById(courseId);
    if (course == NULL) {
        printf("[deleteCourse] 课程不存在\n");
        return;
    }

    // 删除选课记录
    IndexListNode *list = DB_getSelectionsByCourseId(courseId);
    for (IndexListNode *p = list; p != NULL; p = p->next) {
        DB_withdrawCourse(((CourseSelection *) p->index.data)->studentId, courseId);
    }

    course_ID_Index = AVL_deleteNode(course_ID_Index, courseId);
    course_name_Index = AVL_deleteNode(course_name_Index, Hash_String(DB_getCourseById(courseId)->courseName));
    course_teacherId_Index = AVL_deleteNode(course_teacherId_Index, DB_getCourseById(courseId)->teacherId);
    course_file_Index = AVL_deleteNode(course_file_Index, courseId);

    // 删除课程文件
    char *filePath = calloc(100, sizeof(char));
    sprintf(filePath, "data/course/%lld.dat", courseId);
    remove(filePath);

    // 删除索引
    DB_saveCourseIndex();
}

NodeList *DB_getAllCourses() {
    NodeList *result = NULL;
    AVL_inOrderTraverse(course_file_Index, &result);
    return result;
}

#endif //COURSESYSTEM2023_COURSES_H
