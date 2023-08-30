//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_HASH_H
#define COURSESYSTEM2023_HASH_H

#include "../models.h"

/**
 * 获取中文的相对Hash，此处用GBK编码，因此每个中文字符占两个字节
 *
 * @param chn
 * @return hash
 */
int64 Hash_Chinese(char chn1, char chn2) {
    return (chn1 << 8) + chn2;
}

/**
 * 预处理字符串，将大写字母转换为小写字母，去除不在支持范围内的字符
 * @param string 字符串
 * @return 预处理后的字符串
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
 * 获取string的相对Hash，由于Index的Hash函数为36进制，大小是int64，因此最长只能返回12位Hash
 *
 * @param string
 * @return hash
 */
int64 Hash_String(char *string) {
    int64 hash = 0;
    int i = 0;
    string = Hash_PreprocessString(string);

    while (string[i] != '\0') {
        // 数字对应0-9，字母对应10-35
        int64 value = string[i] - '0';
        if (value > 9) {
            value = string[i] - 'a' + 10;
        }
        // 只有36种可能，因此用36进制
        hash = hash * 36 + value;
        i++;
        // 由于log(36,2^64)=12.4，因此最长只能返回12位Hash
        if (i > 12) {
            break;
        }
    }

    // 若字符串长度小于12，则将剩余位数补0
    while (i < 12) {
        hash = hash * 36;
        i++;
    }
    safe_free(&string);

    return hash;
}

#endif //COURSESYSTEM2023_HASH_H
