#include <stdio.h>
#include <ctype.h>
#include <string.h>

char expr[100];
int pos = 0;
int tempCount = 1;

char code[100][100];
int lineCount = 0;

void skipSpaces() {
    while (expr[pos] == ' ' || expr[pos] == '\t' || expr[pos] == '\n')
        pos++;
}

void newTemp(char temp[]) {
    sprintf(temp, "t%d", tempCount++);
}

void addCode(const char *result, const char *arg1, char op, const char *arg2) {
    sprintf(code[lineCount++], "%s = %s %c %s", result, arg1, op, arg2);
}

void copyString(char dest[], const char src[]) {
    strcpy(dest, src);
}

void F(char place[]);
void T(char place[]);
void E(char place[]);

void F(char place[]) {
    skipSpaces();

    if (expr[pos] == '(') {
        pos++;
        E(place);
        skipSpaces();
        if (expr[pos] == ')')
            pos++;
    }
    else if (isalnum(expr[pos])) {
        int i = 0;
        while (isalnum(expr[pos])) {
            place[i++] = expr[pos++];
        }
        place[i] = '\0';
    }
}

void T(char place[]) {
    char left[20], right[20], temp[20];
    F(left);

    skipSpaces();
    while (expr[pos] == '*' || expr[pos] == '/') {
        char op = expr[pos++];
        F(right);
        newTemp(temp);
        addCode(temp, left, op, right);
        copyString(left, temp);
        skipSpaces();
    }

    copyString(place, left);
}

void E(char place[]) {
    char left[20], right[20], temp[20];
    T(left);

    skipSpaces();
    while (expr[pos] == '+' || expr[pos] == '-') {
        char op = expr[pos++];
        T(right);
        newTemp(temp);
        addCode(temp, left, op, right);
        copyString(left, temp);
        skipSpaces();
    }

    copyString(place, left);
}

int main() {
    char result[20];

    printf("For input expression:\n");
    fgets(expr, sizeof(expr), stdin);

    expr[strcspn(expr, "\n")] = '\0';

    E(result);

    printf("\nGenerated Three Address Code:\n");
    for (int i = 0; i < lineCount; i++) {
        printf("%s\n", code[i]);
    }

    return 0;
}