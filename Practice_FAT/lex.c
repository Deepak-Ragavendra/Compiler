#include <stdio.h>
#include <string.h>
#include <ctype.h>

int keyword(char *s) {
    char *kw[] = {"int", "float", "char", "if", "else", "while", "return", "void", "main", "include", "printf"};
    int n = sizeof(kw) / sizeof(kw[0]);
    for (int i = 0; i < n; i++)
        if (strcmp(s, kw[i]) == 0) return 1;
    return 0;
}

int isoperator(char c) {
    char op[] = "+-*/=<>!";
    for (int i = 0; i < strlen(op); i++)
        if (op[i] == c) return 1;
    return 0;
}

int validIdentifier(char *s) {
    if (!( isalpha(s[0]) || s[0] == '_')) return 0;
    for (int i = 1; s[i]; i++)
        if (!(isalnum(s[i]) || s[i] == '_')) return 0;
    return !keyword(s);
}

int main() {
    FILE *fp = fopen("code.txt", "r");
    FILE *clean = fopen("clean.txt", "w");

    if (fp == NULL || clean == NULL) {
        printf("File error\n");
        return 1;
    }

    int ch;
    char buff[100];
    int i;
    int k_cnt = 0, id_cnt = 0, num_cnt = 0, op_cnt = 0, sym_cnt = 0, inv_id_cnt = 0;

    while ((ch = fgetc(fp)) != EOF) {

        // --- COMMENT HANDLING & REMOVAL ---
        if (ch == '/') {
            int next = fgetc(fp);
            if (next == '/') {
                // Single-line comment
                while ((ch = fgetc(fp)) != '\n' && ch != EOF);
                fputc('\n', clean);
                continue;
            } else if (next == '*') {
                // Multi-line comment
                int closed = 0;
                while ((ch = fgetc(fp)) != EOF) {
                    if (ch == '*') {
                        int peek = fgetc(fp);
                        if (peek == '/') { closed = 1; break; }
                        else ungetc(peek, fp);
                    }
                }
                if (!closed) {
                    printf("LEXICAL ERROR: Unclosed multi-line comment\n");
                    fclose(fp); fclose(clean);
                    return 1;
                }
                continue;
            } else {
                // Division operator
                ungetc(next, fp);
                printf("<Operator, />\n");
                fputc('/', clean);
                op_cnt++;
                continue;
            }
        }

        if (isspace(ch)) { fputc(ch, clean); continue; }

        // --- STRING LITERAL HANDLING ("...") ---
        if (ch == '"') {
            i = 0; buff[i++] = ch;
            while ((ch = fgetc(fp)) != EOF && ch != '"') {
                if (i < 99) buff[i++] = ch;
            }
            if (ch == '"') buff[i++] = ch;
            buff[i] = '\0';
            printf("<String, %s>\n", buff);
            fprintf(clean, "%s", buff);
            continue;
        }

        // --- CHAR LITERAL HANDLING ('x') ---
        if (ch == '\'') {
            i = 0; buff[i++] = ch;
            while ((ch = fgetc(fp)) != EOF && ch != '\'') {
                if (i < 99) buff[i++] = ch;
            }
            if (ch == '\'') buff[i++] = ch;
            buff[i] = '\0';
            printf("<Char Literal, %s>\n", buff);
            fprintf(clean, "%s", buff);
            continue;
        }

        // --- IDENTIFIER / KEYWORD ---
        if (isalpha(ch) || ch == '_') {
            i = 0; buff[i++] = ch;
            while ((ch = fgetc(fp)) != EOF && (isalnum(ch) || ch == '_')) {
                if (i < 99) buff[i++] = ch;
            }
            buff[i] = '\0';
            ungetc(ch, fp);
            fprintf(clean, "%s", buff);

            if (keyword(buff)) {
                printf("<Keyword, %s>\n", buff);
                k_cnt++;
            } else if (validIdentifier(buff)) {
                printf("<Identifier, %s>\n", buff);
                id_cnt++;
            } else {
                printf("<Invalid Identifier, %s>\n", buff);
                inv_id_cnt++;
            }
            continue;
        }

        // --- NUMBER HANDLING + INVALID IDENTIFIER (e.g. 2wrong) ---
        if (isdigit(ch)) {
            i = 0; buff[i++] = ch;
            int isInvalid = 0, dot = 0;
            while ((ch = fgetc(fp)) != EOF && (isalnum(ch) || ch == '.' || ch == '_')) {
                if (ch == '.') dot++;
                if (isalpha(ch) || ch == '_') isInvalid = 1;
                if (i < 99) buff[i++] = ch;
            }
            buff[i] = '\0';
            ungetc(ch, fp);
            fprintf(clean, "%s", buff);

            if (isInvalid || dot > 1) {
                printf("<Invalid Identifier, %s>\n", buff);
                inv_id_cnt++;
            } else {
                printf("<Number, %s>\n", buff);
                num_cnt++;
            }
            continue;
        }

        // --- OPERATORS ---
        if (isoperator(ch)) {
            int next = fgetc(fp);
            if ((ch == '=' || ch == '<' || ch == '>' || ch == '!') && next == '=') {
                printf("<Operator, %c%c>\n", ch, next);
                fprintf(clean, "%c%c", ch, next);
            } else {
                ungetc(next, fp);
                printf("<Operator, %c>\n", ch);
                fputc(ch, clean);
            }
            op_cnt++;
            continue;
        }

        // --- SYMBOLS ---
        printf("<Symbol, %c>\n", ch);
        sym_cnt++;
        fputc(ch, clean);
    }

    fclose(fp);
    fclose(clean);

    // Token Count Summary
    printf("\n--- TOKEN COUNT SUMMARY ---\n");
    printf("Keywords:    %d\n", k_cnt);
    printf("Identifiers: %d\n", id_cnt);
    printf("Invalid IDs: %d\n", inv_id_cnt);
    printf("Numbers:     %d\n", num_cnt);
    printf("Operators:   %d\n", op_cnt);
    printf("Symbols:     %d\n", sym_cnt);

    return 0;
}