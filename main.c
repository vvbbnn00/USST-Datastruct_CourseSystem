#ifndef COURSESYSTEM2023_MAIN_C
#define COURSESYSTEM2023_MAIN_C

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

/**
 * ���ڲ���AVL���Ĳ��롢ɾ���Ƿ�����
 */
void Main_testAVL(){
    system("cls & MODE CON COLS=55 LINES=9999");

    DB_registerUser("����Ա", ADMIN_USERNAME, ADMIN_PASSWORD, 2, "11451419191");
    DB_registerUser("½���", "2135060620", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("��ʦ", "teacher", ADMIN_PASSWORD, 1, "11451419191");
    DB_registerUser("�����ƶ�", "ex001", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("������", "ex002", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("��ѩ�˰�", "ex003", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("Ұ���ȱ�", "ex004", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("ATRI", "ex005", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("�е��", "ex006", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("籵�����", "ex007", ADMIN_PASSWORD, 0, "0d000721");
    DB_registerUser("�º���", "ex008", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("���ϣ��", "ex009", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("������", "ex010", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("���䷼��", "ex011", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("�깬����", "ex012", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("��������", "ex013", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("Ȫ�˷�", "ex014", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("�ܷ�����", "ex015", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("����������", "ex016", ADMIN_PASSWORD, 0, "11451419191");

    printf("���ɾ��ǰ�Ķ�������\n");
    printf("user_name_Index:\n");
    AVL_printInOrder(user_name_Index);
    AVL_printPreOrder(user_name_Index);

    printf("user_empId_Index:\n");
    AVL_printInOrder(user_empId_Index);
    AVL_printPreOrder(user_empId_Index);

    printf("user_file_Index:\n");
    AVL_printInOrder(user_file_Index);
    AVL_printPreOrder(user_file_Index);

    printf("\n\nִ��ɾ���û�\n\n");

    DB_deleteUser(8);

    printf("���ɾ����Ķ�������\n");
    printf("user_name_Index:\n");
    AVL_printInOrder(user_name_Index);
    AVL_printPreOrder(user_name_Index);

    printf("user_empId_Index:\n");
    AVL_printInOrder(user_empId_Index);
    AVL_printPreOrder(user_empId_Index);

    printf("user_file_Index:\n");
    AVL_printInOrder(user_file_Index);
    AVL_printPreOrder(user_file_Index);

    system("pause");
    system("MODE CON COLS=55 LINES=30");
}


/**
 * ���ɲ�������
 */
void Main_generateTestData() {

    int scheduleList[7][13] = {{1, 1},
                               {0, 0, 0, 1}};

    User *admin = DB_registerUser("����Ա", ADMIN_USERNAME, ADMIN_PASSWORD, 2, "11451419191");
    User *stu = DB_registerUser("½���", "2135060620", ADMIN_PASSWORD, 0, "11451419191");
    User *teacher = DB_registerUser("��ʦ", "teacher", ADMIN_PASSWORD, 1, "11451419191");
    DB_registerUser("�����ƶ�", "ex001", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("������", "ex002", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("��ѩ�˰�", "ex003", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("Ұ���ȱ�", "ex004", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("ATRI", "ex005", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("�е��", "ex006", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("籵�����", "ex007", ADMIN_PASSWORD, 0, "0d000721");
    DB_registerUser("�º���", "ex008", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("���ϣ��", "ex009", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("������", "ex010", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("���䷼��", "ex011", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("�깬����", "ex012", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("��������", "ex013", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("Ȫ�˷�", "ex014", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("�ܷ�����", "ex015", ADMIN_PASSWORD, 0, "11451419191");
    DB_registerUser("����������", "ex016", ADMIN_PASSWORD, 0, "11451419191");


    if (admin == NULL || stu == NULL || teacher == NULL) {
        printf("���ɲ�������ʧ�ܣ�\n");
        return;
    }

    Course *course1 = DB_createCourse("�й���ʷ",
                                      "����һ�Ų��Կγ�111",
                                      teacher->id,
                                      1,
                                      1,
                                      10,
                                      0,
                                      10,
                                      "�ص�1",
                                      1.50,
                                      scheduleList);
    Course *course2 = DB_createCourse("��ѧӢ��",
                                      "����һ�Ų��Կγ�222",
                                      teacher->id,
                                      1,
                                      11,
                                      12,
                                      0,
                                      10,
                                      "�ص�1",
                                      1.50,
                                      scheduleList);
    Course *course3 = DB_createCourse("�ߵ���ѧ",
                                      "����һ�Ų��Կγ�333",
                                      teacher->id,
                                      1,
                                      1,
                                      12,
                                      0,
                                      10,
                                      "�ص�1",
                                      1.50,
                                      scheduleList);
    Course *course4 = DB_createCourse("���Դ���",
                                      "����һ�Ų��Կγ�444",
                                      teacher->id,
                                      1,
                                      1,
                                      12,
                                      0,
                                      10,
                                      "�ص�1",
                                      1.50,
                                      scheduleList);

    CourseSelection *selection1 = DB_selectCourse(stu->id, course1->id);
    CourseSelection *selection2 = DB_selectCourse(stu->id, course2->id);
    CourseSelection *selection3 = DB_selectCourse(stu->id, course3->id);
    CourseSelection *selection4 = DB_selectCourse(stu->id, course4->id);
}


/**
 * ��ӡ������
 */
void Main_printTrees() {
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


/**
 * ������
 * @return
 */
int main() {
    system("chcp 65001");  // ���������Ի����£�����GBK����ǰ��Ҫ������ΪUTF-8�����������
    system("chcp 936");
    system("TITLE �γ̹���ϵͳ - " VERSION);
    Wubi_Init();
    DB_Init();

     Main_testAVL();

    char status = setjmp(GLOBAL_goto_login); // ȫ�ֱ�ǵ㣬�����˳���¼���¼����ʱ��ת

    Main_generateTestData();

    // GlobalUser = DB_getUserByEmpId("2135060620");

    Main_printTrees();

    Serv_User_login(status);
    // ��ӡ���˵�
    UI_printMainMenu(0);

    return 0;
}

#endif //COURSESYSTEM2023_MAIN_C