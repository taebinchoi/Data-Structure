#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Kernel/stdbool.h> // 필요한 헤더파일 선언
#define CAPACITY 1000000 // 공간 지정

typedef struct Node{ // 구조체 정의
    int element;
    struct Node* next;
} Node;

typedef struct Stack{ // 구조체 정의
    Node *top; 
} Stack;

Stack* createStack() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->top = NULL;
    return stack;
} // 재확인

int isEmpty(Stack* stack) { // 비어있는지 여부 체크
    return (stack->top == NULL);
}

int push(Stack *stack, int element){ // time complexity = O(1)
    Node *node = (Node*) malloc(sizeof(Node));
    if (node == NULL){
        printf("Overflow\n");
        return -1;
    }

    node->element = element;
    node->next = stack->top;
    stack->top = node;

    return 0;
}

int pop(Stack *stack){ // time complexity = O(1), 꺼내오기
    if (stack->top == NULL){
        printf("Underflow\n");
        return -CAPACITY;
    }
    Node *node = stack->top;
    int element = node->element;
    stack->top = node->next;
    free(node);

    return element;
}

int top(Stack *stack){ // time complexity = O(1), 가장 상단의 요소 꺼내기(Peek)
    if (isEmpty(stack)){
        printf("Underflow\n");
        return -1;
    }
    return stack->top->element;
}

int isPalindrome(Stack *stack, char word[], int length){ // time complexity = O(n), 펠린드롬 수 여부 체크
    int i;
    for (i=0; i<length/2; i++)
        push(stack, word[i]);
    if (length%2 == 1)
        i++;
    while (i < length){
        char character = pop(stack);
        if (character != word[i])
            return 0;
        i++;
    }
    return 1;
}

int isMatched(char character1, char character2){ // 2개의 괄호 종류가 일치하면 1을 반환
    if (character1 == '(' && character2 == ')')
        return 1;
    else if (character1 == '{' && character2 == '}')
        return 1;
    else if (character1 == '[' && character2 == ']')
        return 1;
    else
        return 0;    
}

int isBalanced(char exp[], int length) {
    Stack* stack = createStack();
    for (int i = 0; i < length; i++) {
        if (exp[i] == '(' || exp[i] == '[' || exp[i] == '{') {
            push(stack, exp[i]);
        } else if (exp[i] == ')' || exp[i] == ']' || exp[i] == '}') {
            if (isEmpty(stack) || !isMatched(pop(stack), exp[i])) {
                free(stack);
                return 0;
            }
        }
    }
    int result = isEmpty(stack); // 스택이 비어있으면 1, 아니면 0
    free(stack);
    return result;
}

int main(int argc, char *argv[]){ // time complexity = O(n), main 함수
    Stack stack;
    stack.top = NULL;

    FILE *ptr_input = fopen(argv[1], "r"); // 파일 입출력 관련
    FILE *ptr_output = fopen(argv[2], "w");
    if (ptr_input == NULL || ptr_output == NULL){
        return 1;
    }

    char command;
    int value;
    char string[100];
    char line[CAPACITY];
    
    while (fscanf(ptr_input, " %c", &command) == 1){ // 입력받는 문자에 따라 출력값 다르게
        switch (command){
            case 'H': // Push
                fscanf(ptr_input, "\t%d", &value);
                if (push(&stack, value) == -1) {
                    fprintf(ptr_output, "Overflow\n"); // 스택이 꽉 차있을때 Overflow 출력
                } else {
                // 스택의 모든 요소를 출력 파일에 기록
                Node* current = stack.top;
                while (current != NULL) {
                    fprintf(ptr_output, "%d ", current->element);
                    current = current->next;
                }
                fprintf(ptr_output, "\n");
                }
                break;

            case 'O': // Pop
                value = pop(&stack);
                if (value == -1){
                    fprintf(ptr_output, "Underflow\n"); // 스택이 비어있을 때 Underflow 출력
                } else{
                // 스택의 모든 요소를 출력 파일에 기록
                Node* current = stack.top;
                while (current != NULL){
                    fprintf(ptr_output, "%d ", current->element);
                    current = current->next;
                }
                    fprintf(ptr_output, "\n");
                }
                break;
            case 'T': // Top (Peek)
                value = top(&stack);
                if (value == -1){
                    fprintf(ptr_output, "Underflow\n"); // 스택이 비어있을 때 Underflow 출력
                } else{
                    fprintf(ptr_output, "%d\n", value);
                }
                break;
            case 'P': // Palindrome
                fscanf(ptr_input, " %[^\t\n]", string);
                if (isPalindrome(&stack, string, strlen(string))){
                    fprintf(ptr_output, "T\n");
                } else{
                    fprintf(ptr_output, "F\n");
                }
                break;
            case 'B': // Balanced
                // fscanf(ptr_input, " %[^\t\n]", string);
                while (fgets(line, CAPACITY, ptr_input) != NULL){
                    if (isBalanced(line, strlen(line))){
                        fprintf(ptr_output, "T\n");
                    } else {
                        fprintf(ptr_output, "F\n");
                    }
                }
            default:
                break;
        }
    }

    fclose(ptr_input); // 입출력 파일 닫기
    fclose(ptr_output);

    return 0;
}
