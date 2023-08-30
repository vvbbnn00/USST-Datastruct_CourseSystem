//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_HASH_H
#define COURSESYSTEM2023_HASH_H

#include "../models.h"

/**
 * ��ȡ���ĵ����Hash���˴���GBK���룬���ÿ�������ַ�ռ�����ֽ�
 *
 * @param chn
 * @return hash
 */
int64 Hash_Chinese(char chn1, char chn2) {
    return (chn1 << 8) + chn2;
}

/**
 * Ԥ�����ַ���������д��ĸת��ΪСд��ĸ��ȥ������֧�ַ�Χ�ڵ��ַ�
 * @param string �ַ���
 * @return Ԥ�������ַ���
 */
char *Hash_PreprocessString(char *string) {
    char *final = calloc(strlen(string) + 1, sizeof(char));
    int i = -1, j = 0;
    do {
        i++;
        if (string[i] >= 'A' && string[i] <= 'Z') {
            final[j++] = string[i] - 'A' + 'a';
            continue;
        }
        if (string[i] >= 'a' && string[i] <= 'z') {
            final[j++] = string[i];
            continue;
        }
        if (string[i] >= '0' && string[i] <= '9') {
            final[j++] = string[i];
            continue;
        }
    } while (string[i] != '\0');
    return final;
}


/**
 * ��ȡstring�����Hash������Index��Hash����Ϊ36���ƣ���С��int64������ֻ�ܷ���12λHash
 *
 * @param string
 * @return hash
 */
int64 Hash_String(char *string) {
    int64 hash = 0;
    int i = 0;
    string = Hash_PreprocessString(string);

    while (string[i] != '\0') {
        // ���ֶ�Ӧ0-9����ĸ��Ӧ10-35
        int64 value = string[i] - '0';
        if (value > 9) {
            value = string[i] - 'a' + 10;
        }
        // ֻ��36�ֿ��ܣ������36����
        hash = hash * 36 + value;
        i++;
        // ����log(36,2^64)=12.4������ֻ�ܷ���12λHash
        if (i > 12) {
            break;
        }
    }

    // ���ַ�������С��12����ʣ��λ����0
    while (i < 12) {
        hash = hash * 36;
        i++;
    }
    safe_free(&string);

    return hash;
}

#endif //COURSESYSTEM2023_HASH_H
