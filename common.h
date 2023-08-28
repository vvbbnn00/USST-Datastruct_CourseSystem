//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_COMMON_H
#define COURSESYSTEM2023_COMMON_H

#define SECRET_KEY "COURSESYSTEM2023"
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "123456"
#define VERSION "v1.0(AVL DB Core v0.1)"

// �û���������ʽ
#define USER_PATTERN "^[a-zA-Z0-9]{3,15}$"
// ����������ʽ
#define PASSWD_PATTERN "^(?=.*\\d)(?=.*[a-zA-Z]).{8,20}$"
// ���������ô��λ��
#define SERVER_CONFIG_PATH "server.conf"
// �û���¼Session���λ��
#define USER_SESSION "login.session"
// �γ�ID������ʽ������Ϊ5-30������ĸ�����ֺ�-��ɵ��ַ����������ʽ<ѧ��>-<ѧ��>-<�γ̴���>-<�༶>��
#define COURSE_ID_PATTERN "^[a-zA-z0-9-]{5,30}$"
// �γ�����������ʽ������Ϊ5-50�ַ�������ĸ�����֡����ĺ�()-��ɵ��ַ�����
#define COURSE_TITLE_PATTERN "^[a-zA-Z0-9\(\)����\u4e00-\u9fa5\-]{5,50}$"
// �γ�����������ʽ������Ϊ 5-250 �ַ���
#define COURSE_DESCRIPTION_PATTERN "^[\\D0-9]{5,250}$"
// �γ�����������ʽ
#define COURSE_TYPE_PATTERN "^[0-3]{1}$"
// ѧ��������ʽ������Ϊ 4-10 �ַ�������ĸ�����ֺ�-��ɵ��ַ����������ʽ<ѧ��>-<ѧ��>��
#define SEMESTER_PATTERN "^[a-zA-Z0-9-]{4,10}$"
// ���ֱ�׼������ʽ������0��
#define NUMBER_PATTERN "^[+]{0,1}(\\d+)$"
// ѧ��������ʽ������0��С�������С�����2λ��
#define POINTS_PATTERN "^[+]{0,1}(\\d+)$|^[+]{0,1}(\\d+\\.\\d{1,2})$"
// �û���ɫ������ʽ
#define USER_ROLE_PATTERN "^[0-2]{1}$"
// �û�����������ʽ
#define USER_NAME_PATTERN "^[ 0-9a-zA-Z\u4e00-\u9fa5��]{2,20}$"
// ѧУ������ʽ������Ϊ3-50�ַ�������ĸ�����֡����ĺͿո���ɵ��ַ�����
#define SCHOOL_PATTERN "^[a-zA-Z0-9()����\u4e00-\u9fa5-]{3,50}$"


#define WORDS_Login_success "---- �û���¼ - ��½�ɹ� ----\n�û�����      %s\n��  ����      %s\n��  ɫ��      %s\n����ʱ�䣺    %s\n\n��½�ɹ���%s����ӭ�����������������ҳ����\n\n"
#define WORDS_Login_role_student "ѧ��"
#define WORDS_Login_role_teacher "��ʦ"
#define WORDS_Login_role_admin "����Ա"
#define WORDS_Login_role_unknown "δ֪��ɫ"

#define WORDS_Login_parse_role_error "[��������] ���ز���role�������󣬵�¼��ֹ��\n"
#define WORDS_Login_save_session_error "[�ļ�����] д���¼״̬�쳣���Զ���¼���ܿ��ܲ����á�\n"

#include <setjmp.h>
jmp_buf GLOBAL_goto_login;

#endif //COURSESYSTEM2023_COMMON_H
