//
// Created by vvbbnn00 on 2023/08/28.
//
#ifndef COURSESYSTEM2023_MODELS_H
#define COURSESYSTEM2023_MODELS_H

typedef long long int64;

// 用户类型
typedef struct {
    int64 id;                  // 用户ID
    char name[21],             // 用户姓名
    passwd[65],                // 密码
    empId[21];                // 学工号
    int role;                  // 用户角色 0-学生 1-教师 2-管理员
    char contact[201];         // 联系方式
    int64 lastLoginTime;     // 最后一次登录时间戳
} User;

// 课程类型
typedef struct {
    int64 id;                  // 课程ID/课程号
    char courseName[101],     // 课程名称
    description[501];          // 课程描述
    int64 teacherId;          // 任课老师ID
    User *teacher;             // 任课老师
    int type,                  // 课程类型：0-必修 1-选修 2-公选 3-辅修
    weekStart,                // 开课周
    weekEnd,                  // 结课周
    currentMembers,           // 当前报名人数
    maxMembers;               // 最大可报名人数
    char location[101];        // 上课地点
    double points;             // 学分
    int schedule[7][13];       // 每星期课程安排
} Course;

// 选课类型
typedef struct {
    int64 id;                  // 选课ID
    int64 studentId;          // 学生ID
    User *student;             // 学生信息
    int64 courseId;           // 课程ID
    Course *course;            // 课程信息
    int64 selectionTime;      // 选择该课程的时间
    int64 score;               // 成绩
} CourseSelection;

// 选课交互类型
typedef struct {
    int status;                // 是否可选该门课程
    char lockedReason[100];   // 该门课不能选择的原因
    int64 selectionTime;      // 选择该课程的时间
    Course *course;            // 课程信息
} CourseSelectionInteraction;


// 索引类型定义
typedef enum {
    INDEX_TYPE_INT64,
    INDEX_TYPE_STRING,
    INDEX_TYPE_OBJECT
} IndexType;

// 索引定义
typedef struct {
    int64 hash;
    IndexType type;
    void *data;
    struct IndexListNode *next; // 保留
} Index;

#endif