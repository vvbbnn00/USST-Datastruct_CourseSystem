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
    char *username = calloc(21, sizeof(char)); // �û����20λ
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
    char *password = calloc(33, sizeof(char)); //�洢����
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
    system("TITLE �γ̹���ϵͳ - ��¼");

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
    safe_free(&password);

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
 * ִ���޸��������
 */
void Serv_changePassword() {
    char *ori_password, *new_password, *new_password_repeat;

    Init:

    system("cls");
    system("@echo off");
    system("MODE CON COLS=55 LINES=30");

    UI_printHeader(55);

    printf("\n---- �޸����� - ����\"esc\"�������������Է��أ� ----\n\n");
    printf("�û�����%s\n", GlobalUser->empId);
    printf("������ԭ���루��\"esc\"�������������Է��أ���\n");
    ori_password = Serv_User_getPassword(0);
    if (strlen(ori_password) == 0) {
        return;
    }
    // ���ñ�ǵ㣬�����벻���Ϲ��������ش˴�
    EnterNewPassword:
    printf("�����������룺\n");
    printf("[����ǿ����ʾ] ����������ͬʱ������ĸ�����֡��ַ��е����ַ��ţ�������8-20λ��\n");
    new_password = Serv_User_getPassword(0);
    if (strlen(new_password) == 0) {
        return;
    }
    if (!regexMatch(PASSWD_PATTERN, new_password)) {
        printf("����ǿ�Ȳ����Ϲ������������롣\n\n");
        goto EnterNewPassword;
    }
    printf("���ٴ����������룺\n");
    new_password_repeat = Serv_User_getPassword(0);
    if (strcmp(new_password_repeat, new_password) != 0) {
        printf("�����������벻��ͬ�����������롣\n\n");
        goto EnterNewPassword;
    }

    char *hashPasswd = calcHexHMACSHA256(ori_password, SECRET_KEY);
    char *hashNewPasswd = calcHexHMACSHA256(new_password, SECRET_KEY);
    safe_free(&ori_password);
    safe_free(&new_password);
    safe_free(&new_password_repeat);
    if (strcmp(hashPasswd, GlobalUser->passwd) != 0) {
        printf("[ϵͳ��ʾ] �޸�����ʧ�ܣ�ԭ���벻��ȷ����������������룩\n");
        _getch();
        goto Init;
    }

    if (strcmp(hashPasswd, hashNewPasswd) == 0) {
        printf("[ϵͳ��ʾ] �޸�����ʧ�ܣ���������ԭ������ͬ����������������룩\n");
        _getch();
        goto Init;
    }

    safe_free(&hashPasswd);
    strcpy(GlobalUser->passwd, hashNewPasswd);
    DB_updateUser(GlobalUser);

    printf("[ϵͳ��ʾ] �����޸ĳɹ�����������������˵�����\n");
    _getch();
}


/**
 * �����û�
 * @return ȡ���򷵻�NULL�����򷵻��û�ָ��
 */
User *addUser() {
    system("chcp 936>nul & cls & MODE CON COLS=75 LINES=55");
    printf("\n");
    UI_printHeader(69);
    printf("\n");
    UI_printInMiddle("======= �û��������û� =======\n", 71);

    User *user = calloc(1, sizeof(User));
    if (UI_inputStringWithRegexCheck("[�����û�] �������û�ѧ�ţ���3-15λ����ĸ��������ɣ�\n",
                                     USER_PATTERN,
                                     user->empId,
                                     15) == -1)
        goto CancelAdd;
    if (UI_inputStringWithRegexCheck(
            "[�����û�] �������û���ʼ���루��8-20Ϊ��ĸ�����֡��ַ���ɣ�������������ͬʱ�������֣�\n",
            PASSWD_PATTERN,
            user->passwd,
            20) == -1)
        goto CancelAdd;
    if (UI_inputStringWithRegexCheck("[�����û�] �������û���������2-20λ�����ġ�Ӣ�ĵ���ɣ�\n",
                                     USER_NAME_PATTERN,
                                     user->name,
                                     20) == -1)
        goto CancelAdd;
    if (UI_inputStringWithRegexCheck("[�����û�] ��������ϵ��ʽ����2-20λ�����ġ�Ӣ�ĵ���ɣ�\n",
                                     USER_NAME_PATTERN,
                                     user->contact,
                                     20) == -1)
        goto CancelAdd;
    strcpy(user->passwd, calcHexHMACSHA256(user->passwd, SECRET_KEY));
    if (UI_inputIntWithRegexCheck("[�����û�] �������û���ɫ��0-ѧ����1-��ʦ��2-����Ա\n",
                                  USER_ROLE_PATTERN,
                                  &user->role) == -1)
        goto CancelAdd;
    goto Return;

    CancelAdd:  // ȡ�����
    safe_free(&user);
    user = NULL;

    Return:  // ��������
    return user;
}


/**
 * ����/�޸��û�
 *
 * @param _user ��ΪNULLʱ�������û�������Ϊ�޸��û�
 * @return
 */
void editUser(User *_user) {
    // �����û���Ϣ���������û�
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
    UI_printInMiddle("======= �û�������/�޸��û� =======\n\n", 71);

    UI_selfPlusPrint("\t\t�û�UID       %lld\n", &counter, selected, user->id); // 0
    UI_selfPlusPrint("\t\t�û�ѧ��      %s\n", &counter, selected, user->empId); // 1
    UI_selfPlusPrint("\t\t�û�����      %s\n", &counter, selected, user->name); // 2
    UI_selfPlusPrint("\t\t�û���ɫ      %s\n", &counter, selected, Serv_User_getUserRole(user->role)); // 3
    UI_selfPlusPrint("\t\t����¼ʱ��  %s\n", &counter, selected, getFormatTimeString(user->lastLoginTime)); // 4
    UI_selfPlusPrint("\t\t�û�����      [������]\n", &counter, selected); // 5
    UI_selfPlusPrint("\t\t��ϵ��ʽ      %s\n", &counter, selected, user->contact); // 6

    printf("\n");
    UI_printInMiddle("<Enter>�޸�ѡ���� <Y>�ύ�޸� <Esc>ȡ���޸�", 71);

    EditCourse_GetKey:

    key = _getch();
    switch (key) {
        case 224:
            key = _getch();
            switch (key) {
                case 80: // ��
                    if (selected < 6) selected++;
                    else selected = 0;
                    goto EditUser_Refresh;
                case 72: // ��
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
                    UI_inputStringWithRegexCheck("[�޸��û�] �������û���������2-20λ�����ġ�Ӣ�ĵ���ɣ�\n",
                                                 USER_NAME_PATTERN,
                                                 user->name,
                                                 20);
                    break;
                case 3:
                    UI_inputIntWithRegexCheck("[�޸��û�] �������û���ɫ��0-ѧ����1-��ʦ��2-����Ա\n",
                                              USER_ROLE_PATTERN,
                                              &user->role);
                    break;
                case 5:
                    UI_inputStringWithRegexCheck(
                            "[�޸��û�] �������û����루��8-20Ϊ��ĸ�����֡��ַ���ɣ�������������ͬʱ�������֣�\n",
                            PASSWD_PATTERN,
                            user->passwd,
                            20);
                    strcpy(user->passwd, calcHexHMACSHA256(user->passwd, SECRET_KEY));
                    break;
                case 6:
                    UI_inputStringWithRegexCheck(
                            "[�޸��û�] �������û���ϵ��ʽ����8-20Ϊ��ĸ�����֡��ַ���ɣ�������������ͬʱ�������֣�\n",
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
            printf("\n[��ʾ] ��������...\n");

            if (action == 1) {
                User *ret = DB_registerUser(user->name, user->empId, user->passwd, user->role, user->contact);
                if (ret == NULL) {
                    printf("[�ύʧ��] �û�����ʧ�ܣ�����������ر༭��\n");
                    getch();
                    goto EditUser_Refresh;
                }
            } else {
                char ret = DB_updateUser(user);
                if (ret == 0) {
                    printf("[�ύʧ��] �û��޸�ʧ�ܣ�����������ر༭��\n");
                    getch();
                    goto EditUser_Refresh;
                }
            }

            printf("[�ύ�ɹ�] �û��޸�/�����ɹ���������������û��б�\n");
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
 * ���ȫ���û���Ϣ
 */
void printAllUsers() {
    HANDLE windowHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    char search_kw[36] = "";
    int total, page = 1, max_page, page_size = 15, current_total, cnt = 0;

    LinkList_Node *selectedRow = NULL; // ��ѡ�е���
    LinkList_Object *user_data_list = NULL;

    User_GetAndDisplay:

    total = 0;

    system("chcp 936>nul & cls & MODE CON COLS=130 LINES=50");
    // ... �������
    printf("[��ʾ] ��������...\n");

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
    printf(" <A>�½��û� <Enter>�༭�û� <D>ɾ���û� <K>�û�ģ����ѯ");
    printf(" <Esc>�������˵�\n\n");

    UI_printInMiddle("======= �û����û����� =======\n", 95);
    printf("%-10s%-20s%-17s%-8s%-25s%-20s\n", "�û�UID", "����", "ѧ����", "��ɫ", "����¼ʱ��", "��ϵ��ʽ");
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
        if (pt->next == NULL) { // ��������һ���ڵ�
            printf("\n");
        }
    }
    for (int i = 0; i < page_size - current_total; i++) printf("\n"); // ����ҳ��
    printf("\n");
    UI_printInMiddle("=============================\n", 89);

    if (selectedRow == NULL) {
        if (strlen(search_kw)) {
            UI_printErrorData("û�в�ѯ�������������û�");
            strcpy(search_kw, "");
            goto User_GetAndDisplay;
        } else {
            UI_printErrorData("�����û������ǲ������ģ�������ε�¼�����ģ���");
            goto GC_Collect;
        }
    }

    printf("\n[��ǰ��������] ");
    if (strlen(search_kw) > 0) { printf("ģ������=%s\n", search_kw); } else { printf("\n"); }
    printf("[��ʾ] ��%4d�����ݣ���ǰ��%3dҳ����%3dҳ���������ǰһҳ���ҷ��������һҳ����/�·�������л�ѡ�����ݣ�\n",
           total, page, max_page);
    printf("\n");


    int keyboard_press;

    GetKey:
    keyboard_press = _getch();
    switch (keyboard_press) {
        case 224:
            keyboard_press = _getch();
            switch (keyboard_press) {
                case 80: // ��
                    if (selectedRow->next == NULL) selectedRow = user_data_list->head;
                    else selectedRow = selectedRow->next;
                    goto User_Refresh;
                case 72: // ��
                    if (selectedRow->prev == NULL) selectedRow = user_data_list->foot;
                    else selectedRow = selectedRow->prev;
                    goto User_Refresh;
                case 75: // ��
                    page = (page > 1) ? (page - 1) : 1;
                    selectedRow = 0;
                    goto User_GetAndDisplay;
                case 77: // ��
                    page = (page < max_page) ? (page + 1) : (max_page);
                    selectedRow = 0;
                    goto User_GetAndDisplay;
                default:
                    break;
            }
            break;
        case 'k': // �����ؼ���
        case 'K':
            printf("\n[ģ������] ������ؼ��ʣ���\"Enter\"����������");
            gets_safe(search_kw, 35);
            fflush(stdin);
            selectedRow = 0;
            goto User_GetAndDisplay;
        case 'A':
        case 'a': // �½��û�
            editUser(NULL);
            goto User_GetAndDisplay;
        case 'D':
        case 'd': // ɾ���û�
        {
            User *pt = selectedRow->data;
            printf("\n[ȷ��ɾ��] ��ȷ��Ҫɾ���û� %s(%s) �𣿣�������ɾ���û�����Ҫ��������������û���ѧ����:%sȷ�ϣ�\n",
                   pt->name,
                   pt->empId, pt->empId);
            char input_char[31];
            gets_safe(input_char, 30);
            if (strcmp(input_char, pt->empId) != 0) {
                printf("ѧ���Ų�һ�£���ȡ�����������������������\n");
                getch();
                goto User_GetAndDisplay;
            }

            char ret = DB_deleteUser(pt->id);
            if (ret == 0) {
                printf("[ɾ��ʧ��] �û� %s(%s) ɾ��ʧ�ܣ����������������\n", pt->name, pt->empId);
            } else {
                printf("[ɾ���ɹ�] �û� %s(%s) �ѱ�ɾ�������������������\n", pt->name, pt->empId);
            }

            getch();
            goto User_GetAndDisplay;
        }
        case 13: // �༭�û�
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
