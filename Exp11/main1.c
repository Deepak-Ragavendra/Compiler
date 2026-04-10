#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_INS 200
#define MAX_LEN 50

typedef struct {
    char lhs[MAX_LEN];
    char op1[MAX_LEN];
    char op2[MAX_LEN];
    char op[3];
    int isBinary;
    int removed;
} Instr;

typedef struct {
    char name[MAX_LEN];
    char value[MAX_LEN];
} Mapping;

typedef struct {
    char expr[3 * MAX_LEN];
    char var[MAX_LEN];
} ExprMap;

Instr ins[MAX_INS];
int n = 0;

Mapping constMap[200], copyMap[200];
int constCount = 0, copyCount = 0;

ExprMap exprMap[200];
int exprCount = 0;

int isNumber(char s[]) {
    int i = 0;
    if (s[0] == '\0') return 0;
    if (s[0] == '-' && s[1] != '\0') i = 1;
    for (; s[i]; i++) {
        if (!isdigit(s[i])) return 0;
    }
    return 1;
}

int findConst(char name[]) {
    for (int i = 0; i < constCount; i++) {
        if (strcmp(constMap[i].name, name) == 0) return i;
    }
    return -1;
}

int findCopy(char name[]) {
    for (int i = 0; i < copyCount; i++) {
        if (strcmp(copyMap[i].name, name) == 0) return i;
    }
    return -1;
}

void setConst(char name[], char value[]) {
    int i = findConst(name);
    if (i == -1) {
        strcpy(constMap[constCount].name, name);
        strcpy(constMap[constCount].value, value);
        constCount++;
    } else {
        strcpy(constMap[i].value, value);
    }
}

void removeConst(char name[]) {
    int i = findConst(name);
    if (i != -1) {
        for (int j = i; j < constCount - 1; j++) constMap[j] = constMap[j + 1];
        constCount--;
    }
}

void setCopy(char name[], char value[]) {
    int i = findCopy(name);
    if (i == -1) {
        strcpy(copyMap[copyCount].name, name);
        strcpy(copyMap[copyCount].value, value);
        copyCount++;
    } else {
        strcpy(copyMap[i].value, value);
    }
}

void removeCopy(char name[]) {
    int i = findCopy(name);
    if (i != -1) {
        for (int j = i; j < copyCount - 1; j++) copyMap[j] = copyMap[j + 1];
        copyCount--;
    }
}

void clearMappingsFor(char name[]) {
    removeConst(name);
    removeCopy(name);
}

void resolveOperand(char s[]) {
    int changed = 1;
    char temp[MAX_LEN];

    while (changed) {
        changed = 0;
        int ci = findConst(s);
        if (ci != -1) {
            strcpy(temp, constMap[ci].value);
            strcpy(s, temp);
            changed = 1;
            continue;
        }

        int cp = findCopy(s);
        if (cp != -1) {
            strcpy(temp, copyMap[cp].value);
            if (strcmp(temp, s) != 0) {
                strcpy(s, temp);
                changed = 1;
            }
        }
    }
}

int evalExpr(int a, int b, char op[]) {
    if (strcmp(op, "+") == 0) return a + b;
    if (strcmp(op, "-") == 0) return a - b;
    if (strcmp(op, "*") == 0) return a * b;
    if (strcmp(op, "/") == 0) return (b != 0) ? a / b : 0;
    return 0;
}

void makeExprKey(char key[], char a[], char op[], char b[]) {
    char x[MAX_LEN], y[MAX_LEN];

    strcpy(x, a);
    strcpy(y, b);

    if ((strcmp(op, "+") == 0 || strcmp(op, "*") == 0) && strcmp(x, y) > 0) {
        char t[MAX_LEN];
        strcpy(t, x);
        strcpy(x, y);
        strcpy(y, t);
    }

    sprintf(key, "%s|%s|%s", x, op, y);
}

int findExpr(char key[]) {
    for (int i = 0; i < exprCount; i++) {
        if (strcmp(exprMap[i].expr, key) == 0) return i;
    }
    return -1;
}

void killExprUsingVar(char var[]) {
    for (int i = 0; i < exprCount; ) {
        if (strstr(exprMap[i].expr, var) != NULL || strcmp(exprMap[i].var, var) == 0) {
            for (int j = i; j < exprCount - 1; j++) exprMap[j] = exprMap[j + 1];
            exprCount--;
        } else {
            i++;
        }
    }
}

int isTemp(char s[]) {
    return s[0] == 't';
}

int usedLater(int idx, char var[]) {
    for (int i = idx + 1; i < n; i++) {
        if (ins[i].removed) continue;
        if (strcmp(ins[i].op1, var) == 0) return 1;
        if (ins[i].isBinary && strcmp(ins[i].op2, var) == 0) return 1;
    }
    return 0;
}

void optimize() {
    for (int i = 0; i < n; i++) {
        if (ins[i].removed) continue;

        killExprUsingVar(ins[i].lhs);
        clearMappingsFor(ins[i].lhs);

        resolveOperand(ins[i].op1);
        if (ins[i].isBinary) resolveOperand(ins[i].op2);

        if (ins[i].isBinary) {
            if (strcmp(ins[i].op, "+") == 0) {
                if (strcmp(ins[i].op1, "0") == 0) {
                    ins[i].isBinary = 0;
                    strcpy(ins[i].op1, ins[i].op2);
                    ins[i].op2[0] = '\0';
                    ins[i].op[0] = '\0';
                } else if (strcmp(ins[i].op2, "0") == 0) {
                    ins[i].isBinary = 0;
                    ins[i].op2[0] = '\0';
                    ins[i].op[0] = '\0';
                }
            } else if (strcmp(ins[i].op, "-") == 0) {
                if (strcmp(ins[i].op2, "0") == 0) {
                    ins[i].isBinary = 0;
                    ins[i].op2[0] = '\0';
                    ins[i].op[0] = '\0';
                }
            } else if (strcmp(ins[i].op, "*") == 0) {
                if (strcmp(ins[i].op1, "1") == 0) {
                    ins[i].isBinary = 0;
                    strcpy(ins[i].op1, ins[i].op2);
                    ins[i].op2[0] = '\0';
                    ins[i].op[0] = '\0';
                } else if (strcmp(ins[i].op2, "1") == 0) {
                    ins[i].isBinary = 0;
                    ins[i].op2[0] = '\0';
                    ins[i].op[0] = '\0';
                } else if (strcmp(ins[i].op1, "0") == 0 || strcmp(ins[i].op2, "0") == 0) {
                    ins[i].isBinary = 0;
                    strcpy(ins[i].op1, "0");
                    ins[i].op2[0] = '\0';
                    ins[i].op[0] = '\0';
                }
            } else if (strcmp(ins[i].op, "/") == 0) {
                if (strcmp(ins[i].op2, "1") == 0) {
                    ins[i].isBinary = 0;
                    ins[i].op2[0] = '\0';
                    ins[i].op[0] = '\0';
                } else if (strcmp(ins[i].op1, "0") == 0) {
                    ins[i].isBinary = 0;
                    strcpy(ins[i].op1, "0");
                    ins[i].op2[0] = '\0';
                    ins[i].op[0] = '\0';
                }
            }
        }

        if (ins[i].isBinary && isNumber(ins[i].op1) && isNumber(ins[i].op2)) {
            int a = atoi(ins[i].op1);
            int b = atoi(ins[i].op2);
            int ans = evalExpr(a, b, ins[i].op);
            sprintf(ins[i].op1, "%d", ans);
            ins[i].isBinary = 0;
            ins[i].op2[0] = '\0';
            ins[i].op[0] = '\0';
        }

        if (!ins[i].isBinary) {
            if (isNumber(ins[i].op1)) {
                setConst(ins[i].lhs, ins[i].op1);
            } else {
                setCopy(ins[i].lhs, ins[i].op1);
            }
        } else {
            char key[3 * MAX_LEN];
            makeExprKey(key, ins[i].op1, ins[i].op, ins[i].op2);
            int pos = findExpr(key);
            if (pos != -1) {
                ins[i].isBinary = 0;
                strcpy(ins[i].op1, exprMap[pos].var);
                ins[i].op2[0] = '\0';
                ins[i].op[0] = '\0';
                setCopy(ins[i].lhs, ins[i].op1);
            } else {
                strcpy(exprMap[exprCount].expr, key);
                strcpy(exprMap[exprCount].var, ins[i].lhs);
                exprCount++;
            }
        }
    }

    for (int i = n - 1; i >= 0; i--) {
        if (ins[i].removed) continue;
        if (isTemp(ins[i].lhs) && !usedLater(i, ins[i].lhs)) {
            ins[i].removed = 1;
        }
    }
}

int main() {
    char line[200];

    while (1) {
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "END") == 0) break;
        if (strlen(line) == 0) continue;

        char lhs[MAX_LEN], eq[5], a[MAX_LEN], op[5], b[MAX_LEN];
        int cnt = sscanf(line, "%s %s %s %s %s", lhs, eq, a, op, b);

        strcpy(ins[n].lhs, lhs);
        strcpy(ins[n].op1, a);
        ins[n].removed = 0;

        if (cnt == 5) {
            strcpy(ins[n].op, op);
            strcpy(ins[n].op2, b);
            ins[n].isBinary = 1;
        } else {
            ins[n].op[0] = '\0';
            ins[n].op2[0] = '\0';
            ins[n].isBinary = 0;
        }
        n++;
    }

    optimize();

    for (int i = 0; i < n; i++) {
        if (ins[i].removed) continue;
        if (ins[i].isBinary)
            printf("%s = %s %s %s\n", ins[i].lhs, ins[i].op1, ins[i].op, ins[i].op2);
        else
            printf("%s = %s\n", ins[i].lhs, ins[i].op1);
    }

    return 0;
}