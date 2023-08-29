//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_COURSE_SERV_H
#define COURSESYSTEM2023_COURSE_SERV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <conio.h>
#include <math.h>
#include "../models.h"
#include "../common.h"
#include "../utils/string_ext.h"
#include "../libs/link_list_object.h"
#include "users.h"
#include "ui.h"

User *GlobalUser;
const char *NUMBER_CAPITAL[10] = {"零", "一", "二", "三", "四", "五", "六", "日", "八", "九"};
const char *LECTURE_TYPE[4] = {"必修", "选修", "公选", "辅修"};


// 用于传递一个完整的数组，获取后再使用memcpy传递给course的schedule
typedef struct schedule_ {
    int schedule[7][13];
} Schedule;

struct studentCourseSelection { // 学生选课用
    int status; // 是否可选该门课程
    char locked_reason[100]; // 该门课不能选择的原因
    long long selection_time; // 选择该课程的时间
    Course course; // 课程信息
};

struct teacherCourseSelection { // 学生选课管理结构体
    User student; // 学生信息
    Course course; // 课程信息
    long long selection_time; // 选择该课程的时间
    int64 score; // 该门课程的成绩
    int64 id; // 选课ID
};


extern void printStudentCourseSelection();

extern void printStudentLectureTable();

extern void printAllCourses(int scene);

extern void editCourse(Course *_course);

extern Schedule editSchedule(int schedule[7][13]);

extern void importStuCourseData();

void printStudentList(Course *courseData);

/**
 * 获取课程状态
 *
 * @param status 课程状态
 * @return
 */
char *__getCourseStatus(int status) {
    switch (status) {
        case 0:
            return "可选";
        case 1:
            return "已选";
        case 2:
            return "锁定";
        default:
            return "未知";
    }
}


/**
 * 打印课程安排
 *
 * @param schedule
 * @return
 */
char *printSchedule(int schedule[7][13]) {
    char *final_str = (char *) calloc(2000, sizeof(char));
    if (final_str == NULL) return NULL;
    for (int i = 0; i < 6; i++) {
        char has_course = 0;
        char schedule_str[100] = "";
        char t_str[30] = "";
        char tt[10] = "";
        for (int j = 1; j <= 12; j++) {
            if (schedule[i][j]) {
                if (has_course) strcat(schedule_str, ",");
                has_course = 1;
                sprintf(tt, "%d", j);
                strcat(schedule_str, tt);
            }
        }
        if (has_course) {
            sprintf(t_str, "周%s:", NUMBER_CAPITAL[i + 1]);
            strcat(final_str, t_str);
            strcat(final_str, schedule_str);
            strcat(final_str, "节;");
        }
    }
    return final_str;
}


/**
 * 获取每周总学时
 *
 * @param schedule
 * @return
 */
int getTotalWeekHour(int schedule[][13]) {
    int ans = 0;
    for (int i = 0; i < 7; i++) {
        for (int j = 1; j <= 12; j++) {
            if (schedule[i][j]) {
                ans++;
            }
        }
    }
    return ans;
}


/**
 * 打印课程信息
 * @param selected_course
 */
void printCourseData(Course *selected_course) {
    printf("\n\t===== 课程信息·%s =====\n\n", selected_course->courseName);
    printf("\t课  程ID：%lld\n", selected_course->id);
    printf("\t课程名称：%s\n", selected_course->courseName);
    printf("\t课程简介：%s\n", selected_course->description);
    printf("\t课程性质：%s\n", LECTURE_TYPE[selected_course->type]);
    printf("\t授课周数：第%d周~第%d周\n", selected_course->weekStart, selected_course->weekEnd);
    char *courseArrangeStr = printSchedule(selected_course->schedule);
    if (courseArrangeStr != NULL) {
        printf("\t授课安排：%s\n", courseArrangeStr);
        free(courseArrangeStr);
    }
    printf("\t授课教师：%s(UID:%lld)\n", selected_course->teacher->name, selected_course->teacherId);
    printf("\t课程学时：%d学时\n",
           getTotalWeekHour(selected_course->schedule) * (selected_course->weekEnd - selected_course->weekStart + 1));
    printf("\t课程学分：%.2f\n", selected_course->points);
}


/**
 * 执行学生选课事宜
 */
void printStudentCourseSelection() {
    system("chcp 936>nul & cls & MODE CON COLS=110 LINES=50");
    HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    char search_kw[36] = "";
    double current_score = 0; // 本学期最多可选学分, 当前已选学分
    int total, page = 1, max_page, page_size = 10, current_total;

    LinkList_Node *selectedRow = NULL; // 被选中的行
    LinkList_Object *course_data_list = NULL;

    GetCourseAndDisplay:
    current_score = 0;
    // ... 请求过程
    printf("[提示] 正在请求...\n");

    NodeList *result;
    if (strlen(search_kw) > 0) {
        result = DB_getCoursesByName(search_kw);
    } else {
        result = DB_getAllCourses();
    }

    if (result == NULL) {
        printf("[提示] 无法获取课程数据\n");
        getch();
        goto GC_Collect;
    }

    total = 0;
    for (NodeList *pt = result; pt != NULL; pt = pt->next) total++;
    max_page = (int) ceil((double) total / page_size);
    current_total = (page == max_page) ? (total - (page - 1) * page_size) : page_size;

    // 解析课程数据
    course_data_list = linkListObject_Init(); // 链表初始化
    for (NodeList *pt = result; pt != NULL; pt = pt->next) {
        Course *course_data_pt = DB_getCourseById((int64) pt->indexNode->index.data);
        if (course_data_pt == NULL) {
            continue;
        }
        struct studentCourseSelection *tmp = calloc(1, sizeof(struct studentCourseSelection));
        if (tmp == NULL) {
            printf("[提示] 内存不足\n");
            getch();
            goto GC_Collect;
        }

        if (course_data_pt->currentMembers >= course_data_pt->maxMembers) {
            tmp->status = 2;
            strcpy(tmp->locked_reason, "课程已满员");
        }

        CourseSelection *selection = DB_getSelectionByUserIdAndCourseId(GlobalUser->id, course_data_pt->id);

        if (selection != NULL) {
            tmp->status = 1;
            tmp->selection_time = selection->selectionTime;
            current_score += course_data_pt->points;
        }

        tmp->course = *course_data_pt;
        linkListObject_Append(course_data_list, tmp);
    }
    if (course_data_list->head) {
        selectedRow = course_data_list->head;
    }

    Refresh:

    system("cls");
    printf("\n");
    UI_printHeader(102);
    printf("\n");
    UI_printInMiddle("======= 课程·学生选课 =======\n", 104);
    printf("%-16s%-35s%-35s%-10s%-12s\n", "选课状态", "课程ID", "课程名称", "课程学分", "授课教师");
    printf("--------------------------------------------------------------------------------------------------------\n");
    for (LinkList_Node *pt = course_data_list->head; pt != NULL; pt = pt->next) {
        struct studentCourseSelection *tmp = pt->data;
        if (pt == selectedRow) SetConsoleTextAttribute(windowHandle, 0x70);
        else {
            switch (tmp->status) {
                case 1: // 已选
                    SetConsoleTextAttribute(windowHandle, 0x0e);
                    break;
                case 2: // 锁定
                    SetConsoleTextAttribute(windowHandle, 0x08);
                    break;
                default:
                    break;
            }
        }
        printf("[%4s]%3d/%-3d   %-35lld%-35s%-10.2f%-12s\n",
               __getCourseStatus(tmp->status),
               tmp->course.currentMembers,
               tmp->course.maxMembers,
               tmp->course.id,
               tmp->course.courseName,
               tmp->course.points,
               tmp->course.teacher->name);
        SetConsoleTextAttribute(windowHandle, 0x07);
        if (pt->next == NULL) { // 链表的最后一个节点
            printf("\n");
        }
    }
    for (int i = 0; i < page_size - current_total; i++) printf("\n"); // 补齐页面
    printf("\n");
    UI_printInMiddle("=============================\n", 104);
    printf("[当前搜索条件] ");
    if (strlen(search_kw) > 0) { printf("模糊搜索=%s", search_kw); } else { printf("\n"); }
    printf("  [选课概况] 已选%.2f学分\n\n", current_score);
    printf("[提示] 共%4d条数据，当前第%3d页，共%3d页（左方向键：前一页；右方向键：后一页；上/下方向键：切换选中数据）\n",
           total, page, max_page);
    printf("\n\t   <Enter>选课/退选 <K>课程模糊查询 <Esc>返回主菜单\n");

    if (selectedRow == NULL) {
        UI_printErrorData("暂无可选课程");
//        getch();
        if (strlen(search_kw) == 0)
            goto GC_Collect;
        else {
            strcpy(search_kw, "");
            goto GetCourseAndDisplay;
        }
    }

    struct studentCourseSelection *selected_selection = selectedRow->data;
    Course *selected_course = &selected_selection->course;

    printCourseData(selected_course);
    switch (selected_selection->status) {
        case 1: // 已选
            SetConsoleTextAttribute(windowHandle, 0x0e);
            break;
        case 2: // 锁定
            SetConsoleTextAttribute(windowHandle, 0x08);
            break;
        default:
            break;
    }
    printf("\t状  态：  %s[%d/%d人]%s%s%s%s\n",
           __getCourseStatus(selected_selection->status),
           selected_course->currentMembers,
           selected_course->maxMembers,
           strlen(selected_selection->locked_reason) > 0 ? " - 不可选原因：" : "",
           selected_selection->locked_reason,
           selected_selection->selection_time > 0 ? " - 选课时间：" : "",
           selected_selection->selection_time > 0 ?
           getFormatTimeString(selected_selection->selection_time) : "");
    SetConsoleTextAttribute(windowHandle, 0x07);

    int keyboard_press;

    GetKey:
    keyboard_press = _getch();
    switch (keyboard_press) {
        case 224:
            keyboard_press = _getch();
            switch (keyboard_press) {
                case 80: // 下
                    if (selectedRow->next == NULL) selectedRow = course_data_list->head;
                    else selectedRow = selectedRow->next;
                    goto Refresh;
                case 72: // 上
                    if (selectedRow->prev == NULL) selectedRow = course_data_list->foot;
                    else selectedRow = selectedRow->prev;
                    goto Refresh;
                case 75: // 左
                    page = (page > 1) ? (page - 1) : 1;
                    selectedRow = 0;
                    goto GetCourseAndDisplay;
                case 77: // 右
                    page = (page < max_page) ? (page + 1) : (max_page);
                    selectedRow = 0;
                    goto GetCourseAndDisplay;
                default:
                    break;
            }
            break;
        case 'k': // 搜索关键词
        case 'K':
            printf("\n[模糊搜索]请输入关键词（以\"Enter\"键结束）：");
            gets_safe(search_kw, 35);
            fflush(stdin);
            selectedRow = 0;
            goto GetCourseAndDisplay;
        case 13: // 选课选项
            printf("[提示] 正在请求...\n");
            switch (selected_selection->status) {
                case 0: {
                    CourseSelection *selection = DB_selectCourse(GlobalUser->id, selected_course->id);
                    if (selection == NULL) {
                        printf("\n[选课失败] 选课失败，请重试（按任意键继续）\n");
                    } else {
                        printf("\n[选课成功] 选课成功，您的选课ID为：%lld（按任意键继续）\n", selection->id);
                    }
                    break;
                }
                case 1: {
                    CourseSelection *confirm = DB_withdrawCourse(GlobalUser->id, selected_course->id);
                    if (confirm == NULL) {
                        printf("\n[退选失败] 退选失败，请重试（按任意键继续）\n");
                    } else {
                        printf("\n[退选成功] 退选成功（按任意键继续）\n");
                    }
                    break;
                }
                default:
                    printf("\n[选课失败] 您无法选择该课程：%s（按任意键继续）\n",
                           selected_selection->locked_reason);
                    break;
            }
        After:
            getch();
            goto GetCourseAndDisplay;
        case 27:
            goto GC_Collect;
        default:
            break;
    }
    goto GetKey;

    GC_Collect:
    linkListObject_Delete(course_data_list, 1);
    free(course_data_list);
    course_data_list = NULL;
}

/**
 * 输出课表到文件流中（包括控制台）
 *
 * @param _stream
 * @param scheduleList
 */
void printTableToStream(FILE *_stream, LinkList_Object scheduleList[7][13]) {
    for (int week_num = 0; week_num < 7; week_num++) {
        char has_course = 0; // 当前星期是否有课
        fprintf(_stream, "[星期%s]\n\n", NUMBER_CAPITAL[week_num + 1]);
        for (int course_num = 1; course_num <= 12; course_num++) {
            LinkList_Node *head = scheduleList[week_num][course_num].head;
            if (head == NULL) continue; // 该时段没有课
            has_course = 1;
            for (LinkList_Node *pt = head; pt != NULL; pt = pt->next) {
                Course *c_data = pt->data;
                fprintf(_stream, "  <第");
                for (int i = course_num; i <= 12; i++) {
                    if (!c_data->schedule[week_num][i]) break;
                    fprintf(_stream, " %d", i);
                }
                fprintf(_stream, " 节> %s\n", c_data->courseName);
                fprintf(_stream, "    授课教师：%s，授课周：第%d周至第%d周，学分：%.2f\n",
                        c_data->teacher->name,
                        c_data->weekStart,
                        c_data->weekEnd,
                        c_data->points);
            }
        }
        if (!has_course) fprintf(_stream, "  无课程\n");
        fprintf(_stream, "\n");
    }
}


/**
 * 输出学生成绩表，并展示于控制台
 */
void printStudentScoreTable() {
    system("chcp 936>nul & cls & MODE CON COLS=80 LINES=100"); // 这里行数一定要大，不然数据会被刷掉

    int total; // 总结果条数
    double score_total; // 总学分
    LinkList_Object *scheduleList = linkListObject_Init(); // 解析后的课表数据放在这里，对应节次等信息[7]表示周一(0)至周日(6)，[13]表示第一节课(1)至第十二节课(12)

    // ... 请求过程
    printf("[提示] 正在请求...\n");

    IndexListNode *courseSelections = DB_getSelectionsByUserId(GlobalUser->id);

    // 解析课程数据
    for (IndexListNode *n = courseSelections; n != NULL; n = n->next) {
        CourseSelection *selection = DB_getSelectionById((int64) n->index.data);
        if (selection == NULL) continue;
        Course *course_data_pt = selection->course;
        score_total += course_data_pt->points;

        linkListObject_Append(scheduleList, selection);
    }

    Refresh:

    system("cls");
    printf("\n");
    UI_printHeader(76);
    printf("\n");
    UI_printInMiddle("======= 课程·成绩查询 =======\n", 77);
    printf("%-16s%-35s%-15s%-10s\n", "课程ID", "课程名称", "课程学分", "成绩");
    printf("--------------------------------------------------------------------------\n");
    for (LinkList_Node *pt = scheduleList->head; pt != NULL; pt = pt->next) {
        CourseSelection *selection = pt->data;
        Course *course_data_pt = selection->course;

        // 成绩按照条件显示
        char *showScore;
        if (selection->score < 0) {
            showScore = "未录入";
        } else {
            showScore = calloc(10, sizeof(char));
            showScore = lltoa(selection->score, showScore, 10);
        }

        printf("%-16lld%-35s%-15.2f%-10s\n",
               course_data_pt->id,
               course_data_pt->courseName,
               course_data_pt->points,
               showScore);
    }
    printf("\n");
    UI_printInMiddle("=============================\n", 77);
    printf("\n    [学生姓名] %s  [已选学分] %.2f\n",
           GlobalUser->name,
           score_total);
    printf("\n\t <Esc>返回主菜单\n\n");

    int keyboard_press;

    GetKey:
    keyboard_press = _getch();
    switch (keyboard_press) {
        case 27:
            goto GC_Collect;
        default:
            break;
    }
    goto GetKey;

    GC_Collect:
    linkListObject_Delete(scheduleList, 0);
}


/**
 * 输出学生课表，并展示于控制台
 */
void printStudentLectureTable() {
    system("chcp 936>nul & cls & MODE CON COLS=70 LINES=9001"); // 这里行数一定要大，不然数据会被刷掉

    int total; // 总结果条数
    double score_total; // 总学分
    LinkList_Object scheduleList[7][13] = {0}; // 解析后的课表数据放在这里，对应节次等信息[7]表示周一(0)至周日(6)，[13]表示第一节课(1)至第十二节课(12)

    // ... 请求过程
    printf("[提示] 正在请求...\n");

    IndexListNode *courseSelections = DB_getSelectionsByUserId(GlobalUser->id);

    // 解析课程数据
    for (IndexListNode *n = courseSelections; n != NULL; n = n->next) {
        CourseSelection *selection = DB_getSelectionById((int64) n->index.data);
        if (selection == NULL) continue;
        Course *course_data_pt = selection->course;
        score_total += course_data_pt->points;

        for (int week = 0; week < 7; week++) {
            int continuous = 0; // 判断是否是连续的课程安排，如果是 则算在一起，不重复添加在课表中
            for (int seq = 1; seq <= 12; seq++) {
                if (course_data_pt->schedule[week][seq]) {  // 该时段有课
                    if (!continuous) {
                        linkListObject_Append(&scheduleList[week][seq], course_data_pt); // 将课程加入进去
                        continuous = 1; // 重复，flag激活
                    }
                } else {
                    continuous = 0; // 遇到一个不重复的，则flag重置
                }
            }
        }
    }

    Refresh:

    system("cls");
    printf("\n");
    UI_printHeader(58);
    printf("\n");
    UI_printInMiddle("======= 课程·当前学期课表 =======\n", 60);
    printTableToStream(stdout, scheduleList);
    printf("------------------------------------------------------------\n");
    printf("\n    [学生姓名] %s  [已选学分] %.2f\n",
           GlobalUser->name,
           score_total);
    printf("\n\t <E>导出课表到文件中 <Esc>返回主菜单\n\n");

    int keyboard_press;

    GetKey:
    keyboard_press = _getch();
    switch (keyboard_press) {
        case 'e': // 导出课表
        case 'E': {
            time_t raw_time;
            time(&raw_time);
            char *timestamp = getFormatTimeString_("%Y%m%d%H%M%S", raw_time);
            char *name = GlobalUser->name;
            unsigned long long len = strlen(timestamp) +
                                     strlen(name) +
                                     strlen("的课表_.txt") + 1;
            char file_name[len];
            memset(file_name, 0, len);
            sprintf(file_name, "%s的课表_%s.txt", name, timestamp);
            free(timestamp);
            FILE *file = fopen(file_name, "w");
            printf("[提示] 正在将课表导出至：%s...\n", file_name);
            if (file == NULL) {
                printf("[导出失败] 无法创建文件\"%s\"，请检查是否有读写权限（按任意键继续）\n", file_name);
                goto Refresh;
            }
            printTableToStream(file, scheduleList);
            fclose(file);
            printf("[导出成功] 课表已导出至：%s（按任意键继续）\n", file_name);
            getch();
            goto Refresh;
        }
        case 27:
            goto GC_Collect;
        default:
            break;
    }
    goto GetKey;

    GC_Collect:
    for (int i = 0; i < 7; i++)
        for (int j = 1; j <= 12; j++) linkListObject_Delete(&scheduleList[i][j], 0);
}


/**
 * 输出全校课表（管理员版本）
 * @param scene 场景 0 - 查看全校课表（管理员可管理） 1 - 教师操作（只显示自己教的课程，可新增）
 */
void printAllCourses(int scene) {
    HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    char search_kw[36] = "";
    int total, page = 1, max_page, page_size = 15, current_total;

    LinkList_Node *selectedRow = NULL; // 被选中的行
    LinkList_Object *course_data_list = NULL;

    GetCourseAndDisplay:
    system("chcp 936>nul & cls & MODE CON COLS=130 LINES=55");

    printf("[提示] 正在请求...\n");

    NodeList *result;
    if (strlen(search_kw) > 0) {
        result = DB_getCoursesByName(search_kw);
    } else {
        result = DB_getAllCourses();
    }

    total = 0;
    for (NodeList *i = result; i != NULL; i = i->next) total++;
    max_page = (int) ceil((double) total / page_size);
    current_total = (page == max_page) ? (total - (page - 1) * page_size) : page_size;

    // 解析课程数据
    course_data_list = linkListObject_Init(); // 链表初始化
    int cnt = 0;
    for (NodeList *j = result; j != NULL; j = j->next) {
        cnt += 1;
        if (cnt > (page - 1) * page_size && cnt <= (page - 1) * page_size + current_total) {
            Course *course_data_pt = DB_getCourseById((int64) j->indexNode->index.data);
            if (course_data_pt == NULL) {
                printf("[错误] 无法获取课程信息（课程ID=%lld）\n", (int64) j->indexNode->index.data);
                continue;
            }
            Course *insert = memcpy(malloc(sizeof(Course)), course_data_pt, sizeof(Course));
            linkListObject_Append(course_data_list, insert);
        }
    }
    if (course_data_list->head != NULL) selectedRow = course_data_list->head;

    Refresh:

    system("cls");
    printf("\n");
    UI_printHeader(120);
    printf("\n");
    UI_printInMiddle("======= 课程·全校课表一览 =======\n", 122);
    printf("%-35s%-35s%-10s%-10s%-12s\n", "课程ID", "课程名称", "课程学分", "课程性质", "授课教师");
    printf("--------------------------------------------------------------------------------------------------------------------------\n");
    for (LinkList_Node *pt = course_data_list->head; pt != NULL; pt = pt->next) {
        Course *tmp = pt->data;
        if (pt == selectedRow) SetConsoleTextAttribute(windowHandle, 0x70);
        printf("%-35lld%-35s%-10.2f%-10s%-12s\n",
               tmp->id,
               tmp->courseName,
               tmp->points,
               LECTURE_TYPE[tmp->type],
               tmp->teacher->name);
        SetConsoleTextAttribute(windowHandle, 0x07);
        if (pt->next == NULL) { // 链表的最后一个节点
            printf("\n");
        }
    }
    for (int i = 0; i < page_size - current_total; i++) printf("\n"); // 补齐页面
    printf("\n");
    UI_printInMiddle("=============================\n", 122);
    printf("[当前搜索条件] ");
    if (strlen(search_kw) > 0) { printf("模糊搜索=%s\n", search_kw); } else { printf("\n"); }
    printf("[提示] 共%4d条数据，当前第%3d页，共%3d页（左方向键：前一页；右方向键：后一页；上/下方向键：切换选中数据）\n",
           total, page, max_page);
    printf("\n");
    if (GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1)) { // 管理员可编辑课程
        printf("<A>开设/新建课程 <Enter>编辑课程 <P>查看学生名单 <D>删除课程");
    } else {
        printf("\t  ");
    }
    printf(" <K>课程模糊查询");
    printf(" <Esc>返回主菜单\n");

    if (selectedRow == NULL) {
        if (strlen(search_kw)) {
            UI_printErrorData("没有查询到符合条件的课程");
            strcpy(search_kw, "");
            goto GetCourseAndDisplay;
        } else {
            if ((GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1))) { // 管理员可编辑课程
                printf("暂无课程，是否立即创建课程？(Y)");
                int ch = getch();
                if (ch == 'Y' || ch == 'y') {
                    editCourse(NULL);
                }
            } else {
                UI_printErrorData("暂无课程");
            }
            goto GC_Collect;
        }
    }

    printCourseData(selectedRow->data);

    int keyboard_press;

    GetKey:
    keyboard_press = _getch();
    switch (keyboard_press) {
        case 224:
            keyboard_press = _getch();
            switch (keyboard_press) {
                case 80: // 下
                    if (selectedRow->next == NULL) selectedRow = course_data_list->head;
                    else selectedRow = selectedRow->next;
                    goto Refresh;
                case 72: // 上
                    if (selectedRow->prev == NULL) selectedRow = course_data_list->foot;
                    else selectedRow = selectedRow->prev;
                    goto Refresh;
                case 75: // 左
                    page = (page > 1) ? (page - 1) : 1;
                    selectedRow = 0;
                    goto GetCourseAndDisplay;
                case 77: // 右
                    page = (page < max_page) ? (page + 1) : (max_page);
                    selectedRow = 0;
                    goto GetCourseAndDisplay;
                default:
                    break;
            }
            break;
        case 'k': // 搜索关键词
        case 'K':
            printf("\n[模糊搜索]请输入关键词（以\"Enter\"键结束）：");
            gets_safe(search_kw, 35);
            fflush(stdin);
            selectedRow = 0;
            goto GetCourseAndDisplay;
        case 'A':
        case 'a':
            if (!(GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1))) break; // 权限判断
            editCourse(NULL);
            goto GetCourseAndDisplay;
        case 'D':
        case 'd': // 删除课程
            if (!(GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1))) break; // 权限判断
            {
                Course *pt = selectedRow->data;
                printf("\n[确认删除] 您确定要删除课程 %s(%lld) 吗？（删除课程后，该课程所有的学生选课记录将被清空！若要继续，请输入该课程ID:%lld确认）\n",
                       pt->courseName,
                       pt->id,
                       pt->id);
                char input_char[33];
                gets_safe(input_char, 32);
                int64 input_int = strtoll(input_char, NULL, 10);
                if (input_int != pt->id) {
                    printf("课程ID不一致，已取消操作（按任意键继续）。\n");
                    getch();
                    goto GetCourseAndDisplay;
                }

                DB_deleteCourse(pt->id); // 删除课程

                printf("[删除成功] 课程 %s(%lld) 和相关选课记录已被删除（按任意键继续）。\n", pt->courseName,
                       pt->id);
                getch();
                goto GetCourseAndDisplay;
            }
        case 'p':
        case 'P':
            if (!(GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1))) break; // 权限判断
            printStudentList((Course *) selectedRow->data);
            goto GetCourseAndDisplay;
        case 13: // 编辑课程
            if (!(GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1))) break; // 权限判断
            editCourse((Course *) selectedRow->data);
            goto GetCourseAndDisplay;
        case 27:
            goto GC_Collect;
        default:
            break;
    }
    goto GetKey;

    GC_Collect:
    linkListObject_Delete(course_data_list, 1);
    free(course_data_list);
    course_data_list = NULL;
}


/**
 * 导出学生列表
 * @param linkList
 * @param course
 * @return
 */
int exportStudentList(LinkList_Object *linkList, Course *course) {
    time_t raw_time;
    time(&raw_time);
    char *timestamp = getFormatTimeString_("%Y%m%d%H%M%S", raw_time);
    char *title = course->courseName;
    unsigned long long len = strlen(timestamp) +
                             strlen(title) +
                             strlen("()学生名单_.csv") + 10;
    char file_name[len];
    memset(file_name, 0, len);
    sprintf(file_name, "%s(%lld)学生名单_%s.csv", title, course->id, timestamp);
    free(timestamp);
    FILE *file = fopen(file_name, "w");
    printf("[提示] 正在将学生名单导出至：%s...\n", file_name);
    if (file == NULL) {
        printf("[导出失败] 无法创建文件\"%s\"，请检查是否有读写权限（按任意键继续）\n", file_name);
        return -1;
    }
    fprintf(file, "序号,姓名,学号,选课时间,成绩,联系方式\n");
    int counter = 1;
    for (LinkList_Node *pt = linkList->head; pt != NULL; pt = pt->next) {
        struct teacherCourseSelection *p = pt->data;

        // 成绩按照条件显示
        char *showScore;
        if (p->score < 0) {
            showScore = "未录入";
        } else {
            showScore = calloc(10, sizeof(char));
            showScore = lltoa(p->score, showScore, 10);
        }

        fprintf(file, "%d,%s,%s,%s,%s,%s\n",
                counter,
                p->student.name,
                p->student.empId,
                getFormatTimeString(p->selection_time),
                showScore,
                p->student.contact);
        counter++;
    }
    fclose(file);
    printf("[导出成功] 学生名单已导出至：%s（按任意键继续）\n", file_name);
    return 0;
}


/**
 * 教师/管理员打印学生名单
 */
void printStudentList(Course *courseData) {
    system("chcp 936>nul & cls & MODE CON COLS=100 LINES=55");
    HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    int sort_method = 0; // 排序方法 0 - 默认排序 1 - 学分降序 2 - 学分升序
    int total, page = 1, page_size = 30, max_page;
    int stat_a90 = 0, stat_a80 = 0, stat_a60 = 0, stat_a0 = 0; // 统计数据

    LinkList_Object *student_list = NULL;
    LinkList_Node *selectedRow = NULL;

    Teacher_GetCourseAndDisplay:
    printf("[提示] 正在请求...\n");

    IndexListNode *result = DB_getSelectionsByCourseId(courseData->id);
    total = 0;

    stat_a90 = 0, stat_a80 = 0, stat_a60 = 0, stat_a0 = 0;
    for (IndexListNode *i = result; i != NULL; i = i->next) {
        total++;
    }

    max_page = (int) ceilf((float) total / (float) page_size);

    // 解析名单数据
    student_list = linkListObject_Init(); // 链表初始化

    int cnt = 0;
    for (IndexListNode *j = result; j != NULL; j = j->next) {
        cnt += 1;
        if (cnt > (page - 1) * page_size && cnt <= (page - 1) * page_size + page_size) {
            CourseSelection *courseSelection = DB_getSelectionById((int64) j->index.data);
            struct teacherCourseSelection *insert = malloc(sizeof(struct teacherCourseSelection));
            insert->student = *courseSelection->student;
            insert->selection_time = courseSelection->selectionTime;
            insert->course = *courseSelection->course;
            insert->score = courseSelection->score;
            insert->id = courseSelection->id;

            if (insert == NULL) {
                printf("[错误] 无法获取学生选课信息（选课ID=%lld）\n", (int64) j->index.data);
                continue;
            }
            linkListObject_Append(student_list, insert);
        }
    }
    if (student_list->head != NULL) selectedRow = student_list->head;

    Teacher_Refresh:

    system("cls");
    printf("\n");
    UI_printHeader(99);
    printf("\n");
    UI_printInMiddle("======= 课程·课程名单 =======\n", 101);
    printf("%-6s%-15s%-20s%-30s%-10s%-15s\n", "序号", "姓名", "学号", "选课时间", "成绩", "联系方式");
    printf("------------------------------------------------------------------------------------------------\n");
    int counter = 0, current_total = 0;
    for (LinkList_Node *pt = student_list->head; pt != NULL; pt = pt->next) {
        counter++;
        if (ceilf((float) counter / (float) page_size) < (float) page) continue; // 定位到该页面，学生名单分页显示
        if (ceilf((float) counter / (float) page_size) > (float) page) break; // 一页显示完即可跳出循环
        current_total++;
        if (selectedRow == NULL) selectedRow = pt;
        struct teacherCourseSelection *tmp = pt->data;
        if (pt == selectedRow) SetConsoleTextAttribute(windowHandle, 0x70);

        char *showScore;
        if (tmp->score < 0) {
            showScore = "未录入";
        } else {
            showScore = calloc(10, sizeof(char));
            showScore = lltoa(tmp->score, showScore, 10);
        }

        printf("%4d  %-15s%-20s%-30s%-10s%-15s\n",
               counter,
               tmp->student.name,
               tmp->student.empId,
               getFormatTimeString(tmp->selection_time),
               showScore,
               tmp->student.contact);
        SetConsoleTextAttribute(windowHandle, 0x07);
        if (pt->next == NULL) { // 链表的最后一个节点
            printf("\n");
        }
    }
    for (int i = 0; i < page_size - current_total - 1; i++) printf("\n"); // 补齐页面
    printf("\n");
    UI_printInMiddle("=============================\n", 71);
    printf("    [课程名称] %s [课程ID] %lld\n\n", courseData->courseName, courseData->id);
    printf("    [提示] 共%4d条数据，当前第%3d页，共%3d页\n\n    （左方向键：前一页；右方向键：后一页；上/下方向键：切换选中数据）\n",
           total, page, max_page);
    printf("\n  ");
    if (GlobalUser->role == 2) { // 管理员可编辑课程
        printf("<A>添加学生 <D>取消该学生选课 <I>批量导入学生");
    } else {
        printf("\t");
    }
    printf(" <S>录入该学生成绩 <G>统计成绩分段信息 <E>导出学生名单 <Esc>返回主菜单\n");

    if (selectedRow == NULL) {
        UI_printErrorData("暂无名单");
        if (GlobalUser->role == 2) { // 管理员可编辑课程
            printf("您是管理员，是否导入学生名单？(Y)");
            int ch = getch();
            if (ch == 'Y' || ch == 'y') {
                importStuCourseData();
            }
        }
        goto Teacher_GC_Collect;
    }
    int keyboard_press;

    Teacher_GetKey:
    keyboard_press = _getch();
    switch (keyboard_press) {
        case 224:
            keyboard_press = _getch();
            switch (keyboard_press) {
                case 80: // 下
                    if (selectedRow->next == NULL) selectedRow = student_list->head;
                    else selectedRow = selectedRow->next;
                    goto Teacher_Refresh;
                case 72: // 上
                    if (selectedRow->prev == NULL) selectedRow = student_list->foot;
                    else selectedRow = selectedRow->prev;
                    goto Teacher_Refresh;
                case 75: // 左
                    page = (page > 1) ? (page - 1) : 1;
                    selectedRow = 0;
                    goto Teacher_Refresh;
                case 77: // 右
                    page = (page < max_page) ? (page + 1) : (max_page);
                    selectedRow = 0;
                    goto Teacher_Refresh;
                default:
                    break;
            }
            break;
        case 'E':
        case 'e':
            exportStudentList(student_list, courseData);
            _getch();
            goto Teacher_Refresh;
        case 'I':
        case 'i': // 批量导入学生信息
            if (GlobalUser->role != 2) break; // 无法新增学生
            importStuCourseData();
            goto Teacher_GetCourseAndDisplay;
        case 'G':
        case 'g': // 显示分段分数信息
        {
            stat_a90 = 0, stat_a80 = 0, stat_a60 = 0, stat_a0 = 0;
            for (IndexListNode *i = result; i != NULL; i = i->next) {
                CourseSelection *courseSelection = DB_getSelectionById((int64) i->index.data);

                if (courseSelection->score >= 90) stat_a90++;
                else if (courseSelection->score >= 80) stat_a80++;
                else if (courseSelection->score >= 60) stat_a60++;
                else stat_a0++;
            }
            printf("\n[成绩分段统计] 90分以上：%d人，80-89分：%d人，60-79分：%d人，60分以下：%d人\n", stat_a90, stat_a80, stat_a60,
                   stat_a0);
            getch();
            goto Teacher_Refresh;
        }
        case 'S':
        case 's': // 录入学生成绩
        {
            int score = -1;
            UI_inputIntWithRegexCheck("[修改课程] 请录入学生成绩（0-100分）\n",
                                      POINTS_PATTERN,
                                      &score);
            if (score < 0 || score > 100) {
                printf("[录入失败] 无效的成绩（按任意键继续）\n");
                getch();
                goto Teacher_Refresh;
            }
            struct teacherCourseSelection *pt = selectedRow->data;

            if (DB_updateSelectionScore(pt->id, score)) {
                printf("[录入成功] 成绩已录入（按任意键继续）\n");
            } else {
                printf("[录入失败] 成绩录入失败（按任意键继续）\n");
            }
            getch();
            goto Teacher_Refresh;
        }
        case 'A':
        case 'a': // 新增选课学生
            if (GlobalUser->role != 2) break; // 无法新增学生
            {
                char user_id[31] = {0};
                if (UI_inputStringWithRegexCheck(
                        "[课程新增学生] 请输入学生学号（如需批量导入学生信息，请使用“批量导入学生”功能）\n",
                        USER_PATTERN,
                        user_id,
                        30) == -1)
                    goto Teacher_Refresh;
                User *user = DB_getUserByEmpId(user_id);

                if (user == NULL) {
                    printf("[新增失败] 用户 %s 不存在（按任意键继续）\n", user_id);
                    getch();
                    goto Teacher_Refresh;
                }

                CourseSelection *selection = DB_selectCourse(user->id, courseData->id);

                if (selection == NULL) {
                    printf("[新增失败] 校验失败（按任意键继续）\n");
                    getch();
                    goto Teacher_Refresh;
                }

                printf("[新增成功] 用户 %s(%s) 已成功选课（按任意键继续）\n", user->name, user->empId);

                getch();
                goto Teacher_GetCourseAndDisplay;
            }
            break;
        case 'D':
        case 'd': // 退选课程
            if (GlobalUser->role != 2) break;
            {
                struct teacherCourseSelection *pt = selectedRow->data;
                printf("\n[确认退选] 您确定要退选学生 %s(%s) 吗？（请输入该学生的学号:%s确认）\n",
                       pt->student.name,
                       pt->student.empId,
                       pt->student.empId);
                char input_char[31];
                gets_safe(input_char, 30);
                if (strcmp(input_char, pt->student.empId) != 0) {
                    printf("学号不一致，已取消操作（按任意键继续）。\n");
                    getch();
                    goto Teacher_GetCourseAndDisplay;
                }

                User *user = DB_getUserByEmpId(input_char);
                if (user == NULL) {
                    printf("学号不存在，已取消操作（按任意键继续）。\n");
                    getch();
                    goto Teacher_GetCourseAndDisplay;
                }

                CourseSelection *ret = DB_withdrawCourse(user->id, courseData->id); // 删除选课
                if (ret == NULL) {
                    printf("[退选失败] 退选失败（按任意键继续）\n");
                    getch();
                    goto Teacher_GetCourseAndDisplay;
                }

                printf("[退选成功] 用户 %s(%s) 已成功退选（按任意键继续）\n", user->name, user->empId);
                free(ret);
                getch();
                goto Teacher_GetCourseAndDisplay;
            }
            break;
        case 27:
            goto Teacher_GC_Collect;
        default:
            break;
    }
    goto Teacher_GetKey;

    Teacher_GC_Collect:
    linkListObject_Delete(student_list, 1);
    free(student_list);
    student_list = NULL;
}

/**
 * （控件）修改上课安排
 * @param schedule
 * @return
 */
Schedule editSchedule(int schedule[7][13]) {
    HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (schedule == NULL) {
        schedule = alloca(sizeof(int[7][13]));
        memset(schedule, 0, sizeof(int[7][13]));
    }
    Schedule returnSchedule;
    memcpy(returnSchedule.schedule, schedule, sizeof(int[7][13]));
    int x = 1, y = 0;

    ScheduleEditor_Refresh:
    system("chcp 936>nul & cls & MODE CON COLS=65 LINES=32");
    UI_printHeader(50);
    UI_printInMiddle("\n======= 课程·编辑授课安排 =======\n", 52);
    printf("-------------------------------------------------------\n");
    printf("   节次     周一  周二  周三  周四  周五  周六  周日\n");
    printf("-------------------------------------------------------\n");
    for (int i = 1; i <= 12; i++) {
        printf("  第%2d节课 ", i);
        for (int j = 0; j < 7; j++) {
            if (x == i && y == j) SetConsoleTextAttribute(windowHandle, 0x70);
            printf("  %2s  ", returnSchedule.schedule[j][i] == 1 ? "√" : " ");
            SetConsoleTextAttribute(windowHandle, 0x07);
        }
        if (i == 5 || i == 9) printf("\n"); // 分时段
        printf("\n");
    }

    printf("\n\n<Enter>切换选中状态 <Y>结束编辑 <ESC>取消编辑 <上/下/左/右>切换\n");

    int key;
    ScheduleEditor_GetKey:
    key = _getch();
    switch (key) {
        case 224:
            key = _getch();
            switch (key) {
                case 80: // 下
                    if (x < 12) x++;
                    else x = 1;
                    goto ScheduleEditor_Refresh;
                case 72: // 上
                    if (x > 1) x--;
                    else x = 12;
                    goto ScheduleEditor_Refresh;
                case 75: // 左
                    if (y > 0) y--;
                    else y = 6;
                    goto ScheduleEditor_Refresh;
                case 77: // 右
                    if (y < 6) y++;
                    else y = 0;
                    goto ScheduleEditor_Refresh;
                default:
                    break;
            }
            break;
        case 13:
            returnSchedule.schedule[y][x] = returnSchedule.schedule[y][x] == 1 ? 0 : 1;
            goto ScheduleEditor_Refresh;
        case 'y':
        case 'Y':
            return returnSchedule;
        case 27:
            memcpy(returnSchedule.schedule, schedule, sizeof(int[7][13]));
            return returnSchedule;
        default:
            break;
    }
    goto ScheduleEditor_GetKey;
}


/**
 * 新增课程
 * @return 取消则返回NULL，否则返回课程指针
 */
Course *addCourse() {
    system("chcp 936>nul & cls & MODE CON COLS=75 LINES=55");
    printf("\n");
    UI_printHeader(69);
    printf("\n");
    UI_printInMiddle("======= 课程·新增/修改课程 =======\n", 71);

    Course *course = calloc(1, sizeof(Course));
    if (GlobalUser->role == 2) { // 管理员开课需指定老师用户
        char *empId = alloca(sizeof(char[31]));
        if (UI_inputStringWithRegexCheck("[新增课程] 请输入开课教师工号（用户角色需为教师）\n",
                                         USER_PATTERN,
                                         empId, 30) == -1)
            goto CancelAdd;
        User *teacher = DB_getUserByEmpId(empId);
        if (teacher == NULL) {
            printf("[新增课程] 未找到工号为 %s 的教师，请检查工号是否正确（按任意键继续）\n", empId);
            getch();
            goto CancelAdd;
        }
        course->teacher = teacher;
        course->teacherId = teacher->id;
    } else {
        course->teacher = GlobalUser;
        course->teacherId = GlobalUser->id;
    }
    if (UI_inputStringWithRegexCheck("[新增课程] 请输入课程标题（长度为5-50字符，由字母、数字、中文和()-组成的字符串）\n",
                                     COURSE_TITLE_PATTERN,
                                     course->courseName,
                                     100) == -1)
        goto CancelAdd;
    if (UI_inputStringWithRegexCheck("[新增课程] 请输入课程描述（长度为5-250字符）\n",
                                     COURSE_DESCRIPTION_PATTERN,
                                     course->description,
                                     500) == -1)
        goto CancelAdd;
    if (UI_inputStringWithRegexCheck("[新增课程] 请输入上课地点（长度为5-50字符，由字母、数字、中文和()-组成的字符串）\n",
                                     COURSE_TITLE_PATTERN,
                                     course->location,
                                     100) == -1)
        goto CancelAdd;
    if (UI_inputIntWithRegexCheck("[新增课程] 请输入课程类型：0-必修、1-选修、2-公选、3-辅修\n",
                                  COURSE_TYPE_PATTERN,
                                  &course->type))
        goto CancelAdd;
    if (UI_inputIntWithRegexCheck("[新增课程] 请输入课程开始周数（大于0的整数）\n",
                                  NUMBER_PATTERN,
                                  &course->weekStart))
        goto CancelAdd;
    if (UI_inputIntWithRegexCheck("[新增课程] 请输入课程结束周数（大于0的整数）\n",
                                  NUMBER_PATTERN,
                                  &course->weekEnd))
        goto CancelAdd;
    if (UI_inputFloatWithRegexCheck("[新增课程] 请输入课程学分（大于0的小数，最多精确至小数点后2位）\n",
                                    POINTS_PATTERN,
                                    &course->points))
        goto CancelAdd;
    if (UI_inputIntWithRegexCheck("[新增课程] 请输入最大选课人数（大于0的整数）\n",
                                  POINTS_PATTERN,
                                  &course->maxMembers))
        goto CancelAdd;
    printf("[新增课程] 按任意键开始编辑授课安排\n");
    getch();
    Schedule ret = editSchedule(NULL);
    memcpy(course->schedule, ret.schedule, sizeof(int[7][13]));

    goto Return;

    CancelAdd:  // 取消添加
    free(course);
    course = NULL;

    Return:  // 返回数据
    return course;
}


/**
 * 新增/修改课程
 *
 * @param _course 当为NULL时，新增课程，否则为修改课程
 * @return 课程指针
 */
void editCourse(Course *_course) {
    // 如无课程信息，则新增课程
    int action = 0;
    Course *course = _course;

    if (course == NULL) {
        course = addCourse();
        action = 1;
        if (course == NULL) {
            return;
        }
    }

    int counter = 0, selected = 0, key;

    EditCourse_Refresh:

    counter = 0;
    system("chcp 936>nul & cls & MODE CON COLS=75 LINES=55");
    printf("\n");
    UI_printHeader(69);
    printf("\n");
    UI_printInMiddle("======= 课程·新增/修改课程 =======\n\n", 71);

    UI_selfPlusPrint("\t\t课程ID        %lld\n", &counter, selected, course->id); // 0
    UI_selfPlusPrint("\t\t课程名称      %s\n", &counter, selected, course->courseName); // 1
    UI_selfPlusPrint("\t\t课程简介      %s\n", &counter, selected, course->description); // 2
    UI_selfPlusPrint("\t\t上课地点      %s\n", &counter, selected, course->location); // 3
    UI_selfPlusPrint("\t\t课程性质      %s\n", &counter, selected, LECTURE_TYPE[course->type]); // 4
    UI_selfPlusPrint("\t\t授课周数      第%d周~第%d周\n", &counter, selected, course->weekStart, course->weekEnd); // 5
    char *courseArrangeStr = printSchedule(course->schedule); // 6
    if (courseArrangeStr != NULL) {
        UI_selfPlusPrint("\t\t授课安排      %s\n", &counter, selected, courseArrangeStr);
        free(courseArrangeStr);
    }
    UI_selfPlusPrint("\t\t授课教师      %s(UID:%lld)\n", &counter, selected, course->teacher->name, // 7
                     course->teacherId);
    UI_selfPlusPrint("\t\t选修人数      %d/%d人\n", &counter, selected, course->currentMembers,
                     course->maxMembers);  // 8
    UI_selfPlusPrint("\t\t课程学分      %.2f\n", &counter, selected, course->points);  // 9
    UI_selfPlusPrint("\t\t课程学时      %d学时\n", &counter, selected, // 10
                     getTotalWeekHour(course->schedule) * (course->weekEnd - course->weekStart + 1));

    printf("\n");
    UI_printInMiddle("<Enter>修改选中行 <Y>提交修改 <Esc>取消修改", 71);

    EditCourse_GetKey:

    key = _getch();
    switch (key) {
        case 224:
            key = _getch();
            switch (key) {
                case 80: // 下
                    if (selected < 10) selected++;
                    else selected = 0;
                    goto EditCourse_Refresh;
                case 72: // 上
                    if (selected > 0) selected--;
                    else selected = 10;
                    goto EditCourse_Refresh;
                default:
                    break;
            }
            break;
        case 13:
            switch (selected) {
                case 1:
                    UI_inputStringWithRegexCheck(
                            "[修改课程] 请输入课程标题（长度为5-50字符，由字母、数字、中文和()-组成的字符串）\n",
                            COURSE_TITLE_PATTERN,
                            course->courseName,
                            100);
                    break;
                case 2:
                    UI_inputStringWithRegexCheck("[修改课程] 请输入课程描述（长度为5-250字符）\n",
                                                 COURSE_DESCRIPTION_PATTERN,
                                                 course->description,
                                                 500);
                    break;
                case 3:
                    UI_inputStringWithRegexCheck(
                            "[修改课程] 请输入上课地点（长度为5-50字符，由字母、数字、中文和()-组成的字符串）\n",
                            COURSE_TITLE_PATTERN,
                            course->location,
                            100);
                    break;
                case 4:
                    UI_inputIntWithRegexCheck("[修改课程] 请输入课程类型：0-必修、1-选修、2-公选、3-辅修\n",
                                              COURSE_TYPE_PATTERN,
                                              &course->type);
                    break;
                case 5:
                    if (UI_inputIntWithRegexCheck("[修改课程] 请输入课程开始周数（大于0的整数）\n",
                                                  NUMBER_PATTERN,
                                                  &course->weekStart) == -1)
                        break;
                    UI_inputIntWithRegexCheck("[修改课程] 请输入课程结束周数（大于0的整数）\n",
                                              NUMBER_PATTERN,
                                              &course->weekEnd);
                    break;
                case 6: {
                    Schedule ret = editSchedule(course->schedule);
                    memcpy(course->schedule, ret.schedule, sizeof(int[7][13]));
                }
                    break;
                case 8:
                    UI_inputIntWithRegexCheck("[修改课程] 请输入最大选课人数（大于0的整数）\n",
                                              POINTS_PATTERN,
                                              &course->maxMembers);
                    break;
                case 9:
                    UI_inputFloatWithRegexCheck("[修改课程] 请输入课程学分（大于0的小数，最多精确至小数点后2位）\n",
                                                POINTS_PATTERN,
                                                &course->points);
                    break;
                default:
                    break;
            }
            goto EditCourse_Refresh;
        case 'y':
        case 'Y':
            if (course->weekStart <= course->weekEnd && course->weekStart > 0 && course->maxMembers > 0 &&
                course->points > 0) {

                if (course->id) {
                    DB_updateCourse(course);
                } else {
                    Course *ret = DB_createCourse(course->courseName,
                                                  course->description,
                                                  course->teacherId,
                                                  course->type,
                                                  course->weekStart,
                                                  course->weekEnd,
                                                  0,
                                                  course->maxMembers,
                                                  course->location,
                                                  course->points,
                                                  course->schedule);
                    if (ret == NULL) {
                        printf("[提交失败] 课程修改/新增失败（按任意键继续）\n");
                        getch();
                        goto EditCourse_Refresh;
                    }
                }

                printf("[提交成功] 课程修改/新增成功（按任意键返回课程一览）\n");
                getch();
                goto GC_COLLECT;
            } else {
                printf("[提交失败] 开课周应小于等于结束周，且开课周、学分和最大人数应大于0。(按任意键继续)\n");
                getch();
                goto EditCourse_Refresh;
            }
            break;
        case 27:
            goto GC_COLLECT;
        default:
            break;
    }
    goto EditCourse_GetKey;

    GC_COLLECT:
    if (_course == NULL) free(course);
}


/**
 * 导入学生选课列表
 */
void importStuCourseData() {

    typedef struct _import_data {
        char uid[21], course_id[33];
    } ImportData;

    LinkList_Object *import_list = linkListObject_Init(); // 初始化链表
    int page = 1, max_page, page_size = 30, total = 0, key;

    Import_Refresh:

    max_page = (int) ceilf((float) total / (float) page_size);
    system("chcp 936>nul & cls & MODE CON COLS=75 LINES=55");
    printf("\n");
    UI_printHeader(54);
    printf("\n");
    UI_printInMiddle("======= 课程·导入学生名单 =======\n\n", 56);
    printf("%-6s%-20s%-30s\n", "序号", "学号", "选修课程ID");
    printf("--------------------------------------------------------\n");
    int counter = 0, current_total = 0;
    for (LinkList_Node *pt = import_list->head; pt != NULL; pt = pt->next) {
        counter++;
        if (ceilf((float) counter / (float) page_size) < (float) page) continue; // 定位到该页面，学生名单分页显示
        if (ceilf((float) counter / (float) page_size) > (float) page) break; // 一页显示完即可跳出循环
        current_total++;
        ImportData *tmp = pt->data;
        printf("%4d  %-20s%-30s\n",
               counter,
               tmp->uid,
               tmp->course_id);
        if (pt->next == NULL) { // 链表的最后一个节点
            printf("\n");
        }
    }
    for (int i = 0; i < page_size - current_total - 1; i++) printf("\n"); // 补齐页面
    printf("\n");
    UI_printInMiddle("=============================\n", 56);
    printf("    [提示] 共%4d条数据，当前第%3d页，共%3d页\n\n    （左方向键：前一页；右方向键：后一页）\n\n",
           total, page, max_page);
    UI_printInMiddle("<T>导出模板 <I>选择文件 <Y>确认导入 <Esc>取消并返回", 56);
    printf("\n");

    Import_GetKey:

    key = _getch();
    switch (key) {
        case 224:
            key = _getch();
            switch (key) {
                case 75: // 左
                    if (page > 1) page--;
                    else break;
                    goto Import_Refresh;
                case 77: // 右
                    if (page < max_page) page++;
                    else break;
                    goto Import_Refresh;
                default:
                    break;
            }
            break;
        case 'T':
        case 't': {
            FILE *fp = fopen("import_template.csv", "w");
            if (fp == NULL) {
                printf("[提示] 导出模板失败，请检查文件权限。（按任意键继续）\n");
                getch();
                goto Import_Refresh;
            }
            fprintf(fp, "学号,课程ID\n2135060620(示例，请删除),2022-01-0000001-01(示例，请删除)\n");
            fclose(fp);
            printf("[提示] 导出模板成功，模板保存至\"import_template.csv\"文件中。（按任意键继续）\n");
            goto Import_Refresh;
        }
        case 'Y':
        case 'y': { // 批量导入
            if (total == 0) {
                printf("[导入学生失败] 列表为空，请先按\"I\"加载数据（按任意键继续）\n");
                getch();
                goto Import_Refresh;
            }
            printf("[提示] 您确定要导入%d条数据吗？（Y）\n", total);
            int ch = getch();
            if (ch != 'Y' && ch != 'y') {
                printf("[提示] 已取消导入（按任意键继续）\n");
                getch();
                goto Import_Refresh;
            }
            int t_counter = 0, t_success = 0;
            for (LinkList_Node *pt = import_list->head; pt != NULL; pt = pt->next) {
                t_counter++;
                ImportData *tmp = pt->data;
                User *user = DB_getUserByEmpId(tmp->uid);
                if (user == NULL) {
                    printf("[%d - 失败] 未找到学号为 %s 的学生，请检查学号是否正确\n", t_counter, tmp->uid);
                    continue;
                }
                int64 course_id;
                // printf("tmp->course_id: %s\n", tmp->course_id);
                course_id = strtoll(tmp->course_id, NULL, 10);
                CourseSelection *ret = DB_selectCourse(user->id, course_id);
                if (ret == NULL) {
                    printf("[%d - 失败] 校验未通过\n", t_counter);
                    continue;
                }
                t_success++;
            }
            printf("[导入成功] 共导入%d条数据（按任意键继续）\n", t_success);
            getch();
            goto Import_Refresh;
            break;
        }
        case 'I': // 选择文件导入
        case 'i': {
            printf("[提示] 请在打开的对话框中选择文件，以完成进一步的操作...\n");
            char file_path[260] = {0};
            if (openFileDialog(file_path, "TemplateFile\0*.csv\0", "请选择导入模板") == 0) {
                printf("[提示] 用户取消了文件选择（按任意键继续）\n");
                getch();
                goto Import_Refresh;
            }
            char buf[1024];
            FILE *fp = fopen(file_path, "r");
            if (fp == NULL) {
                printf("[提示] 打开文件失败（按任意键继续）\n");
                getch();
                goto Import_Refresh;
            }
            fgets(buf, sizeof(buf), fp);  // 跳过第一行
            while (fgets(buf, sizeof(buf), fp)) {
                ImportData *imp = calloc(1, sizeof(ImportData)); // 开辟内存
                buf[strlen(buf) - 1] = '\0'; // 去除行尾换行
                char *p;
                char *ptr = strtok_r(buf, ",", &p); // 字符串分割
                if (ptr == NULL) break; // 读到第一个空行或无效行
                strcpy(imp->uid, ptr);
                ptr = strtok_r(NULL, ",", &p); // 字符串分割
                strcpy(imp->course_id, ptr);
                if (regexMatch(USER_PATTERN, imp->uid) && regexMatch(COURSE_ID_PATTERN, imp->course_id)) {
                    linkListObject_Append(import_list, imp);
                    total++;
                } else {
                    free(imp);
                }
            }
            fclose(fp);
            printf("[导入成功] 共导入%d条有效数据（按任意键继续）\n", total);
            getch();
            goto Import_Refresh;
            break;
        }
        case 27:
            goto Import_GC_Collect;
        default:
            break;
    }
    goto Import_GetKey;

    Import_GC_Collect:
    linkListObject_Delete(import_list, 1);
    free(import_list);
}

#endif
