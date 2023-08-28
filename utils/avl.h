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

// AVL树节点定义
typedef struct AVLNode {
    IndexListNode *list; // 头节点的列表
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

typedef struct NodeList {
    IndexListNode *indexNode;
    struct NodeList *next;
} NodeList;

// 创建一个新的Index节点
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

// 获取AVL树节点的高度
int AVL_getHeight(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

// 获取两个整数的最大值
int max(int a, int b) {
    return (a > b) ? a : b;
}

// 右旋
AVLNode *AVL_rightRotate(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T3 = x->right;

    x->right = y;
    y->left = T3;

    y->height = max(AVL_getHeight(y->left), AVL_getHeight(y->right)) + 1;
    x->height = max(AVL_getHeight(x->left), AVL_getHeight(x->right)) + 1;

    return x;
}

// 左旋
AVLNode *AVL_leftRotate(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(AVL_getHeight(x->left), AVL_getHeight(x->right)) + 1;
    y->height = max(AVL_getHeight(y->left), AVL_getHeight(y->right)) + 1;

    return y;
}

// 获取平衡因子
int AVL_getBalance(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }
    return AVL_getHeight(node->left) - AVL_getHeight(node->right);
}

// 插入节点
AVLNode *AVL_insertNode(AVLNode *node, int64_t hash, IndexType type, void *data) {
    // 正常的BST插入
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
        // 如果关键字相同，则插入到链表中
        IndexListNode *newNode = Index_newIndexListNode(hash, type, data);
        newNode->next = node->list;
        node->list = newNode;
        return node;
    }

    // 更新高度
    node->height = 1 + max(AVL_getHeight(node->left), AVL_getHeight(node->right));

    // 获取平衡因子并进行旋转
    int balance = AVL_getBalance(node);

    // 左左
    if (balance > 1 && hash < node->left->list->index.hash) {
        return AVL_rightRotate(node);
    }

    // 右右
    if (balance < -1 && hash > node->right->list->index.hash) {
        return AVL_leftRotate(node);
    }

    // 左右
    if (balance > 1 && hash > node->left->list->index.hash) {
        node->left = AVL_leftRotate(node->left);
        return AVL_rightRotate(node);
    }

    // 右左
    if (balance < -1 && hash < node->right->list->index.hash) {
        node->right = AVL_rightRotate(node->right);
        return AVL_leftRotate(node);
    }

    return node;
}

// 找到树中值最小的节点
AVLNode *AVL_minValueNode(AVLNode *node) {
    AVLNode *current = node;

    while (current->left != NULL) {
        current = current->left;
    }

    return current;
}

// 删除指定值的节点
AVLNode *AVL_deleteNode(AVLNode *root, int64_t hash) {
    if (root == NULL) {
        return root;
    }

    // 标准BST删除
    if (hash < root->list->index.hash) {
        root->left = AVL_deleteNode(root->left, hash);
    } else if (hash > root->list->index.hash) {
        root->right = AVL_deleteNode(root->right, hash);
    } else {
        // 找到了需要删除的节点
        // 如果有多个重复关键字，先清空链表
        IndexListNode *list = root->list;
        while (list) {
            IndexListNode *temp = list;
            list = list->next;
            free(temp);
        }

        // 接着删除节点
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

    // 更新高度
    root->height = 1 + max(AVL_getHeight(root->left), AVL_getHeight(root->right));

    // 获取平衡因子并进行旋转
    int balance = AVL_getBalance(root);

    // 左左
    if (balance > 1 && AVL_getBalance(root->left) >= 0) {
        return AVL_rightRotate(root);
    }

    // 左右
    if (balance > 1 && AVL_getBalance(root->left) < 0) {
        root->left = AVL_leftRotate(root->left);
        return AVL_rightRotate(root);
    }

    // 右右
    if (balance < -1 && AVL_getBalance(root->right) <= 0) {
        return AVL_leftRotate(root);
    }

    // 右左
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


// 创建新的节点列表节点
NodeList *NodeList_newNodeList(IndexListNode *indexNode) {
    NodeList *node = (NodeList *) malloc(sizeof(NodeList));
    if (!node) {
        return NULL;
    }

    node->indexNode = indexNode;
    node->next = NULL;

    return node;
}

// 中序遍历并在给定范围内查找节点
void AVL_inOrderSearch(AVLNode *root, int64_t startHash, int64_t endHash, NodeList **resultList) {
    if (root == NULL) {
        return;
    }

    AVL_inOrderSearch(root->left, startHash, endHash, resultList);

    // 检查当前节点的哈希值是否在给定范围内
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


// 递归地将AVL树写入文件
void AVL_saveToFileHelper(AVLNode *node, FILE *file) {
    if (node == NULL) {
        int64_t nullMarker = INT64_MIN;
        fwrite(&nullMarker, sizeof(int64_t), 1, file);
        return;
    }

    // 写入当前节点的哈希值
    fwrite(&node->list->index.hash, sizeof(int64_t), 1, file);
    // 判断IndexType并写入
    switch (node->list->index.type) {
        case INDEX_TYPE_INT64: {
            // printf("写入节点0：%lld\n", node->list->index.hash);
            fwrite("0", sizeof(char), 1, file); // IndexType = 0
            fwrite(&node->list->index.data, sizeof(int64_t), 1, file);
            break;
        }
        case INDEX_TYPE_STRING: {
            // printf("写入节点1：%lld\n", node->list->index.hash);
            int64 len = (int64) strlen(node->list->index.data);
            fwrite("1", sizeof(char), 1, file); // IndexType = 1
            fwrite(&len, sizeof(int64_t), 1, file); // 写入字符串长度
            fwrite(node->list->index.data, sizeof(char), len, file);
            break;
        }
        case INDEX_TYPE_OBJECT:
            break;
    }
//    printf("写入节点：%lld\n", node->list->index.hash);

    AVL_saveToFileHelper(node->left, file);   // 保存左子树
    AVL_saveToFileHelper(node->right, file);  // 保存右子树
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

// 递归地从文件读取并重建AVL树
AVLNode *AVL_loadFromFileHelper(FILE *file) {
    int64_t hash;
    fread(&hash, sizeof(int64_t), 1, file);

    if (hash == INT64_MIN) {
        return NULL;
    }

    // 读取IndexType并重建Index
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

    node->height = max(AVL_getHeight(node->left), AVL_getHeight(node->right)) + 1; // 更新高度

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