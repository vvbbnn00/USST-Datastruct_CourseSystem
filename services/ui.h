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
 * 输出错误信息
 * @param msg
 */
void UI_printErrorData(char *msg) {
    printf("[系统错误] %s（按任意键继续）\n", msg);
    getch();
}


/**
 * 居中输出消息
 * @param msg 消息
 * @param width 宽度
 */
void UI_printInMiddle(char *msg, int width, ...) {
    unsigned int str_length = (unsigned int) strlen(msg);
    char *final_str = calloc(str_length + 1000, sizeof(char));
    // 先将格式化的结果输出到字符串中
    va_list args;
    va_start(args, width);
    vsnprintf(final_str, str_length + 1000, msg, args);
    va_end(args);
    // 计算真正需要占用的长度
    str_length = strlen(final_str);
    if (width - (int) str_length > 0) {
        for (int i = 0; (i < (width - str_length) / 2); i++) printf(" ");
    }
    printf("%s\n", final_str);
}


/**
 * 正则表达式读入数据并验证
 * @param message 提示信息
 * @param pattern 正则表达式
 * @param _dest 目标字符串
 * @param max_length 最大读入长度
 *
 * @return 0 正常 -1 用户取消
 */
int UI_inputStringWithRegexCheck(char *message, char *pattern, char *_dest, int max_length) {
    char *input_string;
    printf("%s", message);

    InputString:
    printf("[请输入数据(Esc取消)] ");
    input_string = calloc(max_length + 1, sizeof(char));
    int str_len = 0, t; //记录长度
    while (1) {
        t = _getch();
        if (t == 27) {
            return -1; // 用户退出
        }
        if (t == '\r' || t == '\n') { //遇到回车，表明输入结束
            printf("\n");
            break; //while 循环的出口
        } else if (t == '\b') { //遇到退格，需要删除前一个星号
            if (str_len <= 0) continue; // 如果长度不足，则不执行退格操作
            printf("\b \b");  //退格，打一个空格，再退格，实质上是用空格覆盖掉星号
            --str_len;
        } else {
            if (str_len >= max_length) continue; // 限制输入长度
            input_string[str_len++] = (char) t;//将字符放入数组
            printf("%c", t);
        }
    }

    if (regexMatch(pattern, input_string) == 0) {
        safe_free((void **) &input_string);
        printf("输入的数据不符合规则，请重新输入\n");
        goto InputString;
    }

    strcpy(_dest, input_string);
    safe_free((void **) &input_string);
    return 0;
}


/**
 * 正则表达式读入数据并验证
 * @param message 提示信息
 * @param pattern 正则表达式
 * @param _dest 目标字符串
 * @param max_length 最大读入长度
 *
 * @return 0 正常 -1 用户取消
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
        printf("请输入正确的数字\n");
        safe_free((void **) &input_string);
        goto InputInteger;
    }
    *_dest = result;
    return 0;
}

/**
 * 正则表达式读入数据并验证（小数）
 * @param message 提示信息
 * @param pattern 正则表达式
 * @param _dest 目标字符串
 * @param max_length 最大读入长度
 *
 * @return 0 正常 -1 用户取消
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
        printf("请输入正确的数字\n");
        safe_free((void **) &input_string);
        goto InputFloat;
    }
    *_dest = result;
    return 0;
}

/**
 * 自增计数输出
 *
 * @param format
 * @param counter
 * @param selected 需要高亮的行
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

    // 不固定参数通过va_list保存，使用vprintf输出，可更好地封装print函数
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
 * UI模块，打印程序名称、版本号等信息
 */
void UI_printHeader(int width) {
    UI_printInMiddle("----------------------------", width);
    UI_printInMiddle("|        课程信息系统        |", width);
    UI_printInMiddle("|   %s   |", width, VERSION);
    UI_printInMiddle("----------------------------", width);
}

/**
 * UI模块，打印用户信息
 */
void UI_printUserInfo() {
    printf("\n-------- 用户信息 --------\n\n - 用户名： %s\n - 姓  名： %s\n - 角  色： %s\n\n",
           GlobalUser->empId,
           GlobalUser->name,
           Serv_User_getUserRole(GlobalUser->role));
}


/**
 * 按照左右两栏分栏形式输出菜单选项
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
 * 对传入的指令进行操作
 * @param command
 */
void UI_doMainMenuActions(int command) {
    switch (command) {
        // 修改密码
        case 5:
        case 13:
        case 23:
            Serv_User_changePassword();
            break;
            // 退出登录
        case 6:
        case 14:
        case 24:
            GlobalUser = NULL;
            longjmp(GLOBAL_goto_login, 1);
            // 退出程序
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
 * 按照权限输出主菜单模块
 * @param wrong_command 是否在先前输入过错误指令
 */
int UI_printMainMenu(char wrong_command) {
    init:
    system("MODE CON COLS=55 LINES=30 & cls");
    UI_printHeader(55);
    UI_printUserInfo();
    printf("============= 主菜单 - 请键入指令 =============\n\n");
    int INDEX = 1;
    switch (GlobalUser->role) {
        case 0: // 学生
            UI_printChoices("(1) 课程学习管理", &INDEX);
            UI_printChoices("(2) 查看当前课表", &INDEX);
            UI_printChoices("(3) 全校课程信息一览", &INDEX);
            UI_printChoices("(4) 查询课程成绩", &INDEX);
            UI_printChoices("(5) 修改密码", &INDEX);
            UI_printChoices("(6) 退出登录", &INDEX);
            UI_printChoices("(7) 退出程序", &INDEX);
            break;
        case 1: // 教师
            UI_printChoices("(1) 授课课程信息管理", &INDEX);
            UI_printChoices("(2) 全校课程信息一览", &INDEX);
            UI_printChoices("(3) 修改密码", &INDEX);
            UI_printChoices("(4) 退出登录", &INDEX);
            UI_printChoices("(5) 退出程序", &INDEX);
            break;
        case 2: // 管理员
            UI_printChoices("(1) 课程信息管理", &INDEX);
            UI_printChoices("(2) 用户管理", &INDEX);
            UI_printChoices("(3) 修改密码", &INDEX);
            UI_printChoices("(4) 退出登录", &INDEX);
            UI_printChoices("(5) 退出程序", &INDEX);
            break;
        default:
            longjmp(GLOBAL_goto_login, 2);
    }
    if (INDEX % 2 == 0) printf("\n");
    printf("\n===============================================\n");
    if (wrong_command) {
        printf("您输入的指令有误，请重新输入：");
        wrong_command = 0;
    } else { printf("请输入命令编号："); }
    char command_raw[11] = {0};
    fgets(command_raw, 10, stdin);
    fflush(stdin); // 清除多余的无效指令，防止影响后续操作
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
 * 打开文件对话框
 *
 * @param path 最终返回路径
 * @param fileType 文件类型，用\0分割，例如：All *.* Text *.TXT
 * @return 1 - 成功 0 - 失败
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
