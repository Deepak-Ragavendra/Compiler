#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX 100
#define LEN 50

typedef struct {
    char res[LEN], op1[LEN], op2[LEN];
    char op;          // '=', '+', '-', '*', '/'
    int isValid;
} TAC;

TAC t[MAX];
int n = 0;

int isNum(const char *s) {
    int i = 0;
    if (s[0] == '\0') return 0;
    if (s[0] == '-') i = 1;
    if (s[i] == '\0') return 0;

    for (; s[i]; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

int isArithmeticOp(char op) {
    return (op == '+' || op == '-' || op == '*' || op == '/');
}

int compute(int a, int b, char op, int *ok) {
    *ok = 1;
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) {
                *ok = 0;
                return 0;
            }
            return a / b;
        default:
            *ok = 0;
            return 0;
    }
}

void parse(char *line) {
    TAC x;
    char eq;

    x.res[0] = x.op1[0] = x.op2[0] = '\0';
    x.op = '=';
    x.isValid = 1;

    int c = sscanf(line, "%s %c %s %c %s", x.res, &eq, x.op1, &x.op, x.op2);

    if (c == 3 && eq == '=') {
        x.op = '=';
        x.op2[0] = '\0';
    } else if (!(c == 5 && eq == '=' && isArithmeticOp(x.op))) {
        printf("Invalid TAC skipped: %s\n", line);
        return;
    }

    if (n < MAX) {
        t[n++] = x;
    }
}

int sameExpr(TAC a, TAC b) {
    if (!a.isValid || !b.isValid) return 0;
    if (a.op != b.op) return 0;
    if (a.op == '=') return 0;

    if (strcmp(a.op1, b.op1) == 0 && strcmp(a.op2, b.op2) == 0)
        return 1;

    // handle commutative operators
    if ((a.op == '+' || a.op == '*') &&
        strcmp(a.op1, b.op2) == 0 &&
        strcmp(a.op2, b.op1) == 0)
        return 1;

    return 0;
}

int constantFolding() {
    int changed = 0;

    for (int i = 0; i < n; i++) {
        if (!t[i].isValid) continue;

        if (t[i].op != '=' && isNum(t[i].op1) && isNum(t[i].op2)) {
            int ok;
            int val = compute(atoi(t[i].op1), atoi(t[i].op2), t[i].op, &ok);

            if (ok) {
                char temp[LEN];
                sprintf(temp, "%d", val);

                if (strcmp(t[i].op1, temp) != 0 || t[i].op != '=' || t[i].op2[0] != '\0') {
                    strcpy(t[i].op1, temp);
                    t[i].op = '=';
                    t[i].op2[0] = '\0';
                    changed = 1;
                }
            }
        }
    }
    return changed;
}

int algebraicSimplification() {
    int changed = 0;

    for (int i = 0; i < n; i++) {
        if (!t[i].isValid) continue;

        if (t[i].op == '+') {
            if (strcmp(t[i].op2, "0") == 0) {
                t[i].op = '=';
                t[i].op2[0] = '\0';
                changed = 1;
            } else if (strcmp(t[i].op1, "0") == 0) {
                strcpy(t[i].op1, t[i].op2);
                t[i].op = '=';
                t[i].op2[0] = '\0';
                changed = 1;
            }
        } else if (t[i].op == '-') {
            if (strcmp(t[i].op2, "0") == 0) {
                t[i].op = '=';
                t[i].op2[0] = '\0';
                changed = 1;
            }
        } else if (t[i].op == '*') {
            if (strcmp(t[i].op1, "0") == 0 || strcmp(t[i].op2, "0") == 0) {
                strcpy(t[i].op1, "0");
                t[i].op = '=';
                t[i].op2[0] = '\0';
                changed = 1;
            } else if (strcmp(t[i].op2, "1") == 0) {
                t[i].op = '=';
                t[i].op2[0] = '\0';
                changed = 1;
            } else if (strcmp(t[i].op1, "1") == 0) {
                strcpy(t[i].op1, t[i].op2);
                t[i].op = '=';
                t[i].op2[0] = '\0';
                changed = 1;
            }
        } else if (t[i].op == '/') {
            if (strcmp(t[i].op2, "1") == 0) {
                t[i].op = '=';
                t[i].op2[0] = '\0';
                changed = 1;
            } else if (strcmp(t[i].op1, "0") == 0 && strcmp(t[i].op2, "0") != 0) {
                strcpy(t[i].op1, "0");
                t[i].op = '=';
                t[i].op2[0] = '\0';
                changed = 1;
            }
        }
    }

    return changed;
}

int constantPropagation() {
    int changed = 0;

    for (int i = 0; i < n; i++) {
        if (!t[i].isValid) continue;

        if (t[i].op == '=' && isNum(t[i].op1)) {
            char var[LEN], val[LEN];
            strcpy(var, t[i].res);
            strcpy(val, t[i].op1);

            for (int j = i + 1; j < n; j++) {
                if (!t[j].isValid) continue;

                // stop if variable is redefined
                if (strcmp(t[j].res, var) == 0) break;

                if (strcmp(t[j].op1, var) == 0) {
                    strcpy(t[j].op1, val);
                    changed = 1;
                }
                if (t[j].op != '=' && strcmp(t[j].op2, var) == 0) {
                    strcpy(t[j].op2, val);
                    changed = 1;
                }
            }
        }
    }

    return changed;
}

int copyPropagation() {
    int changed = 0;

    for (int i = 0; i < n; i++) {
        if (!t[i].isValid) continue;

        if (t[i].op == '=' && !isNum(t[i].op1)) {
            char var[LEN], val[LEN];
            strcpy(var, t[i].res);
            strcpy(val, t[i].op1);

            for (int j = i + 1; j < n; j++) {
                if (!t[j].isValid) continue;

                // stop if variable is redefined
                if (strcmp(t[j].res, var) == 0) break;

                if (strcmp(t[j].op1, var) == 0) {
                    strcpy(t[j].op1, val);
                    changed = 1;
                }
                if (t[j].op != '=' && strcmp(t[j].op2, var) == 0) {
                    strcpy(t[j].op2, val);
                    changed = 1;
                }
            }
        }
    }

    return changed;
}

int CSE() {
    int changed = 0;

    for (int i = 0; i < n; i++) {
        if (!t[i].isValid || t[i].op == '=') continue;

        for (int j = 0; j < i; j++) {
            if (!t[j].isValid) continue;

            if (sameExpr(t[i], t[j])) {
                strcpy(t[i].op1, t[j].res);
                t[i].op = '=';
                t[i].op2[0] = '\0';
                changed = 1;
                break;
            }
        }
    }

    return changed;
}

int isUsed(int idx) {
    if (!t[idx].isValid) return 0;

    for (int j = idx + 1; j < n; j++) {
        if (!t[j].isValid) continue;

        if (strcmp(t[idx].res, t[j].op1) == 0) return 1;
        if (t[j].op != '=' && strcmp(t[idx].res, t[j].op2) == 0) return 1;
    }
    return 0;
}

int deadCodeElimination() {
    int changed = 0;

    // Keep the last valid statement as output statement
    for (int i = 0; i < n - 1; i++) {
        if (t[i].isValid && !isUsed(i)) {
            t[i].isValid = 0;
            changed = 1;
        }
    }

    return changed;
}

void optimize() {
    int changed;

    do {
        changed = 0;
        changed |= constantFolding();
        changed |= constantPropagation();
        changed |= copyPropagation();
        changed |= algebraicSimplification();
        changed |= constantFolding();       // new opportunities after propagation/simplification
        changed |= constantPropagation();
        changed |= copyPropagation();
        changed |= CSE();
    } while (changed);

    while (deadCodeElimination()) {
        do {
            changed = 0;
            changed |= constantFolding();
            changed |= constantPropagation();
            changed |= copyPropagation();
            changed |= algebraicSimplification();
            changed |= constantFolding();
            changed |= constantPropagation();
            changed |= copyPropagation();
            changed |= CSE();
        } while (changed);
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
        if (!fgets(line, sizeof(line), stdin)) break;

        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "END") == 0) break;
        if (strlen(line) == 0) continue;

        parse(line);
    }

    optimize();
    printFinal();

    return 0;
}