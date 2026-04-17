#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
    OP_BIN,
    OP_UN,
    OP_IF,
    OP_GOTO,
    OP_LABEL,
    OP_OTHER
} stmt_type;

typedef struct {
    char res[20], op1[20], op2[20], relop[5];
    char op;
    stmt_type type;
    int used;
} tac;
/*
a = 10
b = 20
t1 = a + b
t2 = a + b
t3 = t2 * 1
if a < b goto L1
goto L2
L1:
x = t3
L2:
END
*/
tac t[100];
int n = 0;

int isnumber(char* s) {
    if (!s || strlen(s) == 0) return 0;
    int start = 0;
    if (s[0] == '-') {
        if (strlen(s) == 1) return 0;
        start = 1;
    }
    for (int i = start; s[i]; i++) {
        if (!isdigit(s[i])) return 0;
    }
    return 1;
}

int compute(int a, int b, char op) {
    if (op == '+') return a + b;
    if (op == '-') return a - b;
    if (op == '*') return a * b;
    if (op == '/' && b != 0) return a / b;
    return 0;
}

void parse(char* line) {
    t[n].used = 1;
    t[n].op = ' ';
    strcpy(t[n].op1, "");
    strcpy(t[n].op2, "");
    strcpy(t[n].res, "");
    strcpy(t[n].relop, "");

    if (line[strlen(line) - 1] == ':') {
        sscanf(line, "%[^:]", t[n].res);
        t[n].type = OP_LABEL;
    }
    else if (strncmp(line, "if", 2) == 0) {
        sscanf(line, "if %s %s %s goto %s", t[n].op1, t[n].relop, t[n].op2, t[n].res);
        t[n].type = OP_IF;
    }
    else if (strncmp(line, "goto", 4) == 0) {
        sscanf(line, "goto %s", t[n].res);
        t[n].type = OP_GOTO;
    }
    else {
        int count = sscanf(line, "%s = %s %c %s", t[n].res, t[n].op1, &t[n].op, t[n].op2);
        if (count == 4) {
            t[n].type = OP_BIN;
        } else if (count == 2) {
            t[n].type = OP_UN;
        } else {
            t[n].type = OP_OTHER;
        }
    }
    n++;
}

void print_tac() {
    for (int i = 0; i < n; i++) {
        if (t[i].used) {
            switch (t[i].type) {
                case OP_LABEL:
                    printf("%s:\n", t[i].res);
                    break;
                case OP_GOTO:
                    printf("goto %s\n", t[i].res);
                    break;
                case OP_IF:
                    printf("if %s %s %s goto %s\n", t[i].op1, t[i].relop, t[i].op2, t[i].res);
                    break;
                case OP_BIN:
                    printf("%s = %s %c %s\n", t[i].res, t[i].op1, t[i].op, t[i].op2);
                    break;
                case OP_UN:
                    printf("%s = %s\n", t[i].res, t[i].op1);
                    break;
                default:
                    break;
            }
        }
    }
    printf("-------------------------\n");
}

int constant_folding() {
    printf("Constant Folding:\n");
    for (int i = 0; i < n; i++) {
        if (t[i].type == OP_BIN && isnumber(t[i].op1) && isnumber(t[i].op2)) {
            sprintf(t[i].op1, "%d", compute(atoi(t[i].op1), atoi(t[i].op2), t[i].op));
            t[i].type = OP_UN;
            t[i].op = ' ';
            strcpy(t[i].op2, "");
        }
    }
    print_tac();
    return 0;
}

int constant_propagation() {
    printf("Constant Propagation:\n");
    for (int i = 0; i < n; i++) {
        if (t[i].type == OP_UN && isnumber(t[i].op1)) {
            for (int j = i + 1; j < n; j++) {
                if (t[j].type == OP_LABEL || t[j].type == OP_GOTO || t[j].type == OP_IF) break;
                if (strcmp(t[i].res, t[j].res) == 0) break;

                if (strcmp(t[i].res, t[j].op1) == 0) strcpy(t[j].op1, t[i].op1);
                if (strcmp(t[i].res, t[j].op2) == 0) strcpy(t[j].op2, t[i].op1);
            }
        }
    }
    print_tac();
    return 0;
}

int copy_propagation() {
    printf("Copy Propagation:\n");
    for (int i = 0; i < n; i++) {
        if (t[i].type == OP_UN && !isnumber(t[i].op1)) {
            for (int j = i + 1; j < n; j++) {
                if (t[j].type == OP_LABEL || t[j].type == OP_GOTO || t[j].type == OP_IF) break;
                if (strcmp(t[i].res, t[j].res) == 0) break;

                if (strcmp(t[i].res, t[j].op1) == 0) strcpy(t[j].op1, t[i].op1);
                if (strcmp(t[i].res, t[j].op2) == 0) strcpy(t[j].op2, t[i].op1);
            }
        }
    }
    print_tac();
    return 0;
}

int cse() {
    printf("Common Subexpression Elimination (CSE):\n");
    for (int i = 0; i < n; i++) {
        if (t[i].type != OP_BIN) continue;

        for (int j = i + 1; j < n; j++) {
            if (t[j].type == OP_LABEL || t[j].type == OP_GOTO || t[j].type == OP_IF) break;

            if (strcmp(t[i].res, t[j].op1) == 0 || strcmp(t[i].res, t[j].op2) == 0) break;
            if (strcmp(t[i].op1, t[j].res) == 0 || strcmp(t[i].op2, t[j].res) == 0) break;

            if (t[j].type == OP_BIN && t[i].op == t[j].op) {
                int match_direct = (strcmp(t[i].op1, t[j].op1) == 0 && strcmp(t[i].op2, t[j].op2) == 0);
                int match_commute = (strcmp(t[i].op1, t[j].op2) == 0 && strcmp(t[i].op2, t[j].op1) == 0);

                if (match_direct || (match_commute && (t[i].op == '+' || t[i].op == '*'))) {
                    strcpy(t[j].op1, t[i].res);
                    t[j].type = OP_UN;
                    t[j].op = ' ';
                    strcpy(t[j].op2, "");
                }
            }
        }
    }
    print_tac();
    return 0;
}

int algebraic_simplification() {
    printf("Algebraic Simplification:\n");
    for (int i = 0; i < n; i++) {
        if (t[i].type == OP_BIN) {
            if (t[i].op == '+') {
                if (strcmp(t[i].op1, "0") == 0) {
                    strcpy(t[i].op1, t[i].op2);
                    t[i].type = OP_UN;
                } else if (strcmp(t[i].op2, "0") == 0) {
                    t[i].type = OP_UN;
                }
            }
            else if (t[i].op == '*') {
                if (strcmp(t[i].op1, "1") == 0) {
                    strcpy(t[i].op1, t[i].op2);
                    t[i].type = OP_UN;
                } else if (strcmp(t[i].op2, "1") == 0) {
                    t[i].type = OP_UN;
                } else if (strcmp(t[i].op1, "0") == 0 || strcmp(t[i].op2, "0") == 0) {
                    strcpy(t[i].op1, "0");
                    t[i].type = OP_UN;
                }
            }
            if (t[i].type == OP_UN) {
                t[i].op = ' ';
                strcpy(t[i].op2, "");
            }
        }
    }
    print_tac();
    return 0;
}

int main() {
    char line[100];
    printf("Enter TAC (type 'END' to stop):\n");

    while (1) {
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "END") == 0) break;
        if (strlen(line) > 0) parse(line);
    }

    printf("\nOriginal TAC:\n");
    print_tac();

    constant_folding();
    constant_propagation();
    constant_folding();
    cse();
    copy_propagation();
    algebraic_simplification();

    return 0;
}