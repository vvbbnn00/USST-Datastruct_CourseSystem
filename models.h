//
// Created by vvbbnn00 on 2023/08/28.
//
#ifndef COURSESYSTEM2023_MODELS_H
#define COURSESYSTEM2023_MODELS_H

typedef long long int64;

// �û�����
typedef struct {
    int64 id;                  // �û�ID
    char name[21],             // �û�����
    passwd[65],                // ����
    empId[21];                // ѧ����
    int role;                  // �û���ɫ 0-ѧ�� 1-��ʦ 2-����Ա
    char contact[201];         // ��ϵ��ʽ
    int64 lastLoginTime;     // ���һ�ε�¼ʱ���
} User;

// �γ�����
typedef struct {
    int64 id;                  // �γ�ID/�γ̺�
    char courseName[101],     // �γ�����
    description[501];          // �γ�����
    int64 teacherId;          // �ο���ʦID
    User *teacher;             // �ο���ʦ
    int type,                  // �γ����ͣ�0-���� 1-ѡ�� 2-��ѡ 3-����
    weekStart,                // ������
    weekEnd,                  // �����
    currentMembers,           // ��ǰ��������
    maxMembers;               // ���ɱ�������
    char location[101];        // �Ͽεص�
    double points;             // ѧ��
    int schedule[7][13];       // ÿ���ڿγ̰���
} Course;

// ѡ������
typedef struct {
    int64 id;                  // ѡ��ID
    int64 studentId;          // ѧ��ID
    User *student;             // ѧ����Ϣ
    int64 courseId;           // �γ�ID
    Course *course;            // �γ���Ϣ
    int64 selectionTime;      // ѡ��ÿγ̵�ʱ��
    int64 score;               // �ɼ�
} CourseSelection;

// ѡ�ν�������
typedef struct {
    int status;                // �Ƿ��ѡ���ſγ�
    char lockedReason[100];   // ���ſβ���ѡ���ԭ��
    int64 selectionTime;      // ѡ��ÿγ̵�ʱ��
    Course *course;            // �γ���Ϣ
} CourseSelectionInteraction;


// �������Ͷ���
typedef enum {
    INDEX_TYPE_INT64,
    INDEX_TYPE_STRING,
    INDEX_TYPE_OBJECT
} IndexType;

// ��������
typedef struct {
    int64 hash;
    IndexType type;
    void *data;
    struct IndexListNode *next; // ����
} Index;

#endif