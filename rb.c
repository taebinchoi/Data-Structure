#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // 필요한 헤더파일 불러오기

typedef enum { RED, BLACK } Color;

typedef struct rbnode {
    int key;
    Color color;
    struct rbnode *left, *right, *parent;
} rbnode;

rbnode* createNode(int key);

void rotateLeft(rbnode **root, rbnode *x);
void rotateRight(rbnode **root, rbnode *x);
void insert(rbnode **root, rbnode *x);
void insertFixup(rbnode **root, rbnode *x);
void delete(rbnode **root, rbnode *z);
void deleteFixup(rbnode **root, rbnode *x);
void transplant(rbnode **root, rbnode *u, rbnode *v);
rbnode* minimum(rbnode *node);
void printInorder(rbnode *root, FILE *file);
void printLevelorder(rbnode *root, FILE *file); // 해당되는 함수들

rbnode* createNode(int key) { // node 생성, O(1)
    rbnode *new_node = (rbnode *)malloc(sizeof(rbnode));
    if (new_node == NULL) {
        exit(EXIT_FAILURE);
    }
    new_node->key = key;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    new_node->color = RED;
    return new_node;
}

void rotateLeft(rbnode **root, rbnode *x) { // 왼쪽 회전, O(1)
    if (x == NULL || x->right == NULL) return;

    rbnode *y = x->right;
    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rotateRight(rbnode **root, rbnode *x) { // 오른쪽 회전, O(1)
    if (x == NULL || x->left == NULL) return;

    rbnode *y = x->left;
    x->left = y->right;
    if (y->right != NULL) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

void insertFixup(rbnode **root, rbnode *x) { // insert 수행 후 RB 조건에 맞게 고치기, 최대 O(log N)
    while (x != *root && x->parent->color == RED) {
        if (x->parent == x->parent->parent->left) {
            rbnode *y = x->parent->parent->right;
            if (y != NULL && y->color == RED) {
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {
                if (x == x->parent->right) {
                    x = x->parent;
                    rotateLeft(root, x);
                }
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateRight(root, x->parent->parent);
            }
        } else {
            rbnode *y = x->parent->parent->left;
            if (y != NULL && y->color == RED) {
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {
                if (x == x->parent->left) {
                    x = x->parent;
                    rotateRight(root, x);
                }
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateLeft(root, x->parent->parent);
            }
        }
    }
    (*root)->color = BLACK;
}

void insert(rbnode **root, rbnode *x) { // insert 수행, O(log N)
    rbnode *y = NULL;
    rbnode *current = *root;
    while (current != NULL) {
        y = current;
        if (x->key == current->key) {
            free(x);
            return;   // 중복된 값일 때 삽입하지 않고 함수를 종료
        } else if (x->key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    x->parent = y;
    if (y == NULL) {
        *root = x;
    } else if (x->key < y->key) {
        y->left = x;
    } else {
        y->right = x;
    }
    x->left = NULL;
    x->right = NULL;
    x->color = RED;
    insertFixup(root, x);
}

void deleteFixup(rbnode **root, rbnode *x) { // delete 수행 후 RB 조건에 맞게 고치기, 최대 O(log N)
    while (x != *root && x != NULL && x->color == BLACK) {
        if (x == x->parent->left) {
            rbnode *w = x->parent->right;
            if (w == NULL) return;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateLeft(root, x->parent);
                w = x->parent->right;
            }
            if (w == NULL) return;
            if ((w->left == NULL || w->left->color == BLACK) && (w->right == NULL || w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right == NULL || w->right->color == BLACK) {
                    if (w->left != NULL) w->left->color = BLACK;
                    w->color = RED;
                    rotateRight(root, w);
                    w = x->parent->right;
                }
                if (w == NULL) return;
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->right != NULL) w->right->color = BLACK;
                rotateLeft(root, x->parent);
                x = *root;
            }
        } else {
            rbnode *w = x->parent->left;
            if (w == NULL) return;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateRight(root, x->parent);
                w = x->parent->left;
            }
            if (w == NULL) return;
            if ((w->right == NULL || w->right->color == BLACK) && (w->left == NULL || w->left->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left == NULL || w->left->color == BLACK) {
                    if (w->right != NULL) w->right->color = BLACK;
                    w->color = RED;
                    rotateLeft(root, w);
                    w = x->parent->left;
                }
                if (w == NULL) return;
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->left != NULL) w->left->color = BLACK;
                rotateRight(root, x->parent);
                x = *root;
            }
        }
    }
    if (x != NULL) {
        x->color = BLACK;
    }
}

void transplant(rbnode **root, rbnode *u, rbnode *v) { // 노드의 포인터 조정, O(1)
    if (u->parent == NULL) {
        *root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v != NULL) {
        v->parent = u->parent;
    }
}

rbnode* minimum(rbnode *node) { // 최솟값 찾기
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

void delete(rbnode **root, rbnode *z) { // delete 수행, O(log N)
    printf("Deleting node with key: %d\n", z->key); // 삭제 직전의 키 출력 (터미널에서 검토용)
    rbnode *y = z;
    rbnode *x;
    int original_color = y->color;
    if (z->left == NULL) {
        x = z->right;
        transplant(root, z, z->right);
    } else if (z->right == NULL) {
        x = z->left;
        transplant(root, z, z->left);
    } else {
        y = minimum(z->right);
        original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            if (x != NULL) {
                x->parent = y;
            }
        } else {
            transplant(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (original_color == BLACK) {
        deleteFixup(root, x);
    }
    free(z);
    printf("After deletion:\n"); // 추가: 삭제 후의 트리 상태 출력 (터미널에서 검토용)
    printInorder(*root, stdout);
    printf("\n");
    printLevelorder(*root, stdout);
    printf("\n");
}

void printInorder(rbnode *root, FILE *file) { // In-Order 순회결과 출력, O(N)
    if (root != NULL) {
        printInorder(root->left, file);
        fprintf(file, "%d ", root->key);
        printInorder(root->right, file);
    }
}

void printLevelorder(rbnode *root, FILE *file) { // Level-Order 순회결과 출력, O(N)
    if (root == NULL) return;

    rbnode *queue[1000];
    int front = 0, rear = 0;
    queue[rear++] = root;

    while (front < rear) {
        rbnode *current = queue[front++];
        fprintf(file, "%d ", current->key);

        if (current->left != NULL) {
            queue[rear++] = current->left;
        }
        if (current->right != NULL) {
            queue[rear++] = current->right;
        }
    }
}

int main(int argc, char *argv[]) { // main 함수, 시간복잡도 O(NlogN)
    if (argc != 3) { // 예외 처리
        printf("Usage: %s input.txt output.txt\n", argv[0]);
        return 1;
    }

    FILE *ptr_input = fopen(argv[1], "r");
    FILE *ptr_output = fopen(argv[2], "w");
    if (ptr_input == NULL || ptr_output == NULL) {
        printf("Error opening files.\n");
        return 1;
    }

    rbnode *root = NULL;
    rbnode *original_root = NULL;

    int num;
    while (fscanf(ptr_input, "%d", &num) == 1) {
        rbnode *new_node = createNode(num);
        insert(&root, new_node);
    }

    original_root = root;

    printInorder(root, ptr_output); // 문제 조건의 insert 후 In-Order 순회
    fprintf(ptr_output, "\n");
    printLevelorder(root, ptr_output); // Level-order 순회
    fprintf(ptr_output, "\n");

    rewind(ptr_input); // 이하는 3, 4번째 줄 출력을 위한 과정
    while (fgetc(ptr_input) != '\n') {}
    while (fscanf(ptr_input, "%d", &num) == 1) {
        rbnode *to_delete = NULL;
        to_delete = original_root;
        while (to_delete != NULL) {
            if (num == to_delete->key) {
                break;
            } else if (num < to_delete->key) {
                to_delete = to_delete->left;
            } else {
                to_delete = to_delete->right;
            }
        }
        if (to_delete != NULL) {
            delete(&root, to_delete);
        }
    }

    printInorder(root, ptr_output); // 문제 조건의 Delete 후 In-Order 순회
    fprintf(ptr_output, "\n");
    printLevelorder(root, ptr_output); // Level-Order 순회

    fclose(ptr_input); // 입출력파일 닫기
    fclose(ptr_output);

    return 0;
}
