//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_WUBI_H
#define COURSESYSTEM2023_WUBI_H

#include "avl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

#define WUBI_PATH "utils/wubi.txt"

AVLNode *wubiTree = NULL;

/**
 * 加载五笔文件到AVL树中
 */
void Wubi_Init() {
    wubiTree = AVL_loadFromFile("utils/wubi.avl");
    if (wubiTree != NULL) {
        printf("[Wubi_Init] 从缓存索引中加载五笔树完成\n");
        return;
    }

    FILE *fp = fopen(WUBI_PATH, "r");
    if (fp == NULL) {
        perror("[Wubi_Init] 打开五笔文件失败");
        exit(EXIT_FAILURE);
    }
    char *line = calloc(100, sizeof(char));
    while (fgets(line, 100, fp)) {
        // 格式为：编号\t中文\t五笔
        strtok(line, "\t");
        char *chn = strtok(NULL, "\t");
        char *wubi = strtok(NULL, "\t");
        // 去除换行符
        wubi[strlen(wubi) - 1] = '\0';
        // 插入AVL树
        int64 hash = Hash_Chinese(chn[0], chn[1]);
        wubiTree = AVL_insertNode(wubiTree, hash, INDEX_TYPE_STRING, strdup(wubi));
    }

    fclose(fp);
    safe_free(&line);
    printf("[Wubi_Init] 五笔树初始化完成\n");

    AVL_saveToFile(wubiTree, "utils/wubi.avl");
}


/**
 * 中文转五笔函数，由于Index的Hash函数为36进制，大小是int64，因此最长只能返回12位五笔
 * @param chn 中文字符串
 * @return 五笔字符串
 */
char *Wubi_chn2wubi(char *chn) {
    char *wubi = calloc(13, sizeof(char));
    int i = 0, current = 0;
    while (chn[current] != '\0') {
        // 若是大写字母，则转换为小写字母，直接加入五笔字符串
        if (chn[current] >= 'A' && chn[current] <= 'Z') {
            wubi[i++] = chn[current] + 32;
            current++;
            continue;
        }
        // 若是小写字母或数字，则直接加入五笔字符串
        if ((chn[current] >= 'a' && chn[current] <= 'z') || (chn[current] >= '0' && chn[current] <= '9')) {
            wubi[i++] = chn[current];
            current++;
            continue;
        }
        // 若是中文，则转换为五笔
        int64 hash = Hash_Chinese(chn[current], chn[current + 1]);
        // printf("当前中文：%c%c\n", chn[current], chn[current + 1]);
        IndexListNode *node = AVL_searchExact(wubiTree, hash);

        // 对于没有找到的中文，直接跳过
        if (node == NULL) {
            // printf("未找到五笔：%c%c\n", chn[current], chn[current + 1]);
            current += 2;
            continue;
        }

        // printf("找到五笔：%s\n", (const char *) node->index.data);
        const char *tmp = (const char *) node->index.data;
        int j = 0;
        while (tmp[j] != '\0') {
            wubi[i++] = tmp[j++];
            // 判断是否超过12位
            if (i >= 12) {
                wubi[12] = '\0';
                return wubi;
            }
        }
        current += 2;
    }

    return wubi;
}

#endif //COURSESYSTEM2023_WUBI_H
