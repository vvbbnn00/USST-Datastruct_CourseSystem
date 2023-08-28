//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_USERS_SERVICE_H
#define COURSESYSTEM2023_USERS_SERVICE_H

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "../models.h"
#include "../common.h"
#include "../utils/string_ext.h"
#include "../database/users.h"
#include "../libs/hmacsha256.h"
#include "ui.h"

User *GlobalUser = NULL;

/**
 * �жϸ��ַ��Ƿ�Ϊ�����ַ�
 * @param c
 * @return �����򷵻�1���������򷵻�0
 */
char __inAvailableCharset(char c) {
    const char *available_char = "`~!@#$%^&*()_+-= []{}\\|;:'\",.<>?/";
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;
    for (int i = 0; i < 33; i++) if (c == available_char[i]) return 1;
    return 0;
}

/**
 * ��ȡ�û��������س�����
 * @return
 */
char *Serv_User_getUsername() {
    char *username = malloc(21); // �û����20λ
    memset(username, 0, 21);
    int i = 0, t;
    char c;
    while (1) {
        t = _getch();
        if (t == 27) exit(0);
        if (t > 127) continue; // ��ASCII�ַ���������
        c = (char) t; //�� _getch() �������룬�ַ�������ʾ����Ļ��
//        printf("%c, %d\n", c);
        if (c == '\r' || c == '\n') { //�����س������������������
            printf("\n");
            break; //while ѭ���ĳ���
        } else if (c == '\b') { //�����˸���Ҫɾ��ǰһ���Ǻ�
            if (i <= 0) continue; // ������Ȳ��㣬��ִ���˸����
            printf("\b \b");  //�˸񣬴�һ���ո����˸�ʵ�������ÿո񸲸ǵ��Ǻ�
            --i;
        } else {
            if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) { // �û���ֻ������ĸ���������ʽ���������ݲ�����
                if (i >= 20) continue; // �����û�������
                username[i++] = c;//���ַ���������
                printf("%c", c);
            }
        }
    }
    return username;
}


/**
 * ��ȡ���룬����������ʾ
 * @param do_exit ��escȡ�������˳�
 * @return
 */
char *Serv_User_getPassword(char do_exit) {
    char *password = malloc(33); //�洢����
    memset(password, 0, 33);
    int i = 0, t; //��¼���볤��
    char c; //����ʵ��������ʽ����
    while (1) {
        t = _getch();
        if (t == 27) {
            if (do_exit) {
                exit(0);
            } else {
                return "";
            }
        }
        if (t > 127) continue; // ��ASCII�ַ���������
        c = (char) t; //�� _getch() �������룬�ַ�������ʾ����Ļ��
//        printf("%c, %d\n", c);
        if (c == '\r' || c == '\n') { //�����س������������������
            printf("\n");
            break; //while ѭ���ĳ���
        } else if (c == '\b') { //�����˸���Ҫɾ��ǰһ���Ǻ�
            if (i <= 0) continue; // ������Ȳ��㣬��ִ���˸����
            printf("\b \b");  //�˸񣬴�һ���ո����˸�ʵ�������ÿո񸲸ǵ��Ǻ�
            --i;
        } else {
            if (__inAvailableCharset(c)) {
                if (i >= 30) continue; // �������볤��
                password[i++] = c;//���ַ���������
                printf("*");
            }
        }
    }
    return password;
}


/**
 * ��ȡ�û���ɫ
 * @return �û���ɫ
 */
char *Serv_User_getUserRole(int role) {
    switch (role) {
        case 0:
            return WORDS_Login_role_student;
        case 1 :
            return WORDS_Login_role_teacher;
        case 2:
            return WORDS_Login_role_admin;
        default:
            return WORDS_Login_role_unknown;
    }
}


/**
 * ִ�е�¼����
 * @param status ��ʾ���ε�¼�Ƿ����Я��״̬
 */
int Serv_User_login(char status) {
    char *username, *password;
    Login: // ���ñ�ǩ������������򷵻����µ�¼
    system("cls");
    system("@echo off");
    system("MODE CON COLS=55 LINES=30");

    UI_printHeader(55);

    switch (status) {
        case 1:
            printf("[ϵͳ��ʾ] ���ѳɹ��˳���¼��\n");
            break;
        default:
            break;
    }
    status = 0; // ״̬��һ���Ե�

    printf("\n---- �û���¼ - ����\"esc\"��ȡ���� ----\n\n");
    printf("�������û��������س���ȷ�ϣ���\n");
    username = Serv_User_getUsername();
    if (strlen(username) == 0) {
        goto Login;
    }
    printf("���������루���س���ȷ�ϣ���\n");
    password = Serv_User_getPassword(1);
    if (strlen(password) == 0) {
        goto Login;
    }
    char *hashPasswd = calcHexHMACSHA256(password, SECRET_KEY);
    free(password);

    // �������ݿ�API��ִ�е�¼����
    User *user = DB_getUserByEmpId(username);
    if (user == NULL) {
        printf("��¼ʧ�ܣ��û�����������󣨰���������µ�¼����\n");
        _getch();
        goto Login;
    }
    if (strcmp(user->passwd, hashPasswd) != 0) {
        printf("��¼ʧ�ܣ��û�����������󣨰���������µ�¼����\n");
        _getch();
        goto Login;
    }

    free(username);
    free(hashPasswd);

    GlobalUser = user;

    system("cls");
    UI_printHeader(55);
    printf(WORDS_Login_success, GlobalUser->empId, GlobalUser->name,
           Serv_User_getUserRole(GlobalUser->role), getFormatTimeString(GlobalUser->lastLoginTime), GlobalUser->name);

    GlobalUser->lastLoginTime = (int64) time(NULL);
    DB_updateUser(GlobalUser);

    _getch();
    return 0;
}


#endif //COURSESYSTEM2023_USERS_SERVICE_H
