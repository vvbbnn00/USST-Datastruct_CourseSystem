#include <stdio.h>
#include <stdlib.h>
#include "models.h"
#include "utils/avl.h"
#include "utils/wubi.h"
#include "libs/hmacsha256.h"
#include "common.h"
#include "database/database.h"
#include "database/users.h"
#include "services/users.h"
#include <setjmp.h>


void generateTestData() {

    int scheduleList[7][13] = {{1, 1},
                               {0, 0, 0, 1}};

    User *admin = DB_registerUser("管理员", ADMIN_USERNAME, ADMIN_PASSWORD, 2, "11451419191");
    User *stu = DB_registerUser("陆天成", "2135060620", "123456", 0, "11451419191");
    User *teacher = DB_registerUser("老师", "teacher", "123456", 1, "11451419191");

    if (admin == NULL || stu == NULL || teacher == NULL) {
        printf("生成测试数据失败！\n");
        return;
    }

    Course *course1 = DB_createCourse("中国历史",
                                     "这是一门测试课程111",
                                     teacher->id,
                                     1,
                                     1,
                                     10,
                                     0,
                                     10,
                                     "地点1",
                                     1.50,
                                     scheduleList);
    Course *course2 = DB_createCourse("大学英语",
                                     "这是一门测试课程222",
                                     teacher->id,
                                     1,
                                     11,
                                     12,
                                     0,
                                     10,
                                     "地点1",
                                     1.50,
                                     scheduleList);
    Course *course3 = DB_createCourse("高等数学",
                                        "这是一门测试课程333",
                                        teacher->id,
                                        1,
                                        1,
                                        12,
                                        0,
                                        10,
                                        "地点1",
                                        1.50,
                                        scheduleList);
    Course *course4 = DB_createCourse("线性代数",
                                        "这是一门测试课程444",
                                        teacher->id,
                                        1,
                                        1,
                                        12,
                                        0,
                                        10,
                                        "地点1",
                                        1.50,
                                        scheduleList);

    CourseSelection *selection1 = DB_selectCourse(stu->id, course1->id);
    CourseSelection *selection2 = DB_selectCourse(stu->id, course2->id);
    CourseSelection *selection3 = DB_selectCourse(stu->id, course3->id);
    CourseSelection *selection4 = DB_selectCourse(stu->id, course4->id);
}


void printTrees(){
    system("cls & MODE CON COLS=55 LINES=9999");

    printf("user_name_Index:\n");
    AVL_printInOrder(user_name_Index);
    AVL_printPreOrder(user_name_Index);

    printf("user_empId_Index:\n");
    AVL_printInOrder(user_empId_Index);
    AVL_printPreOrder(user_empId_Index);

    printf("user_file_Index:\n");
    AVL_printInOrder(user_file_Index);
    AVL_printPreOrder(user_file_Index);

    printf("course_name_Index:\n");
    AVL_printInOrder(course_name_Index);
    AVL_printPreOrder(course_name_Index);

    printf("course_teacherId_Index:\n");
    AVL_printInOrder(course_teacherId_Index);
    AVL_printPreOrder(course_teacherId_Index);

    printf("course_file_Index:\n");
    AVL_printInOrder(course_file_Index);
    AVL_printPreOrder(course_file_Index);

    printf("selection_userId_Index:\n");
    AVL_printInOrder(selection_userId_Index);
    AVL_printPreOrder(selection_userId_Index);

    printf("selection_courseId_Index:\n");
    AVL_printInOrder(selection_courseId_Index);
    AVL_printPreOrder(selection_courseId_Index);

    printf("selection_userId_courseId_Index:\n");
    AVL_printInOrder(selection_userId_courseId_Index);
    AVL_printPreOrder(selection_userId_courseId_Index);

    printf("selection_file_Index:\n");
    AVL_printInOrder(selection_file_Index);
    AVL_printPreOrder(selection_file_Index);

    system("pause");

    system("MODE CON COLS=55 LINES=30");
}


int main() {
    system("chcp 65001");
    system("chcp 936");
    Wubi_Init();
    DB_Init();
    char status = setjmp(GLOBAL_goto_login); // 全局标记点，用于退出登录或登录过期时跳转

    generateTestData();

     GlobalUser = DB_getUserByEmpId("2135060620");

    printTrees();

//    Serv_User_login(status);
    // 打印主菜单
    UI_printMainMenu(0);

    return 0;
}
