//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_UI_H
#define COURSESYSTEM2023_UI_H

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include <string.h>
#include <Windows.h>
#include "pcre2.h"
#include "../common.h"
#include "../utils/string_ext.h"

User *GlobalUser;

char *Serv_User_getUserRole(int role);

void Serv_User_changePassword();

void Serv_User_printAllUsers();

void Serv_Course_printStudentCourseSelection();

void Serv_Course_printAllCourses(int scene);

void Serv_Course_printStudentLectureTable();

void Serv_Course_printStudentScoreTable();

/**
 * ���������Ϣ
 * @param msg
 */
void UI_printErrorData(char *msg) {
    printf("[ϵͳ����] %s���������������\n", msg);
    getch();
}


/**
 * ���������Ϣ
 * @param msg ��Ϣ
 * @param width ���
 */
void UI_printInMiddle(char *msg, int width, ...) {
    unsigned int str_length = (unsigned int) strlen(msg);
    char *final_str = calloc(str_length + 1000, sizeof(char));
    // �Ƚ���ʽ���Ľ��������ַ�����
    va_list args;
    va_start(args, width);
    vsnprintf(final_str, str_length + 1000, msg, args);
    va_end(args);
    // ����������Ҫռ�õĳ���
    str_length = strlen(final_str);
    if (width - (int) str_length > 0) {
        for (int i = 0; (i < (width - str_length) / 2); i++) printf(" ");
    }
    printf("%s\n", final_str);
}


/**
 * ������ʽ�������ݲ���֤
 * @param message ��ʾ��Ϣ
 * @param pattern ������ʽ
 * @param _dest Ŀ���ַ���
 * @param max_length �����볤��
 *
 * @return 0 ���� -1 �û�ȡ��
 */
int UI_inputStringWithRegexCheck(char *message, char *pattern, char *_dest, int max_length) {
    char *input_string;
    printf("%s", message);

    InputString:
    printf("[����������(Escȡ��)] ");
    input_string = calloc(max_length + 1, sizeof(char));
    int str_len = 0, t; //��¼����
    while (1) {
        t = _getch();
        if (t == 27) {
            return -1; // �û��˳�
        }
        if (t == '\r' || t == '\n') { //�����س��������������
            printf("\n");
            break; //while ѭ���ĳ���
        } else if (t == '\b') { //�����˸���Ҫɾ��ǰһ���Ǻ�
            if (str_len <= 0) continue; // ������Ȳ��㣬��ִ���˸����
            printf("\b \b");  //�˸񣬴�һ���ո����˸�ʵ�������ÿո񸲸ǵ��Ǻ�
            --str_len;
        } else {
            if (str_len >= max_length) continue; // �������볤��
            input_string[str_len++] = (char) t;//���ַ���������
            printf("%c", t);
        }
    }

    if (regexMatch(pattern, input_string) == 0) {
        safe_free((void **) &input_string);
        printf("��������ݲ����Ϲ�������������\n");
        goto InputString;
    }

    strcpy(_dest, input_string);
    safe_free((void **) &input_string);
    return 0;
}


/**
 * ������ʽ�������ݲ���֤
 * @param message ��ʾ��Ϣ
 * @param pattern ������ʽ
 * @param _dest Ŀ���ַ���
 * @param max_length �����볤��
 *
 * @return 0 ���� -1 �û�ȡ��
 */
int UI_inputIntWithRegexCheck(char *message, char *pattern, int *_dest) {
    char *input_string;
    InputInteger:
    input_string = calloc(12, sizeof(char));
    if (UI_inputStringWithRegexCheck(message, pattern, input_string, 11) == -1) {
        return -1;
    }
    char *end_ptr;
    int result = strtol(input_string, &end_ptr, 10);
    if (*end_ptr != 0) {
        printf("��������ȷ������\n");
        safe_free((void **) &input_string);
        goto InputInteger;
    }
    *_dest = result;
    return 0;
}

/**
 * ������ʽ�������ݲ���֤��С����
 * @param message ��ʾ��Ϣ
 * @param pattern ������ʽ
 * @param _dest Ŀ���ַ���
 * @param max_length �����볤��
 *
 * @return 0 ���� -1 �û�ȡ��
 */
int UI_inputFloatWithRegexCheck(char *message, char *pattern, double *_dest) {
    char *input_string;
    InputFloat:
    input_string = calloc(12, sizeof(char));
    if (UI_inputStringWithRegexCheck(message, pattern, input_string, 11) == -1) {
        return -1;
    }
    char *end_ptr;
    double result = strtof(input_string, &end_ptr);
    if (*end_ptr != 0) {
        printf("��������ȷ������\n");
        safe_free((void **) &input_string);
        goto InputFloat;
    }
    *_dest = result;
    return 0;
}

/**
 * �����������
 *
 * @param format
 * @param counter
 * @param selected ��Ҫ��������
 * @param args
 */
void UI_selfPlusPrint(char *format, int *counter, int selected, ...) {
    HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (windowHandle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error: Invalid handle value.");
        return;
    }

    if (*counter == selected) {
        SetConsoleTextAttribute(windowHandle, 0x70);
    }

    // ���̶�����ͨ��va_list���棬ʹ��vprintf������ɸ��õط�װprint����
    va_list args;
    va_start(args, selected);
    int ret = vprintf(format, args);
    va_end(args);

    if (ret < 0) {
        fprintf(stderr, "Error: vprintf failed.");
    }

    SetConsoleTextAttribute(windowHandle, 0x07);

    (*counter)++;
}


/**
 * UIģ�飬��ӡ�������ơ��汾�ŵ���Ϣ
 */
void UI_printHeader(int width) {
    UI_printInMiddle("----------------------------", width);
    UI_printInMiddle("|        �γ���Ϣϵͳ        |", width);
    UI_printInMiddle("|   %s   |", width, VERSION);
    UI_printInMiddle("----------------------------", width);
}

/**
 * UIģ�飬��ӡ�û���Ϣ
 */
void UI_printUserInfo() {
    printf("\n-------- �û���Ϣ --------\n\n - �û����� %s\n - ��  ���� %s\n - ��  ɫ�� %s\n\n",
           GlobalUser->empId,
           GlobalUser->name,
           Serv_User_getUserRole(GlobalUser->role));
}


/**
 * ������������������ʽ����˵�ѡ��
 * @param msg
 * @param current
 */
void UI_printChoices(char *msg, int *current) {
    char fmt[50] = "%-30s";
    sprintf(fmt, "%%-%ds", 30);
    printf(fmt, msg);
    if (*current % 2 == 0) {
        printf("\n");
    }
    (*current)++;
}


/**
 * �Դ����ָ����в���
 * @param command
 */
void UI_doMainMenuActions(int command) {
    switch (command) {
        // �޸�����
        case 5:
        case 13:
        case 23:
            Serv_User_changePassword();
            break;
            // �˳���¼
        case 6:
        case 14:
        case 24:
            GlobalUser = NULL;
            longjmp(GLOBAL_goto_login, 1);
            // �˳�����
        case 7:
        case 15:
        case 25:
            exit(0);
        case 1:
            Serv_Course_printStudentCourseSelection();
            break;
        case 2:
            Serv_Course_printStudentLectureTable();
            break;
        case 3:
        case 12:
        case 21:
            Serv_Course_printAllCourses(0);
            break;
        case 4:
            Serv_Course_printStudentScoreTable();
            break;
        case 11:
            Serv_Course_printAllCourses(1);
            break;
        case 22:
            Serv_User_printAllUsers();
            break;
        default:
            break;
    }
}


/**
 * ����Ȩ��������˵�ģ��
 * @param wrong_command �Ƿ�����ǰ���������ָ��
 */
int UI_printMainMenu(char wrong_command) {
    init:
    system("MODE CON COLS=55 LINES=30 & cls");
    UI_printHeader(55);
    UI_printUserInfo();
    printf("============= ���˵� - �����ָ�� =============\n\n");
    int INDEX = 1;
    switch (GlobalUser->role) {
        case 0: // ѧ��
            UI_printChoices("(1) �γ�ѧϰ����", &INDEX);
            UI_printChoices("(2) �鿴��ǰ�α�", &INDEX);
            UI_printChoices("(3) ȫУ�γ���Ϣһ��", &INDEX);
            UI_printChoices("(4) ��ѯ�γ̳ɼ�", &INDEX);
            UI_printChoices("(5) �޸�����", &INDEX);
            UI_printChoices("(6) �˳���¼", &INDEX);
            UI_printChoices("(7) �˳�����", &INDEX);
            break;
        case 1: // ��ʦ
            UI_printChoices("(1) �ڿογ���Ϣ����", &INDEX);
            UI_printChoices("(2) ȫУ�γ���Ϣһ��", &INDEX);
            UI_printChoices("(3) �޸�����", &INDEX);
            UI_printChoices("(4) �˳���¼", &INDEX);
            UI_printChoices("(5) �˳�����", &INDEX);
            break;
        case 2: // ����Ա
            UI_printChoices("(1) �γ���Ϣ����", &INDEX);
            UI_printChoices("(2) �û�����", &INDEX);
            UI_printChoices("(3) �޸�����", &INDEX);
            UI_printChoices("(4) �˳���¼", &INDEX);
            UI_printChoices("(5) �˳�����", &INDEX);
            break;
        default:
            longjmp(GLOBAL_goto_login, 2);
    }
    if (INDEX % 2 == 0) printf("\n");
    printf("\n===============================================\n");
    if (wrong_command) {
        printf("�������ָ���������������룺");
        wrong_command = 0;
    } else { printf("�����������ţ�"); }
    char command_raw[11] = {0};
    fgets(command_raw, 10, stdin);
    fflush(stdin); // ����������Чָ���ֹӰ���������
    if (strlen(command_raw) != 2) {
        wrong_command = 1;
        goto init;
    }
    int command = (int) command_raw[0];
    if (command >= '0' && command <= '9') {
        command += GlobalUser->role * 10 - '0';
        UI_doMainMenuActions(command);
        goto init;
    } else {
        wrong_command = 1;
        goto init;
    }
}


/**
 * ���ļ��Ի���
 *
 * @param path ���շ���·��
 * @param fileType �ļ����ͣ���\0�ָ���磺All *.* Text *.TXT
 * @return 1 - �ɹ� 0 - ʧ��
 */
int UI_openFileDialog(char *path, const char *fileType, char *title) {
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = fileType;
    ofn.lpstrFileTitle = title;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    return GetOpenFileName(&ofn);
}


#include "users.h"
#include "course.h"

#endif //COURSESYSTEM2023_UI_H
