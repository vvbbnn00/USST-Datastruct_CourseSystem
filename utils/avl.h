//
// Created by vvbbnn00 on 2023/08/28.
//
#ifndef COURSESYSTEM2023_AVL_H
#define COURSESYSTEM2023_AVL_H

#include <stdlib.h>
#include <stdint.h>
#include "../models.h"
#include "math_ext.h"
#include "string_ext.h"

// ��������
typedef struct IndexListNode {
    Index index; // ����
    struct IndexListNode *next; // ��һ���ڵ�
} IndexListNode;

// AVL���ڵ㶨��
typedef struct AVLNode {
    IndexListNode *list;    // ��������
    int height;             // �ڵ�߶�
    struct AVLNode *left;   // ������
    struct AVLNode *right;  // ������
} AVLNode;

// �ڵ��б�����������ڵ�ʱʹ��
typedef struct NodeList {
    IndexListNode *indexNode;   // ��������ڵ�
    struct NodeList *next;      // ��һ���ڵ�
} NodeList;


/**
 * �����µ������ڵ�
 * @param hash
 * @param type
 * @param data
 * @return
 */
IndexListNode *Index_newIndexListNode(int64 hash, IndexType type, void *data) {
    IndexListNode *node = (IndexListNode *) calloc(1, sizeof(IndexListNode));
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


/**
 * �������
 * @param node
 * @return
 */
IndexListNode *IndexListNode_deepCopy(IndexListNode *node) {
    IndexListNode *newNode = (IndexListNode *) calloc(1, sizeof(IndexListNode));
    if (!newNode) {
        return NULL;
    }

    if (node == NULL) {
        return newNode;
    }

    newNode->index.hash = node->index.hash;
    newNode->index.type = node->index.type;
    newNode->index.data = node->index.data;
    newNode->index.next = NULL;

    for (IndexListNode *p = node->next, *q = newNode; p != NULL; p = p->next, q = q->next) {
        q->next = Index_newIndexListNode(p->index.hash, p->index.type, p->index.data);
    }


    return newNode;
}


/**
 * ɾ����������
 * @param node
 */
void IndexListNode_delete(IndexListNode *node) {
    if (node == NULL) {
        return;
    }
    IndexListNode *p = node;
    while (p) {
        IndexListNode *temp = p;
        p = p->next;
        if (temp->index.type == INDEX_TYPE_STRING) {
            safe_free(&temp->index.data);
        }
        safe_free(&temp);
    }
}


/**
 * ��ȡ�ڵ�ĸ߶�
 * @param node
 * @return
 */
int AVL_getHeight(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

/**
 * ����
 * @param y
 * @return
 */
AVLNode *AVL_rightRotate(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T3 = x->right;

    x->right = y;
    y->left = T3;

    y->height = max(AVL_getHeight(y->left), AVL_getHeight(y->right)) + 1;
    x->height = max(AVL_getHeight(x->left), AVL_getHeight(x->right)) + 1;

    return x;
}

/**
 * ����
 * @param x
 * @return
 */
AVLNode *AVL_leftRotate(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(AVL_getHeight(x->left), AVL_getHeight(x->right)) + 1;
    y->height = max(AVL_getHeight(y->left), AVL_getHeight(y->right)) + 1;

    return y;
}

/**
 * ��ȡ�ڵ��ƽ������
 * @param node
 * @return
 */
int AVL_getBalance(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }
    return AVL_getHeight(node->left) - AVL_getHeight(node->right);
}

/**
 * �����µĽڵ�
 * @param node
 * @param hash
 * @param type
 * @param data
 * @return
 */
AVLNode *AVL_insertNode(AVLNode *node, int64 hash, IndexType type, void *data) {
    // ������BST����
    if (node == NULL) {
        AVLNode *newNode = (AVLNode *) calloc(1, sizeof(AVLNode));
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

/**
 * ����ָ����ϣֵ�Ľڵ�
 * @param node
 * @return
 */
AVLNode *AVL_minValueNode(AVLNode *node) {
    AVLNode *current = node;

    while (current->left != NULL) {
        current = current->left;
    }

    return current;
}

/**
 * ɾ��ָ����ϣֵ�Ľڵ�
 * @param root
 * @param hash
 * @return
 */
AVLNode *AVL_deleteNode(AVLNode *root, int64 hash) {
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
            safe_free(&temp);
        }

        // ����ɾ���ڵ�
        if (root->left == NULL || root->right == NULL) {
            AVLNode *temp = root->left ? root->left : root->right;

            if (!temp) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }

            safe_free(&temp);
        } else {
            AVLNode *temp = AVL_minValueNode(root->right);

            // ��Ҫ���
            root->list = IndexListNode_deepCopy(temp->list);

            root->right = AVL_deleteNode(root->right, temp->list->index.hash);
        }
    }

    if (root == NULL) {
        return root;
    }

    root->height = 1 + max(AVL_getHeight(root->left), AVL_getHeight(root->right));

    int balance = AVL_getBalance(root);

    if (balance > 1 && AVL_getBalance(root->left) >= 0) {
        return AVL_rightRotate(root);
    }

    if (balance > 1 && AVL_getBalance(root->left) < 0) {
        root->left = AVL_leftRotate(root->left);
        return AVL_rightRotate(root);
    }

    if (balance < -1 && AVL_getBalance(root->right) <= 0) {
        return AVL_leftRotate(root);
    }

    if (balance < -1 && AVL_getBalance(root->right) > 0) {
        root->right = AVL_rightRotate(root->right);
        return AVL_leftRotate(root);
    }

    return root;
}


/**
 * ɾ�������ض�ID�������ڵ㣬��ɾ��������Ϊ�գ���ɾ������AVL�ڵ�
 *
 * @param root AVL���ĸ��ڵ�
 * @param hash Ҫɾ���Ľڵ�Ĺ�ϣֵ
 * @param delId Ҫɾ��������ID
 * @return ���ز������AVL���ĸ��ڵ�
 */
AVLNode *AVL_deleteNodeById(AVLNode *root, int64 hash, int64 delId) {
    IndexListNode *AVL_searchExact(AVLNode *root, int64 hash);
    IndexListNode *node1 = AVL_searchExact(root, hash);

    // �������Ƿ�ɹ�
    if (node1 == NULL) {
        return root;
    }

    // ���⴦���һ���ڵ㣬�Ա�����ѭ������
    if ((int64) node1->index.data == delId) {
        IndexListNode *temp = node1->next;
        if (temp) {
            node1->index = temp->index;
            node1->next = temp->next;
            safe_free(&temp);
            return root;
        } else {
            return AVL_deleteNode(root, hash);
        }
    }

    // ����������Ľڵ�
    for (IndexListNode *prev = node1, *n = node1->next; n != NULL; prev = n, n = n->next) {
        if ((int64) n->index.data == delId) {
            prev->next = n->next;
            safe_free(&n);
            break;
        }
    }
    return root;
}

/**
 * ����ָ����ϣֵ�Ľڵ�
 * @param root
 * @param hash
 * @return
 */
IndexListNode *AVL_searchExact(AVLNode *root, int64 hash) {
    if (root == NULL) {
        return NULL;
    }

    if (hash < root->list->index.hash) {
        return AVL_searchExact(root->left, hash);
    }
    if (hash > root->list->index.hash) {
        return AVL_searchExact(root->right, hash);
    }
    return root->list;
}


/**
 * ��ȡAVL���нڵ������
 * @param root
 * @return
 */
int64 AVL_countNodes(AVLNode *root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + AVL_countNodes(root->left) + AVL_countNodes(root->right);
}


/**
 * �½�һ���ڵ�����
 * @param indexNode
 * @return
 */
NodeList *NodeList_newNodeList(IndexListNode *indexNode) {
    NodeList *node = (NodeList *) calloc(1, sizeof(NodeList));
    if (!node) {
        return NULL;
    }

    node->indexNode = indexNode;
    node->next = NULL;

    return node;
}


/**
 * �������AVL������������浽������
 * @param root
 * @param resultList
 */
void AVL_inOrderTraverse(AVLNode *root, NodeList **resultList) {
    if (root == NULL) {
        return;
    }

    AVL_inOrderTraverse(root->left, resultList);

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

    AVL_inOrderTraverse(root->right, resultList);
}


/**
 * ����ָ����Χ�ڵĽڵ�
 * @param root
 * @param startHash
 * @param endHash
 * @param resultList
 */
void AVL_inOrderSearch(AVLNode *root, int64 startHash, int64 endHash, NodeList **resultList) {
    if (root == NULL) {
        return;
    }

    AVL_inOrderSearch(root->left, startHash, endHash, resultList);

    // ��鵱ǰ�ڵ�Ĺ�ϣֵ�Ƿ��ڸ�����Χ��
    if (root->list->index.hash >= startHash && root->list->index.hash <= endHash) {
        // printf("�ҵ��ڵ㣺%lld\n", root->list->index.hash);
        // system("pause");
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


/**
 * ����AVL�����ļ� Helper
 * @param node
 * @param file
 */
void AVL_saveToFileHelper(AVLNode *node, FILE *file) {
    if (node == NULL) {
        int64 nullMarker = INT64_MIN;
        fwrite(&nullMarker, sizeof(int64), 1, file);
        return;
    }

    // д�뵱ǰ�ڵ�Ĺ�ϣֵ
    fwrite(&node->list->index.hash, sizeof(int64), 1, file);
    int64 continueMarker = INT64_MIN + 1;
    // �ж�IndexType��д��
    switch (node->list->index.type) {
        case INDEX_TYPE_INT64: {
            // printf("д��ڵ�0��%lld\n", node->list->index.hash);
            fwrite("0", sizeof(char), 1, file); // IndexType = 0
            // ��List�е�Indexת��Ϊint64��д��
            for (IndexListNode *p = node->list; p != NULL; p = p->next) {
                if (p->index.data == NULL) {
                    continue;
                }
                fwrite(&p->index.data, sizeof(int64), 1, file);
                if (p->next != NULL) {
                    fwrite(&continueMarker, sizeof(int64), 1, file);
                }
            }
            break;
        }
        case INDEX_TYPE_STRING: {
            // printf("д��ڵ�1��%lld\n", node->list->index.hash);
            int64 len = (int64) strlen(node->list->index.data);
            fwrite("1", sizeof(char), 1, file); // IndexType = 1
            for (IndexListNode *p = node->list; p != NULL; p = p->next) {
                fwrite(&len, sizeof(int64), 1, file); // д���ַ�������
                fwrite(p->index.data, sizeof(char), len, file);
                if (p->next != NULL) {
                    fwrite(&continueMarker, sizeof(int64), 1, file);
                }
            }
            break;
        }
        case INDEX_TYPE_OBJECT:
            break;
    }
//    printf("д��ڵ㣺%lld\n", node->list->index.hash);

    AVL_saveToFileHelper(node->left, file);   // ����������
    AVL_saveToFileHelper(node->right, file);  // ����������
}

/**
 * ����AVL�����ļ�
 * @param root
 * @param filename
 */
void AVL_saveToFile(AVLNode *root, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("[AVL] Failed to open file for writing");
        return;
    }

    AVL_saveToFileHelper(root, file);

    fclose(file);
}

/**
 * ���ļ��м���AVL�� Helper
 * @param file
 * @return
 */
AVLNode *AVL_loadFromFileHelper(FILE *file) {
    int64 hash;
    fread(&hash, sizeof(int64), 1, file);

    if (hash == INT64_MIN) {
        return NULL;
    }

    // ��ȡIndexType���ؽ�Index
    char type;
    IndexType indexType;
    fread(&type, sizeof(char), 1, file);
    void *data;

    AVLNode *node = (AVLNode *) calloc(1, sizeof(AVLNode));
    if (!node) {
        perror("[AVL] Failed to allocate memory for AVLNode");
        return NULL;
    }

    do {
        switch (type) {
            case '0': {
                fread(&data, sizeof(int64), 1, file);
                indexType = INDEX_TYPE_INT64;
                break;
            }
            case '1': {
                int64 len;
                indexType = INDEX_TYPE_STRING;
                fread(&len, sizeof(int64), 1, file);
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
        if (node->list == NULL) {
            node->list = Index_newIndexListNode(hash, indexType, data);
        } else {
            IndexListNode *newNode = Index_newIndexListNode(hash, indexType, data);
            newNode->next = node->list;
            node->list = newNode;
        }
        int64 signal;
        fread(&signal, sizeof(int64), 1, file);
        if (signal == INT64_MIN + 1) {
            continue;
        } else {
            // ����һ��int64
            fseek(file, -8, SEEK_CUR);
            break;
        }
    } while (1);

    node->left = AVL_loadFromFileHelper(file);
    node->right = AVL_loadFromFileHelper(file);

    node->height = max(AVL_getHeight(node->left), AVL_getHeight(node->right)) + 1; // ���¸߶�

    return node;
}

/**
 * ���ļ��м���AVL��
 * @param filename
 * @return
 */
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


/**
 * �������AVL������ӡ��� Helper
 * @param root
 */
void AVL_printInOrder_Helper(AVLNode *root) {
    if (root == NULL) {
        return;
    }
    AVL_printInOrder_Helper(root->left);
    printf("%lld: ", root->list->index.hash);
    IndexListNode *list = root->list;
    while (list) {
        printf("%lld ", (int64) list->index.data);
        list = list->next;
    }
    printf("\n");
    AVL_printInOrder_Helper(root->right);
}


/**
 * �������AVL������ӡ���
 * @param root
 */
void AVL_printInOrder(AVLNode *root) {
    printf("AVL���й��� %lld ���ڵ㣬��ʼ�������\n", AVL_countNodes(root));
    AVL_printInOrder_Helper(root);
    printf("�����������\n");
}


/**
 * �������AVL������ӡ��� Helper
 * @param root
 */
void AVL_printPreOrder_Helper(AVLNode *root) {
    if (root == NULL) {
        return;
    }
    printf("%lld: ", root->list->index.hash);
    IndexListNode *list = root->list;
    while (list) {
        printf("%lld ", (int64) list->index.data);
        list = list->next;
    }
    printf("\n");
    AVL_printPreOrder_Helper(root->left);
    AVL_printPreOrder_Helper(root->right);
}


/**
 * �������AVL������ӡ���
 * @param root
 */
void AVL_printPreOrder(AVLNode *root) {
    printf("AVL���й��� %lld ���ڵ㣬��ʼ�������\n", AVL_countNodes(root));
    AVL_printPreOrder_Helper(root);
    printf("�����������\n");
}

#endif