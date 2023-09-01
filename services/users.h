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
    char *username = calloc(21, sizeof(char)); // 用户名最长20位
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
    char *password = calloc(33, sizeof(char)); //存储密码
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
    system("TITLE 课程管理系统 - 登录");

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
    safe_free(&password);

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

    safe_free(&username);
    safe_free(&hashPasswd);

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


/**
 * 执行修改密码操作
 */
void Serv_changePassword() {
    char *ori_password, *new_password, *new_password_repeat;

    Init:

    system("cls");
    system("@echo off");
    system("MODE CON COLS=55 LINES=30");

    UI_printHeader(55);

    printf("\n---- 修改密码 - （按\"esc\"键或不输入内容以返回） ----\n\n");
    printf("用户名：%s\n", GlobalUser->empId);
    printf("请输入原密码（按\"esc\"键或不输入内容以返回）：\n");
    ori_password = Serv_User_getPassword(0);
    if (strlen(ori_password) == 0) {
        return;
    }
    // 设置标记点，若密码不符合规则，则跳回此处
    EnterNewPassword:
    printf("请输入新密码：\n");
    printf("[密码强度提示] 密码中至少同时出现字母、数字、字符中的两种符号，长度在8-20位。\n");
    new_password = Serv_User_getPassword(0);
    if (strlen(new_password) == 0) {
        return;
    }
    if (!regexMatch(PASSWD_PATTERN, new_password)) {
        printf("密码强度不符合规则，请重新输入。\n\n");
        goto EnterNewPassword;
    }
    printf("请再次输入新密码：\n");
    new_password_repeat = Serv_User_getPassword(0);
    if (strcmp(new_password_repeat, new_password) != 0) {
        printf("两次密码输入不相同，请重新输入。\n\n");
        goto EnterNewPassword;
    }

    char *hashPasswd = calcHexHMACSHA256(ori_password, SECRET_KEY);
    char *hashNewPasswd = calcHexHMACSHA256(new_password, SECRET_KEY);
    safe_free(&ori_password);
    safe_free(&new_password);
    safe_free(&new_password_repeat);
    if (strcmp(hashPasswd, GlobalUser->passwd) != 0) {
        printf("[系统提示] 修改密码失败：原密码不正确（按任意键重新输入）\n");
        _getch();
        goto Init;
    }

    if (strcmp(hashPasswd, hashNewPasswd) == 0) {
        printf("[系统提示] 修改密码失败：新密码与原密码相同（按任意键重新输入）\n");
        _getch();
        goto Init;
    }

    safe_free(&hashPasswd);
    strcpy(GlobalUser->passwd, hashNewPasswd);
    DB_updateUser(GlobalUser);

    printf("[系统提示] 密码修改成功（按任意键返回主菜单）。\n");
    _getch();
}


/**
 * 新增用户
 * @return 取消则返回NULL，否则返回用户指针
 */
User *addUser() {
    system("chcp 936>nul & cls & MODE CON COLS=75 LINES=55");
    printf("\n");
    UI_printHeader(69);
    printf("\n");
    UI_printInMiddle("======= 用户·新增用户 =======\n", 71);

    User *user = calloc(1, sizeof(User));
    if (UI_inputStringWithRegexCheck("[新增用户] 请输入用户学号（由3-15位的字母、数字组成）\n",
                                     USER_PATTERN,
                                     user->empId,
                                     15) == -1)
        goto CancelAdd;
    if (UI_inputStringWithRegexCheck(
            "[新增用户] 请设置用户初始密码（由8-20为字母、数字、字符组成，三种类型至少同时出现两种）\n",
            PASSWD_PATTERN,
            user->passwd,
            20) == -1)
        goto CancelAdd;
    if (UI_inputStringWithRegexCheck("[新增用户] 请设置用户姓名（由2-20位的中文、英文等组成）\n",
                                     USER_NAME_PATTERN,
                                     user->name,
                                     20) == -1)
        goto CancelAdd;
    if (UI_inputStringWithRegexCheck("[新增用户] 请设置联系方式（由2-20位的中文、英文等组成）\n",
                                     USER_NAME_PATTERN,
                                     user->contact,
                                     20) == -1)
        goto CancelAdd;
    strcpy(user->passwd, calcHexHMACSHA256(user->passwd, SECRET_KEY));
    if (UI_inputIntWithRegexCheck("[新增用户] 请设置用户角色：0-学生、1-教师、2-管理员\n",
                                  USER_ROLE_PATTERN,
                                  &user->role) == -1)
        goto CancelAdd;
    goto Return;

    CancelAdd:  // 取消添加
    safe_free(&user);
    user = NULL;

    Return:  // 返回数据
    return user;
}


/**
 * 新增/修改用户
 *
 * @param _user 当为NULL时，新增用户，否则为修改用户
 * @return
 */
void editUser(User *_user) {
    // 如无用户信息，则新增用户
    int action = 0;
    User *user = _user;

    if (user == NULL) {
        user = addUser();
        action = 1;
        if (user == NULL) {
            return;
        }
    }

    int counter = 0, selected = 0, key;

    EditUser_Refresh:

    counter = 0;
    system("chcp 936>nul & cls & MODE CON COLS=75 LINES=55");
    printf("\n");
    UI_printHeader(69);
    printf("\n");
    UI_printInMiddle("======= 用户·新增/修改用户 =======\n\n", 71);

    UI_selfPlusPrint("\t\t用户UID       %lld\n", &counter, selected, user->id); // 0
    UI_selfPlusPrint("\t\t用户学号      %s\n", &counter, selected, user->empId); // 1
    UI_selfPlusPrint("\t\t用户姓名      %s\n", &counter, selected, user->name); // 2
    UI_selfPlusPrint("\t\t用户角色      %s\n", &counter, selected, Serv_User_getUserRole(user->role)); // 3
    UI_selfPlusPrint("\t\t最后登录时间  %s\n", &counter, selected, getFormatTimeString(user->lastLoginTime)); // 4
    UI_selfPlusPrint("\t\t用户密码      [可重置]\n", &counter, selected); // 5
    UI_selfPlusPrint("\t\t联系方式      %s\n", &counter, selected, user->contact); // 6

    printf("\n");
    UI_printInMiddle("<Enter>修改选中行 <Y>提交修改 <Esc>取消修改", 71);

    EditCourse_GetKey:

    key = _getch();
    switch (key) {
        case 224:
            key = _getch();
            switch (key) {
                case 80: // 下
                    if (selected < 6) selected++;
                    else selected = 0;
                    goto EditUser_Refresh;
                case 72: // 上
                    if (selected > 0) selected--;
                    else selected = 6;
                    goto EditUser_Refresh;
                default:
                    break;
            }
            break;
        case 13:
            switch (selected) {
                case 2:
                    UI_inputStringWithRegexCheck("[修改用户] 请设置用户姓名（由2-20位的中文、英文等组成）\n",
                                                 USER_NAME_PATTERN,
                                                 user->name,
                                                 20);
                    break;
                case 3:
                    UI_inputIntWithRegexCheck("[修改用户] 请设置用户角色：0-学生、1-教师、2-管理员\n",
                                              USER_ROLE_PATTERN,
                                              &user->role);
                    break;
                case 5:
                    UI_inputStringWithRegexCheck(
                            "[修改用户] 请设置用户密码（由8-20为字母、数字、字符组成，三种类型至少同时出现两种）\n",
                            PASSWD_PATTERN,
                            user->passwd,
                            20);
                    strcpy(user->passwd, calcHexHMACSHA256(user->passwd, SECRET_KEY));
                    break;
                case 6:
                    UI_inputStringWithRegexCheck(
                            "[修改用户] 请设置用户联系方式（由8-20为字母、数字、字符组成，三种类型至少同时出现两种）\n",
                            USER_NAME_PATTERN,
                            user->contact,
                            20);
                    break;
                default:
                    break;
            }
            goto EditUser_Refresh;
        case 'y':
        case 'Y': {
            printf("\n[提示] 正在请求...\n");

            if (action == 1) {
                User *ret = DB_registerUser(user->name, user->empId, user->passwd, user->role, user->contact);
                if (ret == NULL) {
                    printf("[提交失败] 用户新增失败（按任意键返回编辑）\n");
                    getch();
                    goto EditUser_Refresh;
                }
            } else {
                char ret = DB_updateUser(user);
                if (ret == 0) {
                    printf("[提交失败] 用户修改失败（按任意键返回编辑）\n");
                    getch();
                    goto EditUser_Refresh;
                }
            }

            printf("[提交成功] 用户修改/新增成功（按任意键返回用户列表）\n");
            getch();
            goto GC_COLLECT;
        }
            break;
        case 27:
            goto GC_COLLECT;
        default:
            break;
    }
    goto EditCourse_GetKey;

    GC_COLLECT:
    if (_user == NULL) safe_free(&user);
}


/**
 * 输出全体用户信息
 */
void printAllUsers() {
    HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    char search_kw[36] = "";
    int total, page = 1, max_page, page_size = 15, current_total, cnt = 0;

    LinkList_Node *selectedRow = NULL; // 被选中的行
    LinkList_Object *user_data_list = NULL;

    User_GetAndDisplay:

    total = 0;

    system("chcp 936>nul & cls & MODE CON COLS=130 LINES=50");
    // ... 请求过程
    printf("[提示] 正在请求...\n");

    NodeList *result;
    if (strlen(search_kw) > 0) {
        result = DB_getUsersByName(search_kw);
    } else {
        result = DB_getAllUsers();
    }
    for (NodeList *pt = result; pt != NULL; pt = pt->next) {
        total++;
    }

    max_page = (total / page_size) + ((total % page_size) ? 1 : 0);
    if (page > max_page) page = max_page;
    current_total = (page == max_page) ? (total % page_size) : page_size;

    user_data_list = linkListObject_Init();

    cnt = 0;
    for (NodeList *pt = result; pt != NULL; pt = pt->next) {
        if (cnt >= (page - 1) * page_size && cnt < page * page_size) {
            IndexListNode *node = pt->indexNode;
            int64 id = (int64) node->index.data;
            User *user = DB_getUserById(id);
            User *userInsert = memcpy(calloc(1, sizeof(User)), user, sizeof(User));
            if (user == NULL) continue;
            linkListObject_Append(user_data_list, userInsert);
        }
        cnt++;
    }

    if (user_data_list->head != NULL) selectedRow = user_data_list->head;
    else selectedRow = NULL;

    User_Refresh:

    system("cls");
    printf("\n");
    UI_printHeader(92);
    printf("\n");

    printf("\n  ");
    printf(" <A>新建用户 <Enter>编辑用户 <D>删除用户 <K>用户模糊查询");
    printf(" <Esc>返回主菜单\n\n");

    UI_printInMiddle("======= 用户·用户管理 =======\n", 95);
    printf("%-10s%-20s%-17s%-8s%-25s%-20s\n", "用户UID", "姓名", "学工号", "角色", "最后登录时间", "联系方式");
    printf("------------------------------------------------------------------------------------------------------------\n");
    for (LinkList_Node *pt = user_data_list->head; pt != NULL; pt = pt->next) {
        User *tmp = pt->data;
        if (pt == selectedRow) SetConsoleTextAttribute(windowHandle, 0x70);
        printf("%-10lld%-20s%-17s%-8s%-25s%-20s\n",
               tmp->id,
               tmp->name,
               tmp->empId,
               Serv_User_getUserRole(tmp->role),
               getFormatTimeString(tmp->lastLoginTime),
               tmp->contact);
        SetConsoleTextAttribute(windowHandle, 0x07);
        if (pt->next == NULL) { // 链表的最后一个节点
            printf("\n");
        }
    }
    for (int i = 0; i < page_size - current_total; i++) printf("\n"); // 补齐页面
    printf("\n");
    UI_printInMiddle("=============================\n", 89);

    if (selectedRow == NULL) {
        if (strlen(search_kw)) {
            UI_printErrorData("没有查询到符合条件的用户");
            strcpy(search_kw, "");
            goto User_GetAndDisplay;
        } else {
            UI_printErrorData("暂无用户（这是不正常的，你是如何登录进来的？）");
            goto GC_Collect;
        }
    }

    printf("\n[当前搜索条件] ");
    if (strlen(search_kw) > 0) { printf("模糊搜索=%s\n", search_kw); } else { printf("\n"); }
    printf("[提示] 共%4d条数据，当前第%3d页，共%3d页（左方向键：前一页；右方向键：后一页；上/下方向键：切换选中数据）\n",
           total, page, max_page);
    printf("\n");


    int keyboard_press;

    GetKey:
    keyboard_press = _getch();
    switch (keyboard_press) {
        case 224:
            keyboard_press = _getch();
            switch (keyboard_press) {
                case 80: // 下
                    if (selectedRow->next == NULL) selectedRow = user_data_list->head;
                    else selectedRow = selectedRow->next;
                    goto User_Refresh;
                case 72: // 上
                    if (selectedRow->prev == NULL) selectedRow = user_data_list->foot;
                    else selectedRow = selectedRow->prev;
                    goto User_Refresh;
                case 75: // 左
                    page = (page > 1) ? (page - 1) : 1;
                    selectedRow = 0;
                    goto User_GetAndDisplay;
                case 77: // 右
                    page = (page < max_page) ? (page + 1) : (max_page);
                    selectedRow = 0;
                    goto User_GetAndDisplay;
                default:
                    break;
            }
            break;
        case 'k': // 搜索关键词
        case 'K':
            printf("\n[模糊搜索] 请输入关键词（以\"Enter\"键结束）：");
            gets_safe(search_kw, 35);
            fflush(stdin);
            selectedRow = 0;
            goto User_GetAndDisplay;
        case 'A':
        case 'a': // 新建用户
            editUser(NULL);
            goto User_GetAndDisplay;
        case 'D':
        case 'd': // 删除用户
        {
            User *pt = selectedRow->data;
            printf("\n[确认删除] 您确定要删除用户 %s(%s) 吗？（不建议删除用户。若要继续，请输入该用户的学工号:%s确认）\n",
                   pt->name,
                   pt->empId, pt->empId);
            char input_char[31];
            gets_safe(input_char, 30);
            if (strcmp(input_char, pt->empId) != 0) {
                printf("学工号不一致，已取消操作（按任意键继续）。\n");
                getch();
                goto User_GetAndDisplay;
            }

            char ret = DB_deleteUser(pt->id);
            if (ret == 0) {
                printf("[删除失败] 用户 %s(%s) 删除失败（按任意键继续）。\n", pt->name, pt->empId);
            } else {
                printf("[删除成功] 用户 %s(%s) 已被删除（按任意键继续）。\n", pt->name, pt->empId);
            }

            getch();
            goto User_GetAndDisplay;
        }
        case 13: // 编辑用户
            editUser(selectedRow->data);
            goto User_GetAndDisplay;
        case 27:
            goto GC_Collect;
        default:
            break;
    }
    goto GetKey;

    GC_Collect:
    linkListObject_Delete(user_data_list, 1);
    safe_free(&user_data_list);
    user_data_list = NULL;
}


#endif //COURSESYSTEM2023_USERS_SERVICE_H
