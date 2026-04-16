#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    int state[100];
    int top;
} stack;

void push(stack *s, int x) {
    s->state[++s->top] = x;
}

int pop(stack *s) {
    return s->state[s->top--];
}

int peek(stack *s) {
    return s->state[s->top];
}

void printstack(stack *s) {
    for(int i = 0; i <= s->top; i++)
        printf("%d", s->state[i]);
}

// Productions
// 1: S → CC
// 2: C → cC
// 3: C → d

char lhs[] = {' ', 'S', 'C', 'C'};
char rhs_len[] = {0, 2, 2, 1};

int main() {

    char input[100];
    printf("Enter input (end with $): ");
    scanf("%s", input);

    // ACTION table
    char action[7][3][10] = {
        {"S3","S4",""},     // 0
        {"","","acc"},      // 1
        {"S3","S4",""},     // 2
        {"S3","S4",""},     // 3
        {"R3","R3","R3"},   // 4
        {"R1","R1","R1"},   // 5
        {"R2","R2","R2"}    // 6
    };

    // GOTO table
    int go[7][2] = {
        {1,2},   // 0
        {-1,-1},
        {-1,5},
        {-1,6},
        {-1,-1},
        {-1,-1},
        {-1,-1}
    };

    char terminals[] = {'c','d','$'};
    char nonterminals[] = {'S','C'};

    stack s;
    s.top = -1;
    push(&s, 0);

    int i = 0;

    printf("\nStack\tInput\tAction\n");

    while(1) {
        int state = peek(&s);
        char a = input[i];

        // find terminal index
        int col = -1;
        for(int j=0;j<3;j++)
            if(terminals[j]==a) col=j;

        if(col == -1) {
            printf("Error\n");
            return 0;
        }

        char *act = action[state][col];

        printstack(&s);
        printf("\t%s\t", &input[i]);

        // SHIFT
        if(act[0] == 'S') {
            int next = act[1] - '0';
            printf("Shift %d\n", next);
            push(&s, next);
            i++;
        }

        // REDUCE
        else if(act[0] == 'R') {
            int prod = act[1] - '0';
            printf("Reduce by %c -> (len %d)\n", lhs[prod], rhs_len[prod]);

            for(int k=0;k<rhs_len[prod];k++)
                pop(&s);

            int top = peek(&s);

            // find non-terminal column
            int col2 = -1;
            for(int j=0;j<2;j++)
                if(nonterminals[j]==lhs[prod]) col2=j;

            push(&s, go[top][col2]);
        }

        else if(strcmp(act,"acc")==0) {
            printf("Accept\n");
            break;
        }

        else {
            printf("Error\n");
            return 0;
        }
    }

    return 0;
}