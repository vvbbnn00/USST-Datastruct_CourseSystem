//
// Created by vvbbnn00 on 2023/08/28.
//
#ifndef COURSESYSTEM2023_AVL_H
#define COURSESYSTEM2023_AVL_H

#include <stdlib.h>
#include <stdint.h>
#include "../models.h"

typedef struct IndexListNode {
    Index index;
    struct IndexListNode *next;
} IndexListNode;

// AVL���ڵ㶨��
typedef struct AVLNode {
    IndexListNode *list; // ͷ�ڵ���б�
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

typedef struct NodeList {
    IndexListNode *indexNode;
    struct NodeList *next;
} NodeList;

// ����һ���µ�Index�ڵ�
IndexListNode *Index_newIndexListNode(int64_t hash, IndexType type, void *data) {
    IndexListNode *node = (IndexListNode *) malloc(sizeof(IndexListNode));
    if (!node) {
        return NULL;
    }

    node->index.hash = hash;
    node->index.type = type;
    node->index.data = data;
    node->index.next = NULL;
    node->next = NULL;

    return node;
}

// ��ȡAVL���ڵ�ĸ߶�
int AVL_getHeight(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

// ��ȡ�������������ֵ
int max(int a, int b) {
    return (a > b) ? a : b;
}

// ����
AVLNode *AVL_rightRotate(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T3 = x->right;

    x->right = y;
    y->left = T3;

    y->height = max(AVL_getHeight(y->left), AVL_getHeight(y->right)) + 1;
    x->height = max(AVL_getHeight(x->left), AVL_getHeight(x->right)) + 1;

    return x;
}

// ����
AVLNode *AVL_leftRotate(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(AVL_getHeight(x->left), AVL_getHeight(x->right)) + 1;
    y->height = max(AVL_getHeight(y->left), AVL_getHeight(y->right)) + 1;

    return y;
}

// ��ȡƽ������
int AVL_getBalance(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }
    return AVL_getHeight(node->left) - AVL_getHeight(node->right);
}

// ����ڵ�
AVLNode *AVL_insertNode(AVLNode *node, int64_t hash, IndexType type, void *data) {
    // ������BST����
    if (node == NULL) {
        AVLNode *newNode = (AVLNode *) malloc(sizeof(AVLNode));
        if (!newNode) {
            return NULL;
        }
        IndexListNode *newList = Index_newIndexListNode(hash, type, data);
        newNode->list = newList;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->height = 1;
        return newNode;
    }

    if (hash < node->list->index.hash) {
        node->left = AVL_insertNode(node->left, hash, type, data);
    } else if (hash > node->list->index.hash) {
        node->right = AVL_insertNode(node->right, hash, type, data);
    } else {
        // ����ؼ�����ͬ������뵽������
        IndexListNode *newNode = Index_newIndexListNode(hash, type, data);
        newNode->next = node->list;
        node->list = newNode;
        return node;
    }

    // ���¸߶�
    node->height = 1 + max(AVL_getHeight(node->left), AVL_getHeight(node->right));

    // ��ȡƽ�����Ӳ�������ת
    int balance = AVL_getBalance(node);

    // ����
    if (balance > 1 && hash < node->left->list->index.hash) {
        return AVL_rightRotate(node);
    }

    // ����
    if (balance < -1 && hash > node->right->list->index.hash) {
        return AVL_leftRotate(node);
    }

    // ����
    if (balance > 1 && hash > node->left->list->index.hash) {
        node->left = AVL_leftRotate(node->left);
        return AVL_rightRotate(node);
    }

    // ����
    if (balance < -1 && hash < node->right->list->index.hash) {
        node->right = AVL_rightRotate(node->right);
        return AVL_leftRotate(node);
    }

    return node;
}

// �ҵ�����ֵ��С�Ľڵ�
AVLNode *AVL_minValueNode(AVLNode *node) {
    AVLNode *current = node;

    while (current->left != NULL) {
        current = current->left;
    }

    return current;
}

// ɾ��ָ��ֵ�Ľڵ�
AVLNode *AVL_deleteNode(AVLNode *root, int64_t hash) {
    if (root == NULL) {
        return root;
    }

    // ��׼BSTɾ��
    if (hash < root->list->index.hash) {
        root->left = AVL_deleteNode(root->left, hash);
    } else if (hash > root->list->index.hash) {
        root->right = AVL_deleteNode(root->right, hash);
    } else {
        // �ҵ�����Ҫɾ���Ľڵ�
        // ����ж���ظ��ؼ��֣����������
        IndexListNode *list = root->list;
        while (list) {
            IndexListNode *temp = list;
            list = list->next;
            free(temp);
        }

        // ����ɾ���ڵ�
        if ((root->left == NULL) || (root->right == NULL)) {
            AVLNode *temp = root->left ? root->left : root->right;

            if (!temp) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }

            free(temp);
        } else {
            AVLNode *temp = AVL_minValueNode(root->right);

            root->list = temp->list;

            root->right = AVL_deleteNode(root->right, temp->list->index.hash);
        }

    }

    if (root == NULL) {
        return root;
    }

    // ���¸߶�
    root->height = 1 + max(AVL_getHeight(root->left), AVL_getHeight(root->right));

    // ��ȡƽ�����Ӳ�������ת
    int balance = AVL_getBalance(root);

    // ����
    if (balance > 1 && AVL_getBalance(root->left) >= 0) {
        return AVL_rightRotate(root);
    }

    // ����
    if (balance > 1 && AVL_getBalance(root->left) < 0) {
        root->left = AVL_leftRotate(root->left);
        return AVL_rightRotate(root);
    }

    // ����
    if (balance < -1 && AVL_getBalance(root->right) <= 0) {
        return AVL_leftRotate(root);
    }

    // ����
    if (balance < -1 && AVL_getBalance(root->right) > 0) {
        root->right = AVL_rightRotate(root->right);
        return AVL_leftRotate(root);
    }

    return root;
}


IndexListNode *AVL_searchExact(AVLNode *root, int64_t hash) {
    if (root == NULL) {
        return NULL;
    }

    if (hash < root->list->index.hash) {
        return AVL_searchExact(root->left, hash);
    } else if (hash > root->list->index.hash) {
        return AVL_searchExact(root->right, hash);
    } else {
        return root->list;
    }
}


// �����µĽڵ��б�ڵ�
NodeList *NodeList_newNodeList(IndexListNode *indexNode) {
    NodeList *node = (NodeList *) malloc(sizeof(NodeList));
    if (!node) {
        return NULL;
    }

    node->indexNode = indexNode;
    node->next = NULL;

    return node;
}

// ����������ڸ�����Χ�ڲ��ҽڵ�
void AVL_inOrderSearch(AVLNode *root, int64_t startHash, int64_t endHash, NodeList **resultList) {
    if (root == NULL) {
        return;
    }

    AVL_inOrderSearch(root->left, startHash, endHash, resultList);

    // ��鵱ǰ�ڵ�Ĺ�ϣֵ�Ƿ��ڸ�����Χ��
    if (root->list->index.hash >= startHash && root->list->index.hash <= endHash) {
        NodeList *newNode = NodeList_newNodeList(root->list);
        if (*resultList == NULL) {
            *resultList = newNode;
        } else {
            NodeList *temp = *resultList;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    AVL_inOrderSearch(root->right, startHash, endHash, resultList);
}

NodeList *AVL_findNodesInRange(AVLNode *root, int64_t startHash, int64_t endHash) {
    NodeList *resultList = NULL;
    AVL_inOrderSearch(root, startHash, endHash, &resultList);
    return resultList;
}


// �ݹ�ؽ�AVL��д���ļ�
void AVL_saveToFileHelper(AVLNode *node, FILE *file) {
    if (node == NULL) {
        int64_t nullMarker = INT64_MIN;
        fwrite(&nullMarker, sizeof(int64_t), 1, file);
        return;
    }

    // д�뵱ǰ�ڵ�Ĺ�ϣֵ
    fwrite(&node->list->index.hash, sizeof(int64_t), 1, file);
    // �ж�IndexType��д��
    switch (node->list->index.type) {
        case INDEX_TYPE_INT64: {
            // printf("д��ڵ�0��%lld\n", node->list->index.hash);
            fwrite("0", sizeof(char), 1, file); // IndexType = 0
            fwrite(&node->list->index.data, sizeof(int64_t), 1, file);
            break;
        }
        case INDEX_TYPE_STRING: {
            // printf("д��ڵ�1��%lld\n", node->list->index.hash);
            int64 len = (int64) strlen(node->list->index.data);
            fwrite("1", sizeof(char), 1, file); // IndexType = 1
            fwrite(&len, sizeof(int64_t), 1, file); // д���ַ�������
            fwrite(node->list->index.data, sizeof(char), len, file);
            break;
        }
        case INDEX_TYPE_OBJECT:
            break;
    }
//    printf("д��ڵ㣺%lld\n", node->list->index.hash);

    AVL_saveToFileHelper(node->left, file);   // ����������
    AVL_saveToFileHelper(node->right, file);  // ����������
}

void AVL_saveToFile(AVLNode *root, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("[AVL] Failed to open file for writing");
        return;
    }

    AVL_saveToFileHelper(root, file);

    fclose(file);
}

// �ݹ�ش��ļ���ȡ���ؽ�AVL��
AVLNode *AVL_loadFromFileHelper(FILE *file) {
    int64_t hash;
    fread(&hash, sizeof(int64_t), 1, file);

    if (hash == INT64_MIN) {
        return NULL;
    }

    // ��ȡIndexType���ؽ�Index
    char type;
    IndexType indexType;
    fread(&type, sizeof(char), 1, file);
    void *data;

    switch (type) {
        case '0': {
            fread(&data, sizeof(int64_t), 1, file);
            indexType = INDEX_TYPE_INT64;
            break;
        }
        case '1': {
            int64 len;
            indexType = INDEX_TYPE_STRING;
            fread(&len, sizeof(int64_t), 1, file);
            char *data1 = calloc(len + 1, sizeof(char));
            fread(data1, sizeof(char), len, file);
            data = data1;
            break;
        }
        default: {
            printf("[AVL] Invalid IndexType.\n");
            return NULL;
        }
    }

    AVLNode *node = (AVLNode *) malloc(sizeof(AVLNode));
    if (!node) {
        perror("[AVL] Failed to allocate memory for AVLNode");
        return NULL;
    }
    node->list = Index_newIndexListNode(hash, indexType, data);
    node->left = AVL_loadFromFileHelper(file);
    node->right = AVL_loadFromFileHelper(file);

    node->height = max(AVL_getHeight(node->left), AVL_getHeight(node->right)) + 1; // ���¸߶�

    return node;
}

AVLNode *AVL_loadFromFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("[AVL] Failed to open file for reading");
        return NULL;
    }

    AVLNode *root = AVL_loadFromFileHelper(file);

    fclose(file);

    printf("[AVL] Loaded AVL tree from file: %s\n", filename);

    return root;
}

#endif