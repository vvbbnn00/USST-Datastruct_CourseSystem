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
 * 判断该字符是否为可用字符
 * @param c
 * @return 可用则返回1，不可用则返回0
 */
char __inAvailableCharset(char c) {
    const char *available_char = "`~!@#$%^&*()_+-= []{}\\|;:'\",.<>?/";
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;
    for (int i = 0; i < 33; i++) if (c == available_char[i]) return 1;
    return 0;
}

/**
 * 获取用户名，按回车后检查
 * @return
 */
char *Serv_User_getUsername() {
    char *username = malloc(21); // 用户名最长20位
    memset(username, 0, 21);
    int i = 0, t;
    char c;
    while (1) {
        t = _getch();
        if (t == 27) exit(0);
        if (t > 127) continue; // 非ASCII字符，不输入
        c = (char) t; //用 _getch() 函数输入，字符不会显示在屏幕上
//        printf("%c, %d\n", c);
        if (c == '\r' || c == '\n') { //遇到回车，表明密码输入结束
            printf("\n");
            break; //while 循环的出口
        } else if (c == '\b') { //遇到退格，需要删除前一个星号
            if (i <= 0) continue; // 如果长度不足，则不执行退格操作
            printf("\b \b");  //退格，打一个空格，再退格，实质上是用空格覆盖掉星号
            --i;
        } else {
            if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) { // 用户名只允许字母数字组合形式，其他内容不键入
                if (i >= 20) continue; // 限制用户名长度
                username[i++] = c;//将字符放入数组
                printf("%c", c);
            }
        }
    }
    return username;
}


/**
 * 获取密码，密码密文显示
 * @param do_exit 若esc取消，则退出
 * @return
 */
char *Serv_User_getPassword(char do_exit) {
    char *password = malloc(33); //存储密码
    memset(password, 0, 33);
    int i = 0, t; //记录密码长度
    char c; //用于实现密码隐式输入
    while (1) {
        t = _getch();
        if (t == 27) {
            if (do_exit) {
                exit(0);
            } else {
                return "";
            }
        }
        if (t > 127) continue; // 非ASCII字符，不输入
        c = (char) t; //用 _getch() 函数输入，字符不会显示在屏幕上
//        printf("%c, %d\n", c);
        if (c == '\r' || c == '\n') { //遇到回车，表明密码输入结束
            printf("\n");
            break; //while 循环的出口
        } else if (c == '\b') { //遇到退格，需要删除前一个星号
            if (i <= 0) continue; // 如果长度不足，则不执行退格操作
            printf("\b \b");  //退格，打一个空格，再退格，实质上是用空格覆盖掉星号
            --i;
        } else {
            if (__inAvailableCharset(c)) {
                if (i >= 30) continue; // 限制密码长度
                password[i++] = c;//将字符放入数组
                printf("*");
            }
        }
    }
    return password;
}


/**
 * 获取用户角色
 * @return 用户角色
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
 * 执行登录操作
 * @param status 表示本次登录是否额外携带状态
 */
int Serv_User_login(char status) {
    char *username, *password;
    Login: // 设置标签，若密码错误，则返回重新登录
    system("cls");
    system("@echo off");
    system("MODE CON COLS=55 LINES=30");

    UI_printHeader(55);

    switch (status) {
        case 1:
            printf("[系统提示] 您已成功退出登录。\n");
            break;
        default:
            break;
    }
    status = 0; // 状态是一次性的

    printf("\n---- 用户登录 - （按\"esc\"键取消） ----\n\n");
    printf("请输入用户名（按回车键确认）：\n");
    username = Serv_User_getUsername();
    if (strlen(username) == 0) {
        goto Login;
    }
    printf("请输入密码（按回车键确认）：\n");
    password = Serv_User_getPassword(1);
    if (strlen(password) == 0) {
        goto Login;
    }
    char *hashPasswd = calcHexHMACSHA256(password, SECRET_KEY);
    free(password);

    // 调用数据库API，执行登录操作
    User *user = DB_getUserByEmpId(username);
    if (user == NULL) {
        printf("登录失败：用户名或密码错误（按任意键重新登录）。\n");
        _getch();
        goto Login;
    }
    if (strcmp(user->passwd, hashPasswd) != 0) {
        printf("登录失败：用户名或密码错误（按任意键重新登录）。\n");
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
