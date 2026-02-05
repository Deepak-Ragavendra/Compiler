#include <stdio.h>
#include <string.h>
#include <ctype.h>

int Constant(char *token) {
    char *c[] = {"10", "20"};
    for (int i = 0; i < 2; i++) {
        if (strcmp(token, c[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int Special(char a) {
    char sp[] = {'{', '}', '[', ']', ';', ',', '(', ')'};
    for (int i = 0; i < 8; i++) {
        if (a == sp[i]) {
            return 1;
        }
    }
    return 0;
}

int Identifier(char *token) {
    char *ident[] = {"num1", "num2", "num3"};
    for (int i = 0; i < 3; i++) {
        if (strcmp(token, ident[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int Keyword(char *token) {
    char *key[] = {"if", "main", "else", "void", "int"};
    for (int i = 0; i < 5; i++) {
        if (strcmp(token, key[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int Operator(char o) {
    char oper[] = {'+', '-', '=', '>'};
    for (int i = 0; i < 4; i++) {
        if (o == oper[i]) {
            return 1;
        }
    }
    return 0;
}

int main() {
    char text[] =
        "void main ()\n"
        "{\n"
        "int num1 , num2 , num3 ;\n"
        "num1 = 10 ;\n"
        "num2 = 20 ;\n"
        "if ( num1 > 10 ) num3 = num1 ;\n"
        "else num3 = num2 ;\n"
        "}";

    char token[100];
    int j = 0;
    int line = 1;

    printf("Line-%d\n", line);

    for (int i = 0; text[i] != '\0'; i++) {
        if (isspace(text[i])) {
            if (text[i] == '\n') {
                line++;
                printf("\nLine-%d\n", line);
            }
            if (j != 0) {
                token[j] = '\0';
                if (Constant(token))
                    printf("Constant-%s\n", token);
                else if (Keyword(token))
                    printf("Keyword-%s\n", token);
                else if (Identifier(token))
                    printf("Identifier-%s\n", token);
                else
                    printf("Unknown-%s\n", token);
                j = 0;
            }
            continue;
        }
        if (Operator(text[i])) {
            if (j != 0) {
                token[j] = '\0';
                if (Constant(token))
                    printf("Constant-%s\n", token);
                else if (Keyword(token))
                    printf("Keyword-%s\n", token);
                else if (Identifier(token))
                    printf("Identifier-%s\n", token);
                else
                    printf("Unknown-%s\n", token);

                j = 0;
            }
            printf("Operator-%c\n", text[i]);
            continue;
        }
        if (Special(text[i])) {
            if (j != 0) {
                token[j] = '\0';
                if (Keyword(token))
                    printf("Keyword-%s\n", token);
                else if (Identifier(token))
                    printf("Identifier-%s\n", token);
                else if (Constant(token))
                    printf("Constant-%s\n", token);
                else
                    printf("Unknown-%s\n", token);

                j = 0;
            }
            printf("Special Character-%c\n", text[i]);
            continue;
        }

        token[j++] = text[i];
    }

    return 0;
}
