#include <stdio.h>
#include <string.h>

// Stack structure
typedef struct {
    char arr[50];
    int top;
} stack;

// Stack operations
void push(stack *s, char c) {
    s->arr[++s->top] = c;
}

char pop(stack *s) {
    return s->arr[s->top--];
}

char peek(stack *s) {
    return s->arr[s->top];
}

// Print stack
void printstack(stack *s) {
    for(int i = 0; i <= s->top; i++)
        printf("%c", s->arr[i]);
}

// pos() function
int pos(char arr[], int n, char c) {
    for(int i = 0; i < n; i++)
        if(arr[i] == c) return i;
    return -1;
}

// Precedence table: +, *, i, $
char optable[4][4] = {
    {'>', '<', '<', '>'}, // +
    {'>', '>', '<', '>'}, // *
    {'>', '>', 'E', '>'}, // i
    {'<', '<', '<', 'A'}  // $
};

char symbols[] = {'+', '*', 'i', '$'};

int main() {
    stack s;
    s.top = -1;

    char input[50];
    int i = 0;

    printf("Enter input (use i for id, end with $): ");
    scanf("%s", input);

    push(&s, '$');

    printf("\nStack\tInput\tAction\n");

    while(1) {
        printstack(&s);
        printf("\t%s\t", &input[i]);

        char top = peek(&s);
        char a = input[i];

        int row = pos(symbols, 4, top);
        int col = pos(symbols, 4, a);

        if(row == -1 || col == -1) {
            printf("Error\n");
            break;
        }

        char rel = optable[row][col];

        // SHIFT
        if(rel == '<') {
            printf("Shift\n");
            push(&s, a);
            i++;
        }

        // REDUCE (simplified)
        else if(rel == '>') {
            printf("Reduce\n");
            pop(&s);
        }

        // ACCEPT
        else if(rel == 'A') {
            printf("Accept\n");
            break;
        }

        // ERROR
        else {
            printf("Error\n");
            break;
        }
    }

    return 0;
}