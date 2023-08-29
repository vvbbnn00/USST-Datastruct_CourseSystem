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
const char *NUMBER_CAPITAL[10] = {"��", "һ", "��", "��", "��", "��", "��", "��", "��", "��"};
const char *LECTURE_TYPE[4] = {"����", "ѡ��", "��ѡ", "����"};


// ���ڴ���һ�����������飬��ȡ����ʹ��memcpy���ݸ�course��schedule
typedef struct schedule_ {
    int schedule[7][13];
} Schedule;

struct studentCourseSelection { // ѧ��ѡ����
    int status; // �Ƿ��ѡ���ſγ�
    char locked_reason[100]; // ���ſβ���ѡ���ԭ��
    long long selection_time; // ѡ��ÿγ̵�ʱ��
    Course course; // �γ���Ϣ
};

struct teacherCourseSelection { // ѧ��ѡ�ι���ṹ��
    User student; // ѧ����Ϣ
    Course course; // �γ���Ϣ
    long long selection_time; // ѡ��ÿγ̵�ʱ��
    int64 score; // ���ſγ̵ĳɼ�
    int64 id; // ѡ��ID
};


extern void printStudentCourseSelection();

extern void printStudentLectureTable();

extern void printAllCourses(int scene);

extern void editCourse(Course *_course);

extern Schedule editSchedule(int schedule[7][13]);

extern void importStuCourseData();

void printStudentList(Course *courseData);

/**
 * ��ȡ�γ�״̬
 *
 * @param status �γ�״̬
 * @return
 */
char *__getCourseStatus(int status) {
    switch (status) {
        case 0:
            return "��ѡ";
        case 1:
            return "��ѡ";
        case 2:
            return "����";
        default:
            return "δ֪";
    }
}


/**
 * ��ӡ�γ̰���
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
            sprintf(t_str, "��%s:", NUMBER_CAPITAL[i + 1]);
            strcat(final_str, t_str);
            strcat(final_str, schedule_str);
            strcat(final_str, "��;");
        }
    }
    return final_str;
}


/**
 * ��ȡÿ����ѧʱ
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
 * ��ӡ�γ���Ϣ
 * @param selected_course
 */
void printCourseData(Course *selected_course) {
    printf("\n\t===== �γ���Ϣ��%s =====\n\n", selected_course->courseName);
    printf("\t��  ��ID��%lld\n", selected_course->id);
    printf("\t�γ����ƣ�%s\n", selected_course->courseName);
    printf("\t�γ̼�飺%s\n", selected_course->description);
    printf("\t�γ����ʣ�%s\n", LECTURE_TYPE[selected_course->type]);
    printf("\t�ڿ���������%d��~��%d��\n", selected_course->weekStart, selected_course->weekEnd);
    char *courseArrangeStr = printSchedule(selected_course->schedule);
    if (courseArrangeStr != NULL) {
        printf("\t�ڿΰ��ţ�%s\n", courseArrangeStr);
        free(courseArrangeStr);
    }
    printf("\t�ڿν�ʦ��%s(UID:%lld)\n", selected_course->teacher->name, selected_course->teacherId);
    printf("\t�γ�ѧʱ��%dѧʱ\n",
           getTotalWeekHour(selected_course->schedule) * (selected_course->weekEnd - selected_course->weekStart + 1));
    printf("\t�γ�ѧ�֣�%.2f\n", selected_course->points);
}


/**
 * ִ��ѧ��ѡ������
 */
void printStudentCourseSelection() {
    system("chcp 936>nul & cls & MODE CON COLS=110 LINES=50");
    HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    char search_kw[36] = "";
    double current_score = 0; // ��ѧ������ѡѧ��, ��ǰ��ѡѧ��
    int total, page = 1, max_page, page_size = 10, current_total;

    LinkList_Node *selectedRow = NULL; // ��ѡ�е���
    LinkList_Object *course_data_list = NULL;

    GetCourseAndDisplay:
    current_score = 0;
    // ... �������
    printf("[��ʾ] ��������...\n");

    NodeList *result;
    if (strlen(search_kw) > 0) {
        result = DB_getCoursesByName(search_kw);
    } else {
        result = DB_getAllCourses();
    }

    if (result == NULL) {
        printf("[��ʾ] �޷���ȡ�γ�����\n");
        getch();
        goto GC_Collect;
    }

    total = 0;
    for (NodeList *pt = result; pt != NULL; pt = pt->next) total++;
    max_page = (int) ceil((double) total / page_size);
    current_total = (page == max_page) ? (total - (page - 1) * page_size) : page_size;

    // �����γ�����
    course_data_list = linkListObject_Init(); // �����ʼ��
    for (NodeList *pt = result; pt != NULL; pt = pt->next) {
        Course *course_data_pt = DB_getCourseById((int64) pt->indexNode->index.data);
        if (course_data_pt == NULL) {
            continue;
        }
        struct studentCourseSelection *tmp = calloc(1, sizeof(struct studentCourseSelection));
        if (tmp == NULL) {
            printf("[��ʾ] �ڴ治��\n");
            getch();
            goto GC_Collect;
        }

        if (course_data_pt->currentMembers >= course_data_pt->maxMembers) {
            tmp->status = 2;
            strcpy(tmp->locked_reason, "�γ�����Ա");
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
    UI_printInMiddle("======= �γ̡�ѧ��ѡ�� =======\n", 104);
    printf("%-16s%-35s%-35s%-10s%-12s\n", "ѡ��״̬", "�γ�ID", "�γ�����", "�γ�ѧ��", "�ڿν�ʦ");
    printf("--------------------------------------------------------------------------------------------------------\n");
    for (LinkList_Node *pt = course_data_list->head; pt != NULL; pt = pt->next) {
        struct studentCourseSelection *tmp = pt->data;
        if (pt == selectedRow) SetConsoleTextAttribute(windowHandle, 0x70);
        else {
            switch (tmp->status) {
                case 1: // ��ѡ
                    SetConsoleTextAttribute(windowHandle, 0x0e);
                    break;
                case 2: // ����
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
        if (pt->next == NULL) { // ��������һ���ڵ�
            printf("\n");
        }
    }
    for (int i = 0; i < page_size - current_total; i++) printf("\n"); // ����ҳ��
    printf("\n");
    UI_printInMiddle("=============================\n", 104);
    printf("[��ǰ��������] ");
    if (strlen(search_kw) > 0) { printf("ģ������=%s", search_kw); } else { printf("\n"); }
    printf("  [ѡ�θſ�] ��ѡ%.2fѧ��\n\n", current_score);
    printf("[��ʾ] ��%4d�����ݣ���ǰ��%3dҳ����%3dҳ���������ǰһҳ���ҷ��������һҳ����/�·�������л�ѡ�����ݣ�\n",
           total, page, max_page);
    printf("\n\t   <Enter>ѡ��/��ѡ <K>�γ�ģ����ѯ <Esc>�������˵�\n");

    if (selectedRow == NULL) {
        UI_printErrorData("���޿�ѡ�γ�");
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
        case 1: // ��ѡ
            SetConsoleTextAttribute(windowHandle, 0x0e);
            break;
        case 2: // ����
            SetConsoleTextAttribute(windowHandle, 0x08);
            break;
        default:
            break;
    }
    printf("\t״  ̬��  %s[%d/%d��]%s%s%s%s\n",
           __getCourseStatus(selected_selection->status),
           selected_course->currentMembers,
           selected_course->maxMembers,
           strlen(selected_selection->locked_reason) > 0 ? " - ����ѡԭ��" : "",
           selected_selection->locked_reason,
           selected_selection->selection_time > 0 ? " - ѡ��ʱ�䣺" : "",
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
                case 80: // ��
                    if (selectedRow->next == NULL) selectedRow = course_data_list->head;
                    else selectedRow = selectedRow->next;
                    goto Refresh;
                case 72: // ��
                    if (selectedRow->prev == NULL) selectedRow = course_data_list->foot;
                    else selectedRow = selectedRow->prev;
                    goto Refresh;
                case 75: // ��
                    page = (page > 1) ? (page - 1) : 1;
                    selectedRow = 0;
                    goto GetCourseAndDisplay;
                case 77: // ��
                    page = (page < max_page) ? (page + 1) : (max_page);
                    selectedRow = 0;
                    goto GetCourseAndDisplay;
                default:
                    break;
            }
            break;
        case 'k': // �����ؼ���
        case 'K':
            printf("\n[ģ������]������ؼ��ʣ���\"Enter\"����������");
            gets_safe(search_kw, 35);
            fflush(stdin);
            selectedRow = 0;
            goto GetCourseAndDisplay;
        case 13: // ѡ��ѡ��
            printf("[��ʾ] ��������...\n");
            switch (selected_selection->status) {
                case 0: {
                    CourseSelection *selection = DB_selectCourse(GlobalUser->id, selected_course->id);
                    if (selection == NULL) {
                        printf("\n[ѡ��ʧ��] ѡ��ʧ�ܣ������ԣ��������������\n");
                    } else {
                        printf("\n[ѡ�γɹ�] ѡ�γɹ�������ѡ��IDΪ��%lld���������������\n", selection->id);
                    }
                    break;
                }
                case 1: {
                    CourseSelection *confirm = DB_withdrawCourse(GlobalUser->id, selected_course->id);
                    if (confirm == NULL) {
                        printf("\n[��ѡʧ��] ��ѡʧ�ܣ������ԣ��������������\n");
                    } else {
                        printf("\n[��ѡ�ɹ�] ��ѡ�ɹ����������������\n");
                    }
                    break;
                }
                default:
                    printf("\n[ѡ��ʧ��] ���޷�ѡ��ÿγ̣�%s���������������\n",
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
 * ����α��ļ����У���������̨��
 *
 * @param _stream
 * @param scheduleList
 */
void printTableToStream(FILE *_stream, LinkList_Object scheduleList[7][13]) {
    for (int week_num = 0; week_num < 7; week_num++) {
        char has_course = 0; // ��ǰ�����Ƿ��п�
        fprintf(_stream, "[����%s]\n\n", NUMBER_CAPITAL[week_num + 1]);
        for (int course_num = 1; course_num <= 12; course_num++) {
            LinkList_Node *head = scheduleList[week_num][course_num].head;
            if (head == NULL) continue; // ��ʱ��û�п�
            has_course = 1;
            for (LinkList_Node *pt = head; pt != NULL; pt = pt->next) {
                Course *c_data = pt->data;
                fprintf(_stream, "  <��");
                for (int i = course_num; i <= 12; i++) {
                    if (!c_data->schedule[week_num][i]) break;
                    fprintf(_stream, " %d", i);
                }
                fprintf(_stream, " ��> %s\n", c_data->courseName);
                fprintf(_stream, "    �ڿν�ʦ��%s���ڿ��ܣ���%d������%d�ܣ�ѧ�֣�%.2f\n",
                        c_data->teacher->name,
                        c_data->weekStart,
                        c_data->weekEnd,
                        c_data->points);
            }
        }
        if (!has_course) fprintf(_stream, "  �޿γ�\n");
        fprintf(_stream, "\n");
    }
}


/**
 * ���ѧ���ɼ�����չʾ�ڿ���̨
 */
void printStudentScoreTable() {
    system("chcp 936>nul & cls & MODE CON COLS=80 LINES=100"); // ��������һ��Ҫ�󣬲�Ȼ���ݻᱻˢ��

    int total; // �ܽ������
    double score_total; // ��ѧ��
    LinkList_Object *scheduleList = linkListObject_Init(); // ������Ŀα����ݷ��������Ӧ�ڴε���Ϣ[7]��ʾ��һ(0)������(6)��[13]��ʾ��һ�ڿ�(1)����ʮ���ڿ�(12)

    // ... �������
    printf("[��ʾ] ��������...\n");

    IndexListNode *courseSelections = DB_getSelectionsByUserId(GlobalUser->id);

    // �����γ�����
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
    UI_printInMiddle("======= �γ̡��ɼ���ѯ =======\n", 77);
    printf("%-16s%-35s%-15s%-10s\n", "�γ�ID", "�γ�����", "�γ�ѧ��", "�ɼ�");
    printf("--------------------------------------------------------------------------\n");
    for (LinkList_Node *pt = scheduleList->head; pt != NULL; pt = pt->next) {
        CourseSelection *selection = pt->data;
        Course *course_data_pt = selection->course;

        // �ɼ�����������ʾ
        char *showScore;
        if (selection->score < 0) {
            showScore = "δ¼��";
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
    printf("\n    [ѧ������] %s  [��ѡѧ��] %.2f\n",
           GlobalUser->name,
           score_total);
    printf("\n\t <Esc>�������˵�\n\n");

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
 * ���ѧ���α���չʾ�ڿ���̨
 */
void printStudentLectureTable() {
    system("chcp 936>nul & cls & MODE CON COLS=70 LINES=9001"); // ��������һ��Ҫ�󣬲�Ȼ���ݻᱻˢ��

    int total; // �ܽ������
    double score_total; // ��ѧ��
    LinkList_Object scheduleList[7][13] = {0}; // ������Ŀα����ݷ��������Ӧ�ڴε���Ϣ[7]��ʾ��һ(0)������(6)��[13]��ʾ��һ�ڿ�(1)����ʮ���ڿ�(12)

    // ... �������
    printf("[��ʾ] ��������...\n");

    IndexListNode *courseSelections = DB_getSelectionsByUserId(GlobalUser->id);

    // �����γ�����
    for (IndexListNode *n = courseSelections; n != NULL; n = n->next) {
        CourseSelection *selection = DB_getSelectionById((int64) n->index.data);
        if (selection == NULL) continue;
        Course *course_data_pt = selection->course;
        score_total += course_data_pt->points;

        for (int week = 0; week < 7; week++) {
            int continuous = 0; // �ж��Ƿ��������Ŀγ̰��ţ������ ������һ�𣬲��ظ�����ڿα���
            for (int seq = 1; seq <= 12; seq++) {
                if (course_data_pt->schedule[week][seq]) {  // ��ʱ���п�
                    if (!continuous) {
                        linkListObject_Append(&scheduleList[week][seq], course_data_pt); // ���γ̼����ȥ
                        continuous = 1; // �ظ���flag����
                    }
                } else {
                    continuous = 0; // ����һ�����ظ��ģ���flag����
                }
            }
        }
    }

    Refresh:

    system("cls");
    printf("\n");
    UI_printHeader(58);
    printf("\n");
    UI_printInMiddle("======= �γ̡���ǰѧ�ڿα� =======\n", 60);
    printTableToStream(stdout, scheduleList);
    printf("------------------------------------------------------------\n");
    printf("\n    [ѧ������] %s  [��ѡѧ��] %.2f\n",
           GlobalUser->name,
           score_total);
    printf("\n\t <E>�����α��ļ��� <Esc>�������˵�\n\n");

    int keyboard_press;

    GetKey:
    keyboard_press = _getch();
    switch (keyboard_press) {
        case 'e': // �����α�
        case 'E': {
            time_t raw_time;
            time(&raw_time);
            char *timestamp = getFormatTimeString_("%Y%m%d%H%M%S", raw_time);
            char *name = GlobalUser->name;
            unsigned long long len = strlen(timestamp) +
                                     strlen(name) +
                                     strlen("�Ŀα�_.txt") + 1;
            char file_name[len];
            memset(file_name, 0, len);
            sprintf(file_name, "%s�Ŀα�_%s.txt", name, timestamp);
            free(timestamp);
            FILE *file = fopen(file_name, "w");
            printf("[��ʾ] ���ڽ��α�������%s...\n", file_name);
            if (file == NULL) {
                printf("[����ʧ��] �޷������ļ�\"%s\"�������Ƿ��ж�дȨ�ޣ��������������\n", file_name);
                goto Refresh;
            }
            printTableToStream(file, scheduleList);
            fclose(file);
            printf("[�����ɹ�] �α��ѵ�������%s���������������\n", file_name);
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
 * ���ȫУ�α�����Ա�汾��
 * @param scene ���� 0 - �鿴ȫУ�α�����Ա�ɹ��� 1 - ��ʦ������ֻ��ʾ�Լ��̵Ŀγ̣���������
 */
void printAllCourses(int scene) {
    HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    char search_kw[36] = "";
    int total, page = 1, max_page, page_size = 15, current_total;

    LinkList_Node *selectedRow = NULL; // ��ѡ�е���
    LinkList_Object *course_data_list = NULL;

    GetCourseAndDisplay:
    system("chcp 936>nul & cls & MODE CON COLS=130 LINES=55");

    printf("[��ʾ] ��������...\n");

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

    // �����γ�����
    course_data_list = linkListObject_Init(); // �����ʼ��
    int cnt = 0;
    for (NodeList *j = result; j != NULL; j = j->next) {
        cnt += 1;
        if (cnt > (page - 1) * page_size && cnt <= (page - 1) * page_size + current_total) {
            Course *course_data_pt = DB_getCourseById((int64) j->indexNode->index.data);
            if (course_data_pt == NULL) {
                printf("[����] �޷���ȡ�γ���Ϣ���γ�ID=%lld��\n", (int64) j->indexNode->index.data);
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
    UI_printInMiddle("======= �γ̡�ȫУ�α�һ�� =======\n", 122);
    printf("%-35s%-35s%-10s%-10s%-12s\n", "�γ�ID", "�γ�����", "�γ�ѧ��", "�γ�����", "�ڿν�ʦ");
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
        if (pt->next == NULL) { // ��������һ���ڵ�
            printf("\n");
        }
    }
    for (int i = 0; i < page_size - current_total; i++) printf("\n"); // ����ҳ��
    printf("\n");
    UI_printInMiddle("=============================\n", 122);
    printf("[��ǰ��������] ");
    if (strlen(search_kw) > 0) { printf("ģ������=%s\n", search_kw); } else { printf("\n"); }
    printf("[��ʾ] ��%4d�����ݣ���ǰ��%3dҳ����%3dҳ���������ǰһҳ���ҷ��������һҳ����/�·�������л�ѡ�����ݣ�\n",
           total, page, max_page);
    printf("\n");
    if (GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1)) { // ����Ա�ɱ༭�γ�
        printf("<A>����/�½��γ� <Enter>�༭�γ� <P>�鿴ѧ������ <D>ɾ���γ�");
    } else {
        printf("\t  ");
    }
    printf(" <K>�γ�ģ����ѯ");
    printf(" <Esc>�������˵�\n");

    if (selectedRow == NULL) {
        if (strlen(search_kw)) {
            UI_printErrorData("û�в�ѯ�����������Ŀγ�");
            strcpy(search_kw, "");
            goto GetCourseAndDisplay;
        } else {
            if ((GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1))) { // ����Ա�ɱ༭�γ�
                printf("���޿γ̣��Ƿ����������γ̣�(Y)");
                int ch = getch();
                if (ch == 'Y' || ch == 'y') {
                    editCourse(NULL);
                }
            } else {
                UI_printErrorData("���޿γ�");
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
                case 80: // ��
                    if (selectedRow->next == NULL) selectedRow = course_data_list->head;
                    else selectedRow = selectedRow->next;
                    goto Refresh;
                case 72: // ��
                    if (selectedRow->prev == NULL) selectedRow = course_data_list->foot;
                    else selectedRow = selectedRow->prev;
                    goto Refresh;
                case 75: // ��
                    page = (page > 1) ? (page - 1) : 1;
                    selectedRow = 0;
                    goto GetCourseAndDisplay;
                case 77: // ��
                    page = (page < max_page) ? (page + 1) : (max_page);
                    selectedRow = 0;
                    goto GetCourseAndDisplay;
                default:
                    break;
            }
            break;
        case 'k': // �����ؼ���
        case 'K':
            printf("\n[ģ������]������ؼ��ʣ���\"Enter\"����������");
            gets_safe(search_kw, 35);
            fflush(stdin);
            selectedRow = 0;
            goto GetCourseAndDisplay;
        case 'A':
        case 'a':
            if (!(GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1))) break; // Ȩ���ж�
            editCourse(NULL);
            goto GetCourseAndDisplay;
        case 'D':
        case 'd': // ɾ���γ�
            if (!(GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1))) break; // Ȩ���ж�
            {
                Course *pt = selectedRow->data;
                printf("\n[ȷ��ɾ��] ��ȷ��Ҫɾ���γ� %s(%lld) �𣿣�ɾ���γ̺󣬸ÿγ����е�ѧ��ѡ�μ�¼������գ���Ҫ������������ÿγ�ID:%lldȷ�ϣ�\n",
                       pt->courseName,
                       pt->id,
                       pt->id);
                char input_char[33];
                gets_safe(input_char, 32);
                int64 input_int = strtoll(input_char, NULL, 10);
                if (input_int != pt->id) {
                    printf("�γ�ID��һ�£���ȡ�����������������������\n");
                    getch();
                    goto GetCourseAndDisplay;
                }

                DB_deleteCourse(pt->id); // ɾ���γ�

                printf("[ɾ���ɹ�] �γ� %s(%lld) �����ѡ�μ�¼�ѱ�ɾ�������������������\n", pt->courseName,
                       pt->id);
                getch();
                goto GetCourseAndDisplay;
            }
        case 'p':
        case 'P':
            if (!(GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1))) break; // Ȩ���ж�
            printStudentList((Course *) selectedRow->data);
            goto GetCourseAndDisplay;
        case 13: // �༭�γ�
            if (!(GlobalUser->role == 2 || (GlobalUser->role == 1 && scene == 1))) break; // Ȩ���ж�
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
 * ����ѧ���б�
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
                             strlen("()ѧ������_.csv") + 10;
    char file_name[len];
    memset(file_name, 0, len);
    sprintf(file_name, "%s(%lld)ѧ������_%s.csv", title, course->id, timestamp);
    free(timestamp);
    FILE *file = fopen(file_name, "w");
    printf("[��ʾ] ���ڽ�ѧ��������������%s...\n", file_name);
    if (file == NULL) {
        printf("[����ʧ��] �޷������ļ�\"%s\"�������Ƿ��ж�дȨ�ޣ��������������\n", file_name);
        return -1;
    }
    fprintf(file, "���,����,ѧ��,ѡ��ʱ��,�ɼ�,��ϵ��ʽ\n");
    int counter = 1;
    for (LinkList_Node *pt = linkList->head; pt != NULL; pt = pt->next) {
        struct teacherCourseSelection *p = pt->data;

        // �ɼ�����������ʾ
        char *showScore;
        if (p->score < 0) {
            showScore = "δ¼��";
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
    printf("[�����ɹ�] ѧ�������ѵ�������%s���������������\n", file_name);
    return 0;
}


/**
 * ��ʦ/����Ա��ӡѧ������
 */
void printStudentList(Course *courseData) {
    system("chcp 936>nul & cls & MODE CON COLS=100 LINES=55");
    HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    int sort_method = 0; // ���򷽷� 0 - Ĭ������ 1 - ѧ�ֽ��� 2 - ѧ������
    int total, page = 1, page_size = 30, max_page;
    int stat_a90 = 0, stat_a80 = 0, stat_a60 = 0, stat_a0 = 0; // ͳ������

    LinkList_Object *student_list = NULL;
    LinkList_Node *selectedRow = NULL;

    Teacher_GetCourseAndDisplay:
    printf("[��ʾ] ��������...\n");

    IndexListNode *result = DB_getSelectionsByCourseId(courseData->id);
    total = 0;

    stat_a90 = 0, stat_a80 = 0, stat_a60 = 0, stat_a0 = 0;
    for (IndexListNode *i = result; i != NULL; i = i->next) {
        total++;
    }

    max_page = (int) ceilf((float) total / (float) page_size);

    // ������������
    student_list = linkListObject_Init(); // �����ʼ��

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
                printf("[����] �޷���ȡѧ��ѡ����Ϣ��ѡ��ID=%lld��\n", (int64) j->index.data);
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
    UI_printInMiddle("======= �γ̡��γ����� =======\n", 101);
    printf("%-6s%-15s%-20s%-30s%-10s%-15s\n", "���", "����", "ѧ��", "ѡ��ʱ��", "�ɼ�", "��ϵ��ʽ");
    printf("------------------------------------------------------------------------------------------------\n");
    int counter = 0, current_total = 0;
    for (LinkList_Node *pt = student_list->head; pt != NULL; pt = pt->next) {
        counter++;
        if (ceilf((float) counter / (float) page_size) < (float) page) continue; // ��λ����ҳ�棬ѧ��������ҳ��ʾ
        if (ceilf((float) counter / (float) page_size) > (float) page) break; // һҳ��ʾ�꼴������ѭ��
        current_total++;
        if (selectedRow == NULL) selectedRow = pt;
        struct teacherCourseSelection *tmp = pt->data;
        if (pt == selectedRow) SetConsoleTextAttribute(windowHandle, 0x70);

        char *showScore;
        if (tmp->score < 0) {
            showScore = "δ¼��";
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
        if (pt->next == NULL) { // ��������һ���ڵ�
            printf("\n");
        }
    }
    for (int i = 0; i < page_size - current_total - 1; i++) printf("\n"); // ����ҳ��
    printf("\n");
    UI_printInMiddle("=============================\n", 71);
    printf("    [�γ�����] %s [�γ�ID] %lld\n\n", courseData->courseName, courseData->id);
    printf("    [��ʾ] ��%4d�����ݣ���ǰ��%3dҳ����%3dҳ\n\n    ���������ǰһҳ���ҷ��������һҳ����/�·�������л�ѡ�����ݣ�\n",
           total, page, max_page);
    printf("\n  ");
    if (GlobalUser->role == 2) { // ����Ա�ɱ༭�γ�
        printf("<A>���ѧ�� <D>ȡ����ѧ��ѡ�� <I>��������ѧ��");
    } else {
        printf("\t");
    }
    printf(" <S>¼���ѧ���ɼ� <G>ͳ�Ƴɼ��ֶ���Ϣ <E>����ѧ������ <Esc>�������˵�\n");

    if (selectedRow == NULL) {
        UI_printErrorData("��������");
        if (GlobalUser->role == 2) { // ����Ա�ɱ༭�γ�
            printf("���ǹ���Ա���Ƿ���ѧ��������(Y)");
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
                case 80: // ��
                    if (selectedRow->next == NULL) selectedRow = student_list->head;
                    else selectedRow = selectedRow->next;
                    goto Teacher_Refresh;
                case 72: // ��
                    if (selectedRow->prev == NULL) selectedRow = student_list->foot;
                    else selectedRow = selectedRow->prev;
                    goto Teacher_Refresh;
                case 75: // ��
                    page = (page > 1) ? (page - 1) : 1;
                    selectedRow = 0;
                    goto Teacher_Refresh;
                case 77: // ��
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
        case 'i': // ��������ѧ����Ϣ
            if (GlobalUser->role != 2) break; // �޷�����ѧ��
            importStuCourseData();
            goto Teacher_GetCourseAndDisplay;
        case 'G':
        case 'g': // ��ʾ�ֶη�����Ϣ
        {
            stat_a90 = 0, stat_a80 = 0, stat_a60 = 0, stat_a0 = 0;
            for (IndexListNode *i = result; i != NULL; i = i->next) {
                CourseSelection *courseSelection = DB_getSelectionById((int64) i->index.data);

                if (courseSelection->score >= 90) stat_a90++;
                else if (courseSelection->score >= 80) stat_a80++;
                else if (courseSelection->score >= 60) stat_a60++;
                else stat_a0++;
            }
            printf("\n[�ɼ��ֶ�ͳ��] 90�����ϣ�%d�ˣ�80-89�֣�%d�ˣ�60-79�֣�%d�ˣ�60�����£�%d��\n", stat_a90, stat_a80, stat_a60,
                   stat_a0);
            getch();
            goto Teacher_Refresh;
        }
        case 'S':
        case 's': // ¼��ѧ���ɼ�
        {
            int score = -1;
            UI_inputIntWithRegexCheck("[�޸Ŀγ�] ��¼��ѧ���ɼ���0-100�֣�\n",
                                      POINTS_PATTERN,
                                      &score);
            if (score < 0 || score > 100) {
                printf("[¼��ʧ��] ��Ч�ĳɼ����������������\n");
                getch();
                goto Teacher_Refresh;
            }
            struct teacherCourseSelection *pt = selectedRow->data;

            if (DB_updateSelectionScore(pt->id, score)) {
                printf("[¼��ɹ�] �ɼ���¼�루�������������\n");
            } else {
                printf("[¼��ʧ��] �ɼ�¼��ʧ�ܣ��������������\n");
            }
            getch();
            goto Teacher_Refresh;
        }
        case 'A':
        case 'a': // ����ѡ��ѧ��
            if (GlobalUser->role != 2) break; // �޷�����ѧ��
            {
                char user_id[31] = {0};
                if (UI_inputStringWithRegexCheck(
                        "[�γ�����ѧ��] ������ѧ��ѧ�ţ�������������ѧ����Ϣ����ʹ�á���������ѧ�������ܣ�\n",
                        USER_PATTERN,
                        user_id,
                        30) == -1)
                    goto Teacher_Refresh;
                User *user = DB_getUserByEmpId(user_id);

                if (user == NULL) {
                    printf("[����ʧ��] �û� %s �����ڣ��������������\n", user_id);
                    getch();
                    goto Teacher_Refresh;
                }

                CourseSelection *selection = DB_selectCourse(user->id, courseData->id);

                if (selection == NULL) {
                    printf("[����ʧ��] У��ʧ�ܣ��������������\n");
                    getch();
                    goto Teacher_Refresh;
                }

                printf("[�����ɹ�] �û� %s(%s) �ѳɹ�ѡ�Σ��������������\n", user->name, user->empId);

                getch();
                goto Teacher_GetCourseAndDisplay;
            }
            break;
        case 'D':
        case 'd': // ��ѡ�γ�
            if (GlobalUser->role != 2) break;
            {
                struct teacherCourseSelection *pt = selectedRow->data;
                printf("\n[ȷ����ѡ] ��ȷ��Ҫ��ѡѧ�� %s(%s) �𣿣��������ѧ����ѧ��:%sȷ�ϣ�\n",
                       pt->student.name,
                       pt->student.empId,
                       pt->student.empId);
                char input_char[31];
                gets_safe(input_char, 30);
                if (strcmp(input_char, pt->student.empId) != 0) {
                    printf("ѧ�Ų�һ�£���ȡ�����������������������\n");
                    getch();
                    goto Teacher_GetCourseAndDisplay;
                }

                User *user = DB_getUserByEmpId(input_char);
                if (user == NULL) {
                    printf("ѧ�Ų����ڣ���ȡ�����������������������\n");
                    getch();
                    goto Teacher_GetCourseAndDisplay;
                }

                CourseSelection *ret = DB_withdrawCourse(user->id, courseData->id); // ɾ��ѡ��
                if (ret == NULL) {
                    printf("[��ѡʧ��] ��ѡʧ�ܣ��������������\n");
                    getch();
                    goto Teacher_GetCourseAndDisplay;
                }

                printf("[��ѡ�ɹ�] �û� %s(%s) �ѳɹ���ѡ���������������\n", user->name, user->empId);
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
 * ���ؼ����޸��Ͽΰ���
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
    UI_printInMiddle("\n======= �γ̡��༭�ڿΰ��� =======\n", 52);
    printf("-------------------------------------------------------\n");
    printf("   �ڴ�     ��һ  �ܶ�  ����  ����  ����  ����  ����\n");
    printf("-------------------------------------------------------\n");
    for (int i = 1; i <= 12; i++) {
        printf("  ��%2d�ڿ� ", i);
        for (int j = 0; j < 7; j++) {
            if (x == i && y == j) SetConsoleTextAttribute(windowHandle, 0x70);
            printf("  %2s  ", returnSchedule.schedule[j][i] == 1 ? "��" : " ");
            SetConsoleTextAttribute(windowHandle, 0x07);
        }
        if (i == 5 || i == 9) printf("\n"); // ��ʱ��
        printf("\n");
    }

    printf("\n\n<Enter>�л�ѡ��״̬ <Y>�����༭ <ESC>ȡ���༭ <��/��/��/��>�л�\n");

    int key;
    ScheduleEditor_GetKey:
    key = _getch();
    switch (key) {
        case 224:
            key = _getch();
            switch (key) {
                case 80: // ��
                    if (x < 12) x++;
                    else x = 1;
                    goto ScheduleEditor_Refresh;
                case 72: // ��
                    if (x > 1) x--;
                    else x = 12;
                    goto ScheduleEditor_Refresh;
                case 75: // ��
                    if (y > 0) y--;
                    else y = 6;
                    goto ScheduleEditor_Refresh;
                case 77: // ��
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
 * �����γ�
 * @return ȡ���򷵻�NULL�����򷵻ؿγ�ָ��
 */
Course *addCourse() {
    system("chcp 936>nul & cls & MODE CON COLS=75 LINES=55");
    printf("\n");
    UI_printHeader(69);
    printf("\n");
    UI_printInMiddle("======= �γ̡�����/�޸Ŀγ� =======\n", 71);

    Course *course = calloc(1, sizeof(Course));
    if (GlobalUser->role == 2) { // ����Ա������ָ����ʦ�û�
        char *empId = alloca(sizeof(char[31]));
        if (UI_inputStringWithRegexCheck("[�����γ�] �����뿪�ν�ʦ���ţ��û���ɫ��Ϊ��ʦ��\n",
                                         USER_PATTERN,
                                         empId, 30) == -1)
            goto CancelAdd;
        User *teacher = DB_getUserByEmpId(empId);
        if (teacher == NULL) {
            printf("[�����γ�] δ�ҵ�����Ϊ %s �Ľ�ʦ�����鹤���Ƿ���ȷ���������������\n", empId);
            getch();
            goto CancelAdd;
        }
        course->teacher = teacher;
        course->teacherId = teacher->id;
    } else {
        course->teacher = GlobalUser;
        course->teacherId = GlobalUser->id;
    }
    if (UI_inputStringWithRegexCheck("[�����γ�] ������γ̱��⣨����Ϊ5-50�ַ�������ĸ�����֡����ĺ�()-��ɵ��ַ�����\n",
                                     COURSE_TITLE_PATTERN,
                                     course->courseName,
                                     100) == -1)
        goto CancelAdd;
    if (UI_inputStringWithRegexCheck("[�����γ�] ������γ�����������Ϊ5-250�ַ���\n",
                                     COURSE_DESCRIPTION_PATTERN,
                                     course->description,
                                     500) == -1)
        goto CancelAdd;
    if (UI_inputStringWithRegexCheck("[�����γ�] �������Ͽεص㣨����Ϊ5-50�ַ�������ĸ�����֡����ĺ�()-��ɵ��ַ�����\n",
                                     COURSE_TITLE_PATTERN,
                                     course->location,
                                     100) == -1)
        goto CancelAdd;
    if (UI_inputIntWithRegexCheck("[�����γ�] ������γ����ͣ�0-���ޡ�1-ѡ�ޡ�2-��ѡ��3-����\n",
                                  COURSE_TYPE_PATTERN,
                                  &course->type))
        goto CancelAdd;
    if (UI_inputIntWithRegexCheck("[�����γ�] ������γ̿�ʼ����������0��������\n",
                                  NUMBER_PATTERN,
                                  &course->weekStart))
        goto CancelAdd;
    if (UI_inputIntWithRegexCheck("[�����γ�] ������γ̽�������������0��������\n",
                                  NUMBER_PATTERN,
                                  &course->weekEnd))
        goto CancelAdd;
    if (UI_inputFloatWithRegexCheck("[�����γ�] ������γ�ѧ�֣�����0��С������ྫȷ��С�����2λ��\n",
                                    POINTS_PATTERN,
                                    &course->points))
        goto CancelAdd;
    if (UI_inputIntWithRegexCheck("[�����γ�] ���������ѡ������������0��������\n",
                                  POINTS_PATTERN,
                                  &course->maxMembers))
        goto CancelAdd;
    printf("[�����γ�] ���������ʼ�༭�ڿΰ���\n");
    getch();
    Schedule ret = editSchedule(NULL);
    memcpy(course->schedule, ret.schedule, sizeof(int[7][13]));

    goto Return;

    CancelAdd:  // ȡ�����
    free(course);
    course = NULL;

    Return:  // ��������
    return course;
}


/**
 * ����/�޸Ŀγ�
 *
 * @param _course ��ΪNULLʱ�������γ̣�����Ϊ�޸Ŀγ�
 * @return �γ�ָ��
 */
void editCourse(Course *_course) {
    // ���޿γ���Ϣ���������γ�
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
    UI_printInMiddle("======= �γ̡�����/�޸Ŀγ� =======\n\n", 71);

    UI_selfPlusPrint("\t\t�γ�ID        %lld\n", &counter, selected, course->id); // 0
    UI_selfPlusPrint("\t\t�γ�����      %s\n", &counter, selected, course->courseName); // 1
    UI_selfPlusPrint("\t\t�γ̼��      %s\n", &counter, selected, course->description); // 2
    UI_selfPlusPrint("\t\t�Ͽεص�      %s\n", &counter, selected, course->location); // 3
    UI_selfPlusPrint("\t\t�γ�����      %s\n", &counter, selected, LECTURE_TYPE[course->type]); // 4
    UI_selfPlusPrint("\t\t�ڿ�����      ��%d��~��%d��\n", &counter, selected, course->weekStart, course->weekEnd); // 5
    char *courseArrangeStr = printSchedule(course->schedule); // 6
    if (courseArrangeStr != NULL) {
        UI_selfPlusPrint("\t\t�ڿΰ���      %s\n", &counter, selected, courseArrangeStr);
        free(courseArrangeStr);
    }
    UI_selfPlusPrint("\t\t�ڿν�ʦ      %s(UID:%lld)\n", &counter, selected, course->teacher->name, // 7
                     course->teacherId);
    UI_selfPlusPrint("\t\tѡ������      %d/%d��\n", &counter, selected, course->currentMembers,
                     course->maxMembers);  // 8
    UI_selfPlusPrint("\t\t�γ�ѧ��      %.2f\n", &counter, selected, course->points);  // 9
    UI_selfPlusPrint("\t\t�γ�ѧʱ      %dѧʱ\n", &counter, selected, // 10
                     getTotalWeekHour(course->schedule) * (course->weekEnd - course->weekStart + 1));

    printf("\n");
    UI_printInMiddle("<Enter>�޸�ѡ���� <Y>�ύ�޸� <Esc>ȡ���޸�", 71);

    EditCourse_GetKey:

    key = _getch();
    switch (key) {
        case 224:
            key = _getch();
            switch (key) {
                case 80: // ��
                    if (selected < 10) selected++;
                    else selected = 0;
                    goto EditCourse_Refresh;
                case 72: // ��
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
                            "[�޸Ŀγ�] ������γ̱��⣨����Ϊ5-50�ַ�������ĸ�����֡����ĺ�()-��ɵ��ַ�����\n",
                            COURSE_TITLE_PATTERN,
                            course->courseName,
                            100);
                    break;
                case 2:
                    UI_inputStringWithRegexCheck("[�޸Ŀγ�] ������γ�����������Ϊ5-250�ַ���\n",
                                                 COURSE_DESCRIPTION_PATTERN,
                                                 course->description,
                                                 500);
                    break;
                case 3:
                    UI_inputStringWithRegexCheck(
                            "[�޸Ŀγ�] �������Ͽεص㣨����Ϊ5-50�ַ�������ĸ�����֡����ĺ�()-��ɵ��ַ�����\n",
                            COURSE_TITLE_PATTERN,
                            course->location,
                            100);
                    break;
                case 4:
                    UI_inputIntWithRegexCheck("[�޸Ŀγ�] ������γ����ͣ�0-���ޡ�1-ѡ�ޡ�2-��ѡ��3-����\n",
                                              COURSE_TYPE_PATTERN,
                                              &course->type);
                    break;
                case 5:
                    if (UI_inputIntWithRegexCheck("[�޸Ŀγ�] ������γ̿�ʼ����������0��������\n",
                                                  NUMBER_PATTERN,
                                                  &course->weekStart) == -1)
                        break;
                    UI_inputIntWithRegexCheck("[�޸Ŀγ�] ������γ̽�������������0��������\n",
                                              NUMBER_PATTERN,
                                              &course->weekEnd);
                    break;
                case 6: {
                    Schedule ret = editSchedule(course->schedule);
                    memcpy(course->schedule, ret.schedule, sizeof(int[7][13]));
                }
                    break;
                case 8:
                    UI_inputIntWithRegexCheck("[�޸Ŀγ�] ���������ѡ������������0��������\n",
                                              POINTS_PATTERN,
                                              &course->maxMembers);
                    break;
                case 9:
                    UI_inputFloatWithRegexCheck("[�޸Ŀγ�] ������γ�ѧ�֣�����0��С������ྫȷ��С�����2λ��\n",
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
                        printf("[�ύʧ��] �γ��޸�/����ʧ�ܣ��������������\n");
                        getch();
                        goto EditCourse_Refresh;
                    }
                }

                printf("[�ύ�ɹ�] �γ��޸�/�����ɹ�������������ؿγ�һ����\n");
                getch();
                goto GC_COLLECT;
            } else {
                printf("[�ύʧ��] ������ӦС�ڵ��ڽ����ܣ��ҿ����ܡ�ѧ�ֺ��������Ӧ����0��(�����������)\n");
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
 * ����ѧ��ѡ���б�
 */
void importStuCourseData() {

    typedef struct _import_data {
        char uid[21], course_id[33];
    } ImportData;

    LinkList_Object *import_list = linkListObject_Init(); // ��ʼ������
    int page = 1, max_page, page_size = 30, total = 0, key;

    Import_Refresh:

    max_page = (int) ceilf((float) total / (float) page_size);
    system("chcp 936>nul & cls & MODE CON COLS=75 LINES=55");
    printf("\n");
    UI_printHeader(54);
    printf("\n");
    UI_printInMiddle("======= �γ̡�����ѧ������ =======\n\n", 56);
    printf("%-6s%-20s%-30s\n", "���", "ѧ��", "ѡ�޿γ�ID");
    printf("--------------------------------------------------------\n");
    int counter = 0, current_total = 0;
    for (LinkList_Node *pt = import_list->head; pt != NULL; pt = pt->next) {
        counter++;
        if (ceilf((float) counter / (float) page_size) < (float) page) continue; // ��λ����ҳ�棬ѧ��������ҳ��ʾ
        if (ceilf((float) counter / (float) page_size) > (float) page) break; // һҳ��ʾ�꼴������ѭ��
        current_total++;
        ImportData *tmp = pt->data;
        printf("%4d  %-20s%-30s\n",
               counter,
               tmp->uid,
               tmp->course_id);
        if (pt->next == NULL) { // ��������һ���ڵ�
            printf("\n");
        }
    }
    for (int i = 0; i < page_size - current_total - 1; i++) printf("\n"); // ����ҳ��
    printf("\n");
    UI_printInMiddle("=============================\n", 56);
    printf("    [��ʾ] ��%4d�����ݣ���ǰ��%3dҳ����%3dҳ\n\n    ���������ǰһҳ���ҷ��������һҳ��\n\n",
           total, page, max_page);
    UI_printInMiddle("<T>����ģ�� <I>ѡ���ļ� <Y>ȷ�ϵ��� <Esc>ȡ��������", 56);
    printf("\n");

    Import_GetKey:

    key = _getch();
    switch (key) {
        case 224:
            key = _getch();
            switch (key) {
                case 75: // ��
                    if (page > 1) page--;
                    else break;
                    goto Import_Refresh;
                case 77: // ��
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
                printf("[��ʾ] ����ģ��ʧ�ܣ������ļ�Ȩ�ޡ����������������\n");
                getch();
                goto Import_Refresh;
            }
            fprintf(fp, "ѧ��,�γ�ID\n2135060620(ʾ������ɾ��),2022-01-0000001-01(ʾ������ɾ��)\n");
            fclose(fp);
            printf("[��ʾ] ����ģ��ɹ���ģ�屣����\"import_template.csv\"�ļ��С����������������\n");
            goto Import_Refresh;
        }
        case 'Y':
        case 'y': { // ��������
            if (total == 0) {
                printf("[����ѧ��ʧ��] �б�Ϊ�գ����Ȱ�\"I\"�������ݣ��������������\n");
                getch();
                goto Import_Refresh;
            }
            printf("[��ʾ] ��ȷ��Ҫ����%d�������𣿣�Y��\n", total);
            int ch = getch();
            if (ch != 'Y' && ch != 'y') {
                printf("[��ʾ] ��ȡ�����루�������������\n");
                getch();
                goto Import_Refresh;
            }
            int t_counter = 0, t_success = 0;
            for (LinkList_Node *pt = import_list->head; pt != NULL; pt = pt->next) {
                t_counter++;
                ImportData *tmp = pt->data;
                User *user = DB_getUserByEmpId(tmp->uid);
                if (user == NULL) {
                    printf("[%d - ʧ��] δ�ҵ�ѧ��Ϊ %s ��ѧ��������ѧ���Ƿ���ȷ\n", t_counter, tmp->uid);
                    continue;
                }
                int64 course_id;
                // printf("tmp->course_id: %s\n", tmp->course_id);
                course_id = strtoll(tmp->course_id, NULL, 10);
                CourseSelection *ret = DB_selectCourse(user->id, course_id);
                if (ret == NULL) {
                    printf("[%d - ʧ��] У��δͨ��\n", t_counter);
                    continue;
                }
                t_success++;
            }
            printf("[����ɹ�] ������%d�����ݣ��������������\n", t_success);
            getch();
            goto Import_Refresh;
            break;
        }
        case 'I': // ѡ���ļ�����
        case 'i': {
            printf("[��ʾ] ���ڴ򿪵ĶԻ�����ѡ���ļ�������ɽ�һ���Ĳ���...\n");
            char file_path[260] = {0};
            if (openFileDialog(file_path, "TemplateFile\0*.csv\0", "��ѡ����ģ��") == 0) {
                printf("[��ʾ] �û�ȡ�����ļ�ѡ�񣨰������������\n");
                getch();
                goto Import_Refresh;
            }
            char buf[1024];
            FILE *fp = fopen(file_path, "r");
            if (fp == NULL) {
                printf("[��ʾ] ���ļ�ʧ�ܣ��������������\n");
                getch();
                goto Import_Refresh;
            }
            fgets(buf, sizeof(buf), fp);  // ������һ��
            while (fgets(buf, sizeof(buf), fp)) {
                ImportData *imp = calloc(1, sizeof(ImportData)); // �����ڴ�
                buf[strlen(buf) - 1] = '\0'; // ȥ����β����
                char *p;
                char *ptr = strtok_r(buf, ",", &p); // �ַ����ָ�
                if (ptr == NULL) break; // ������һ�����л���Ч��
                strcpy(imp->uid, ptr);
                ptr = strtok_r(NULL, ",", &p); // �ַ����ָ�
                strcpy(imp->course_id, ptr);
                if (regexMatch(USER_PATTERN, imp->uid) && regexMatch(COURSE_ID_PATTERN, imp->course_id)) {
                    linkListObject_Append(import_list, imp);
                    total++;
                } else {
                    free(imp);
                }
            }
            fclose(fp);
            printf("[����ɹ�] ������%d����Ч���ݣ��������������\n", total);
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
