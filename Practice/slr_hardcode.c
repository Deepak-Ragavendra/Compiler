#include <stdio.h>
#include <string.h>

int stack[100];
int top = -1;

char input[100];
int ip = 0;

/* Push */
void push(int x) {
    stack[++top] = x;
}

/* Pop */
int pop() {
    return stack[top--];
}

/* ACTION Table */
char* ACTION[12][6] = {
/* i     +     *     (     )     $  */
{"S5", "", "", "S4", "", ""},      // 0
{"", "S6", "", "", "", "acc"},     // 1
{"", "R2", "S7", "", "R2", "R2"},   // 2
{"", "R4", "R4", "", "R4", "R4"},   // 3
{"S5", "", "", "S4", "", ""},      // 4
{"", "R6", "R6", "", "R6", "R6"},   // 5
{"S5", "", "", "S4", "", ""},      // 6
{"S5", "", "", "S4", "", ""},      // 7
{"", "S6", "", "", "S11", ""},     // 8
{"", "R1", "S7", "", "R1", "R1"},   // 9
{"", "R3", "R3", "", "R3", "R3"},   // 10
{"", "R5", "R5", "", "R5", "R5"}    // 11
};

/* GOTO Table */
int GOTO[12][3] = {
/* E  T  F */
{1, 2, 3},
{-1, -1, -1},
{-1, -1, -1},
{-1, -1, -1},
{8, 2, 3},
{-1, -1, -1},
{-1, 9, 3},
{-1, -1, 10},
{-1, -1, -1},
{-1, -1, -1},
{-1, -1, -1},
{-1, -1, -1}
};

/* Production lengths */
int prod_len[] = {0, 3, 1, 3, 1, 3, 1};
char prod_lhs[] = {' ', 'E', 'E', 'T', 'T', 'F', 'F'};

int getCol(char c) {
    switch (c) {
        case 'i': return 0;
        case '+': return 1;
        case '*': return 2;
        case '(': return 3;
        case ')': return 4;
        case '$': return 5;
    }
    return -1;
}

int getGotoCol(char c) {
    if (c == 'E') return 0;
    if (c == 'T') return 1;
    if (c == 'F') return 2;
    return -1;
}

int main() {
    printf("Enter input string (end with $): ");
    scanf("%s", input);

    push(0);

    while (1) {
        int state = stack[top];
        char a = input[ip];
        char* act = ACTION[state][getCol(a)];

        if (act[0] == 'S') {
            int next = act[1] - '0';
            push(next);
            ip++;
        }
        else if (act[0] == 'R') {
            int prod = act[1] - '0';
            for (int i = 0; i < prod_len[prod]; i++)
                pop();

            int t = stack[top];
            int go = GOTO[t][getGotoCol(prod_lhs[prod])];
            push(go);
        }
        else if (strcmp(act, "acc") == 0) {
            printf("Accepted\n");
            return 0;
        }
        else {
            printf("Rejected\n");
            return 0;
        }
    }
}