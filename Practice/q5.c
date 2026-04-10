#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX 100

typedef struct {
    char res[10], op1[10], op2[10];
    char op;
    int isValid;
} TAC;

TAC t[MAX];
int n = 0;

int isNum(char *s) {
    for (int i = 0; s[i]; i++)
        if (!isdigit(s[i])) return 0;
    return 1;
}

void parse(char *line) {
    TAC x;
    char temp[10];

    int c = sscanf(line, "%s = %s %c %s", x.res, x.op1, &x.op, x.op2);

    if (c == 2) {
        sscanf(line, "%s = %s", x.res, x.op1);
        x.op = '=';
        strcpy(x.op2, "");
    }

    x.isValid = 1;
    t[n++] = x;
}

int compute(int a, int b, char op) {
    if (op == '+') return a + b;
    if (op == '-') return a - b;
    if (op == '*') return a * b;
    if (op == '/' && b != 0) return a / b;
    return 0;
}

void constantFolding() {
    for (int i = 0; i < n; i++) {
        if (t[i].op != '=' && isNum(t[i].op1) && isNum(t[i].op2)) {
            int val = compute(atoi(t[i].op1), atoi(t[i].op2), t[i].op);
            sprintf(t[i].op1, "%d", val);
            t[i].op = '=';
            strcpy(t[i].op2, "");
        }
    }
}

void constantPropagation() {
    for (int i = 0; i < n; i++) {
        if (t[i].op == '=' && isNum(t[i].op1)) {
            for (int j = i + 1; j < n; j++) {
                if (strcmp(t[j].op1, t[i].res) == 0)
                    strcpy(t[j].op1, t[i].op1);
                if (strcmp(t[j].op2, t[i].res) == 0)
                    strcpy(t[j].op2, t[i].op1);
            }
        }
    }
}

void copyPropagation() {
    for (int i = 0; i < n; i++) {
        if (t[i].op == '=' && !isNum(t[i].op1)) {
            for (int j = i + 1; j < n; j++) {
                if (strcmp(t[j].op1, t[i].res) == 0)
                    strcpy(t[j].op1, t[i].op1);
                if (strcmp(t[j].op2, t[i].res) == 0)
                    strcpy(t[j].op2, t[i].op1);
            }
        }
    }
}

void algebraicSimplification() {
    for (int i = 0; i < n; i++) {
        if (t[i].op == '+') {
            if (strcmp(t[i].op2, "0") == 0) t[i].op = '=';
            if (strcmp(t[i].op1, "0") == 0) {
                strcpy(t[i].op1, t[i].op2);
                t[i].op = '=';
            }
        }
        if (t[i].op == '*') {
            if (strcmp(t[i].op2, "1") == 0) t[i].op = '=';
            if (strcmp(t[i].op1, "1") == 0) {
                strcpy(t[i].op1, t[i].op2);
                t[i].op = '=';
            }
            if (strcmp(t[i].op1, "0") == 0 || strcmp(t[i].op2, "0") == 0) {
                strcpy(t[i].op1, "0");
                t[i].op = '=';
            }
        }
    }
}

void CSE() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (t[i].op == t[j].op &&
                strcmp(t[i].op1, t[j].op1) == 0 &&
                strcmp(t[i].op2, t[j].op2) == 0) {

                strcpy(t[i].op1, t[j].res);
                t[i].op = '=';
                strcpy(t[i].op2, "");
            }
        }
    }
}

int isUsed(int i) {
    for (int j = i + 1; j < n; j++) {
        if (strcmp(t[i].res, t[j].op1) == 0 ||
            strcmp(t[i].res, t[j].op2) == 0)
            return 1;
    }
    return 0;
}

void deadCodeElimination() {
    for (int i = 0; i < n - 1; i++) {
        if (!isUsed(i)) t[i].isValid = 0;
    }
}

void printFinal() {
    printf("\nOptimized TAC:\n");
    for (int i = 0; i < n; i++) {
        if (!t[i].isValid) continue;

        if (t[i].op == '=')
            printf("%s = %s\n", t[i].res, t[i].op1);
        else
            printf("%s = %s %c %s\n", t[i].res, t[i].op1, t[i].op, t[i].op2);
    }
}

int main() {
    char line[100];

    printf("Enter TAC (END to stop):\n");

    while (1) {
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "END") == 0) break;

        parse(line);
    }

    constantFolding();
    constantPropagation();
    CSE();
    copyPropagation();
    algebraicSimplification();
    deadCodeElimination();

    printFinal();
}