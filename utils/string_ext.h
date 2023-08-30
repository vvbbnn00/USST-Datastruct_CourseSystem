//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef STRING_EXT_H
#define STRING_EXT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pcre2.h>

/**
 * ��ʽ��ʱ��
 *
 * %Y-%m-%d %H:%M:%S
 *
 * @param format ��ʽ
 * @param timestamp ʱ���
 * @return
 */
char *getFormatTimeString_(char *format, time_t timestamp) {
    struct tm *time_info = NULL;
    char buffer[strlen(format) + 50];
    time_info = localtime(&timestamp);
    strftime(buffer, strlen(format) + 50, format, time_info);
    char *str_buffer = calloc(strlen(format) + 51, sizeof(char));
    strcpy(str_buffer, buffer);
    return str_buffer;
}


/**
 * ��ȡ��ʽ��ʱ��
 * @param timestamp
 * @return
 */
char *getFormatTimeString(time_t timestamp) {
    return getFormatTimeString_("%Y-%m-%d %H:%M:%S", timestamp);
}


/**
 * �ڴ氲ȫ��gets
 *
 * @param str
 * @param num
 * @return
 */
void gets_safe(char *str, int num) {
    if (fgets(str, num, stdin) != 0) {
        size_t len = strlen(str);
        if (len > 0 && str[len - 1] == '\n')
            str[len - 1] = '\0';
    }
}


/**
 * �ж�������ʽ�Ƿ���������
 * @param pattern ������ʽ
 * @param string �����ַ���
 * @return 0 - ������ 1- ����
 */
char regexMatch(char *pattern, char *string) {
    PCRE2_SIZE error_offset;
    int error_number;
    pcre2_code *re = pcre2_compile(
            (PCRE2_SPTR) pattern,               /* the pattern */
            PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
            0,                     /* default options */
            &error_number,          /* for error number */
            &error_offset,          /* for error offset */
            NULL);                 /* use default compile context */
    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    int rc = pcre2_match(
            re,                   /* the compiled pattern */
            (PCRE2_SPTR) string,              /* the subject string */
            strlen(string),       /* the length of the subject */
            0,                    /* start at offset 0 in the subject */
            0,                    /* default options */
            match_data,           /* block for storing the result */
            NULL);                /* use default match context */
    pcre2_match_data_free(match_data);   /* Release memory used for the match */
    pcre2_code_free(re);                 /*   data and the compiled pattern. */
    return rc < 0 ? 0 : 1;
}

/**
 * ��ȫ��free
 * @param pptr
 */
void safe_free(void **pptr) {
    if (*pptr != NULL) {
        free(*pptr);
        *pptr = NULL;
    } else {
        // �������򾯸�
    }
}

#endif //COURSESYSTEM2023_STRING_EXT_H
