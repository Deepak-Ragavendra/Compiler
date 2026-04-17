#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

char input[2000];
int pos = 0;
int t_count = 0;
int l_count = 0;

void statement();
void expression(char *place);
void condition(char *falseLabel);

void nextemp(char *result) {
    sprintf(result, "t%d", t_count++);
}

void newlabel(char *label) {
    sprintf(label, "L%d", l_count++);
}

void skip_spaces() {
    while (isspace(input[pos])) pos++;
}

int startswith(const char *kw) {
    skip_spaces();
    int len = strlen(kw);
    return strncmp(&input[pos], kw, len) == 0 && !isalnum(input[pos + len]);
}

void match(char expected) {
    skip_spaces();
    if (input[pos] == expected) {
        pos++;
    } else {
        printf("Syntax Error: Expected '%c' but found '%c' at position %d\n", expected, input[pos], pos);
        exit(1);
    }
}

void factor(char *place) {
    skip_spaces();

    if (input[pos] == '(') {
        pos++;
        expression(place);
        match(')');
    }
    else if (isalnum(input[pos])) {
        int i = 0;
        while (isalnum(input[pos])) {
            place[i++] = input[pos++];
        }
        place[i] = '\0';
    }
    else {
        printf("Syntax Error at position %d near '%c'\n", pos, input[pos]);
        exit(1);
    }
}

void term(char *place) {
    char f1[50], f2[50], tmp[50];
    factor(f1);
    strcpy(place, f1);

    skip_spaces();
    while (input[pos] == '*' || input[pos] == '/') {
        char op = input[pos++];
        factor(f2);
        nextemp(tmp);
        printf("%s = %s %c %s\n", tmp, place, op, f2);
        strcpy(place, tmp);
        skip_spaces();
    }
}

void expression(char *place) {
    char t1[50], t2[50], tmp[50];
    term(t1);
    strcpy(place, t1);

    skip_spaces();
    while (input[pos] == '+' || input[pos] == '-') {
        char op = input[pos++];
        term(t2);
        nextemp(tmp);
        printf("%s = %s %c %s\n", tmp, place, op, t2);
        strcpy(place, tmp);
        skip_spaces();
    }
}

void parse_identifier(char *var) {
    skip_spaces();
    int i = 0;
    if (!isalpha(input[pos])) {
        printf("Syntax Error: Identifier expected at position %d\n", pos);
        exit(1);
    }
    while (isalnum(input[pos])) {
        var[i++] = input[pos++];
    }
    var[i] = '\0';
}

void assignment_or_increment() {
    char var[50], res[50], tmp[50];
    parse_identifier(var);
    skip_spaces();

    if (input[pos] == '=' ) {
        pos++;
        expression(res);
        printf("%s = %s\n", var, res);
    }
    else if (input[pos] == '+' && input[pos + 1] == '+') {
        pos += 2;
        nextemp(tmp);
        printf("%s = %s + 1\n", tmp, var);
        printf("%s = %s\n", var, tmp);
    }
    else if (input[pos] == '-' && input[pos + 1] == '-') {
        pos += 2;
        nextemp(tmp);
        printf("%s = %s - 1\n", tmp, var);
        printf("%s = %s\n", var, tmp);
    }
    else {
        printf("Syntax Error: Invalid assignment/update at position %d\n", pos);
        exit(1);
    }
}

void condition(char *falseLabel) {
    char op1[50], op2[50], relop[10], trueLabel[20];
    int i = 0;

    expression(op1);
    skip_spaces();

    if (input[pos] == '<' || input[pos] == '>' || input[pos] == '=' || input[pos] == '!') {
        while (input[pos] == '<' || input[pos] == '>' || input[pos] == '=' || input[pos] == '!') {
            relop[i++] = input[pos++];
        }
        relop[i] = '\0';
    } else {
        printf("Syntax Error: Relational operator expected at position %d\n", pos);
        exit(1);
    }

    expression(op2);
    newlabel(trueLabel);

    printf("if %s %s %s goto %s\n", op1, relop, op2, trueLabel);
    printf("goto %s\n", falseLabel);
    printf("%s:\n", trueLabel);
}

void block() {
    match('{');
    skip_spaces();
    while (input[pos] != '}' && input[pos] != '\0') {
        statement();
        skip_spaces();
    }
    match('}');
}

void parse_if() {
    pos += 2;
    match('(');

    char l_false[20], l_end[20];
    newlabel(l_false);
    newlabel(l_end);

    condition(l_false);
    match(')');

    statement();

    skip_spaces();
    if (startswith("else")) {
        printf("goto %s\n", l_end);
        printf("%s:\n", l_false);
        pos += 4;
        statement();
        printf("%s:\n", l_end);
    } else {
        printf("%s:\n", l_false);
    }
}

void parse_while() {
    char l_start[20], l_end[20];
    newlabel(l_start);
    newlabel(l_end);

    pos += 5;
    printf("%s:\n", l_start);
    match('(');
    condition(l_end);
    match(')');

    statement();
    printf("goto %s\n", l_start);
    printf("%s:\n", l_end);
}

void parse_for() {
    char l_start[20], l_end[20];
    int cond_start, update_start, update_end, body_end;

    pos += 3;
    match('(');

    assignment_or_increment();
    match(';');

    newlabel(l_start);
    newlabel(l_end);

    printf("%s:\n", l_start);

    cond_start = pos;
    condition(l_end);
    match(';');

    update_start = pos;
    while (input[pos] != ')' && input[pos] != '\0') pos++;
    update_end = pos;

    match(')');

    statement();
    body_end = pos;

    pos = update_start;
    assignment_or_increment();

    pos = body_end;
    printf("goto %s\n", l_start);
    printf("%s:\n", l_end);
}

void parse_switch() {
    char switch_var[50], l_end[20], l_next[20], val[50];
    pos += 6;
    match('(');
    expression(switch_var);
    match(')');
    match('{');

    newlabel(l_end);

    while (1) {
        skip_spaces();
        if (input[pos] == '}') break;

        if (startswith("case")) {
            pos += 4;
            skip_spaces();

            int i = 0;
            while (isalnum(input[pos])) {
                val[i++] = input[pos++];
            }
            val[i] = '\0';

            match(':');
            newlabel(l_next);

            printf("if %s != %s goto %s\n", switch_var, val, l_next);

            while (1) {
                skip_spaces();
                if (startswith("break")) {
                    pos += 5;
                    match(';');
                    printf("goto %s\n", l_end);
                    break;
                }
                if (startswith("case") || startswith("default") || input[pos] == '}') {
                    break;
                }
                statement();
            }

            printf("%s:\n", l_next);
        }
        else if (startswith("default")) {
            pos += 7;
            match(':');
            while (1) {
                skip_spaces();
                if (input[pos] == '}') break;
                statement();
            }
        }
        else {
            printf("Syntax Error in switch at position %d\n", pos);
            exit(1);
        }
    }

    match('}');
    printf("%s:\n", l_end);
}

void statement() {
    skip_spaces();

    if (input[pos] == '{') {
        block();
    }
    else if (startswith("if")) {
        parse_if();
    }
    else if (startswith("while")) {
        parse_while();
    }
    else if (startswith("for")) {
        parse_for();
    }
    else if (startswith("switch")) {
        parse_switch();
    }
    else if (isalpha(input[pos])) {
        assignment_or_increment();
        skip_spaces();
        if (input[pos] == ';') pos++;
    }
    else {
        printf("Syntax Error: Invalid statement at position %d near '%c'\n", pos, input[pos]);
        exit(1);
    }
}

int main() {
    char line[200];

    printf("Enter code block:\n");
    input[0] = '\0';

    while (fgets(line, sizeof(line), stdin)) {
        if (line[0] == '\n') break;
        strcat(input, line);
    }

    printf("\n--- Generated Three Address Code (TAC) ---\n");
    while (1) {
        skip_spaces();
        if (input[pos] == '\0') break;
        statement();
    }

    return 0;
}