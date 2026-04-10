#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char result[10];
    char op1[10];
    char op2[10];
    char op;
} Expr;

int isNumber(char *s) {
    for (int i = 0; s[i]; i++) {
        if (s[i] < '0' || s[i] > '9')
            return 0;
    }
    return 1;
}

void parse(char *line, Expr *e) {
    char temp1[10], temp2[10], temp3[10];
    int count = sscanf(line, "%s = %s %s %s", e->result, temp1, temp2, temp3);
    if (count == 2) {
        strcpy(e->op1, temp1);
        e->op = '=';
        strcpy(e->op2, "");
    }
    else if (count == 4) {
        strcpy(e->op1, temp1);
        e->op = temp2[0];
        strcpy(e->op2, temp3);
    }
}

void printExpr(Expr e) {
    if (e.op == '=')
        printf("%s = %s\n", e.result, e.op1);
    else
        printf("%s = %s %c %s\n", e.result, e.op1, e.op, e.op2);
}

void constantFolding(Expr e[], int n) {
    printf("\nConstant Folding:\n");
    for (int i = 0; i < n; i++) {
        if (isNumber(e[i].op1) && isNumber(e[i].op2)) {
            int a = atoi(e[i].op1);
            int b = atoi(e[i].op2);
            int res;
            switch (e[i].op) {
                case '+': res = a + b; break;
                case '-': res = a - b; break;
                case '*': res = a * b; break;
                case '/': res = (b != 0) ? a / b : 0; break;
                default: continue;
            }
            sprintf(e[i].op1, "%d", res);
            strcpy(e[i].op2, "");
            e[i].op = '=';
        }
        printExpr(e[i]);
    }
}

void commonSubexpressionElimination(Expr e[], int n) {
    printf("\nCommon Subexpression Elimination:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (e[i].op != '=' &&
                strcmp(e[i].op1, e[j].op1) == 0 &&
                strcmp(e[i].op2, e[j].op2) == 0 &&
                e[i].op == e[j].op) {

                strcpy(e[i].op1, e[j].result);
                strcpy(e[i].op2, "");
                e[i].op = '=';
                break;
            }
        }
        printExpr(e[i]);
    }
}

void copyPropagation(Expr e[], int n) {
    printf("\nCopy Propagation:\n");
    for (int i = 0; i < n; i++) {
        if (e[i].op == '=') {
            for (int j = i + 1; j < n; j++) {
                if (strcmp(e[j].op1, e[i].result) == 0)
                    strcpy(e[j].op1, e[i].op1);

                if (strcmp(e[j].op2, e[i].result) == 0)
                    strcpy(e[j].op2, e[i].op1);
            }
        }
    }
    for (int i = 0; i < n; i++)
        printExpr(e[i]);
}

void algebraicSimplification(Expr e[], int n) {
    printf("\nAlgebraic Simplification:\n");
    for (int i = 0; i < n; i++) {
        if (e[i].op == '+') {
            if (strcmp(e[i].op2, "0") == 0) {
                e[i].op = '=';
                strcpy(e[i].op2, "");
            }
        }
        if (e[i].op == '*') {
            if (strcmp(e[i].op2, "1") == 0) {
                e[i].op = '=';
                strcpy(e[i].op2, "");
            }
            else if (strcmp(e[i].op2, "0") == 0) {
                e[i].op = '=';
                strcpy(e[i].op1, "0");
                strcpy(e[i].op2, "");
            }
        }
        printExpr(e[i]);
    }
}

void deadCodeElimination(Expr e[], int n) {
    printf("\nDead Code Elimination:\n");
    for (int i = 0; i < n; i++) {
        int used = 0;
        for (int j = i + 1; j < n; j++) {
            if (strcmp(e[i].result, e[j].op1) == 0 ||
                strcmp(e[i].result, e[j].op2) == 0) {
                used = 1;
                break;
            }
        }
        if (used || i == n - 1)
            printExpr(e[i]);
        else
            printf("%s eliminated\n", e[i].result);
    }
}

int main() {
    int n;
    char line[50];
    Expr e[20];

    printf("Enter number of expressions: ");
    scanf("%d", &n);
    getchar();
    printf("Enter statements:\n");
    for (int i = 0; i < n; i++) {
        fgets(line, sizeof(line), stdin);
        parse(line, &e[i]);
    }

    constantFolding(e, n);
    commonSubexpressionElimination(e, n);
    copyPropagation(e, n);
    algebraicSimplification(e, n);
    deadCodeElimination(e, n);
    return 0;
}