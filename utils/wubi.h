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
 * ��������ļ���AVL����
 */
void Wubi_Init() {
    wubiTree = AVL_loadFromFile("utils/wubi.avl");
    if (wubiTree != NULL) {
        printf("[Wubi_Init] �ӻ��������м�����������\n");
        return;
    }

    FILE *fp = fopen(WUBI_PATH, "r");
    if (fp == NULL) {
        perror("[Wubi_Init] ������ļ�ʧ��");
        exit(EXIT_FAILURE);
    }
    char *line = calloc(100, sizeof(char));
    while (fgets(line, 100, fp)) {
        // ��ʽΪ�����\t����\t���
        strtok(line, "\t");
        char *chn = strtok(NULL, "\t");
        char *wubi = strtok(NULL, "\t");
        // ȥ�����з�
        wubi[strlen(wubi) - 1] = '\0';
        // ����AVL��
        int64 hash = Hash_Chinese(chn[0], chn[1]);
        wubiTree = AVL_insertNode(wubiTree, hash, INDEX_TYPE_STRING, strdup(wubi));
    }

    fclose(fp);
    safe_free(&line);
    printf("[Wubi_Init] �������ʼ�����\n");

    AVL_saveToFile(wubiTree, "utils/wubi.avl");
}


/**
 * ����ת��ʺ���������Index��Hash����Ϊ36���ƣ���С��int64������ֻ�ܷ���12λ���
 * @param chn �����ַ���
 * @return ����ַ���
 */
char *Wubi_chn2wubi(char *chn) {
    char *wubi = calloc(13, sizeof(char));
    int i = 0, current = 0;
    while (chn[current] != '\0') {
        // ���Ǵ�д��ĸ����ת��ΪСд��ĸ��ֱ�Ӽ�������ַ���
        if (chn[current] >= 'A' && chn[current] <= 'Z') {
            wubi[i++] = chn[current] + 32;
            current++;
            continue;
        }
        // ����Сд��ĸ�����֣���ֱ�Ӽ�������ַ���
        if ((chn[current] >= 'a' && chn[current] <= 'z') || (chn[current] >= '0' && chn[current] <= '9')) {
            wubi[i++] = chn[current];
            current++;
            continue;
        }
        // �������ģ���ת��Ϊ���
        int64 hash = Hash_Chinese(chn[current], chn[current + 1]);
        // printf("��ǰ���ģ�%c%c\n", chn[current], chn[current + 1]);
        IndexListNode *node = AVL_searchExact(wubiTree, hash);

        // ����û���ҵ������ģ�ֱ������
        if (node == NULL) {
            // printf("δ�ҵ���ʣ�%c%c\n", chn[current], chn[current + 1]);
            current += 2;
            continue;
        }

        // printf("�ҵ���ʣ�%s\n", (const char *) node->index.data);
        const char *tmp = (const char *) node->index.data;
        int j = 0;
        while (tmp[j] != '\0') {
            wubi[i++] = tmp[j++];
            // �ж��Ƿ񳬹�12λ
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
