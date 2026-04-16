#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

char input[1000];
int pos = 0;
int t_count = 0;
int l_count = 0;

// Function Prototypes
void e(char *place);
void condition(char *falseLabel);
void statement();
void assignment();

// TAC Temp Variable Generator
void nextemp(char *result) {
    sprintf(result, "t%d", t_count++);
}

// TAC Label Generator
void newlabel(char *label) {
    sprintf(label, "L%d", l_count++);
}

void skip_spaces() {
    while (isspace(input[pos])) pos++;
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

// --- EXPRESSION PARSING (e, t, f) ---
void f(char *place) {
    skip_spaces();
    if (input[pos] == '(') {
        pos++;
        e(place);
        match(')');
    } else if (isalnum(input[pos])) {
        int i = 0;
        while (isalnum(input[pos])) place[i++] = input[pos++];
        place[i] = '\0';
    } else {
        printf("Syntax Error at position %d: %c\n", pos, input[pos]);
        exit(1);
    }
}

void t(char *place) {
    char f1[10], f2[10], tmp[10];
    f(f1);
    strcpy(place, f1);
    skip_spaces();
    while (input[pos] == '*' || input[pos] == '/') {
        char op = input[pos++];
        f(f2);
        nextemp(tmp);
        printf("%s = %s %c %s\n", tmp, place, op, f2);
        strcpy(place, tmp);
        skip_spaces();
    }
}

void e(char *place) {
    char t1[10], t2[10], tmp[10];
    t(t1);
    strcpy(place, t1);
    skip_spaces();
    while (input[pos] == '+' || input[pos] == '-') {
        char op = input[pos++];
        t(t2);
        nextemp(tmp);
        printf("%s = %s %c %s\n", tmp, place, op, t2);
        strcpy(place, tmp);
        skip_spaces();
    }
}

// --- CONDITION PARSING ---
void condition(char *falseLabel) {
    char op1[10], op2[10], relop[5];
    e(op1);
    skip_spaces();
    
    // Parse relational operators (e.g., <, >, <=, >=, ==, !=)
    int i = 0;
    while (input[pos] == '<' || input[pos] == '>' || input[pos] == '=' || input[pos] == '!') {
        relop[i++] = input[pos++];
    }
    relop[i] = '\0';
    
    e(op2);
    char skipLabel[10];
    newlabel(skipLabel);
    
    // If condition is true, skip the goto false statement
    printf("if %s %s %s goto %s\n", op1, relop, op2, skipLabel);
    printf("goto %s\n", falseLabel);
    printf("%s:\n", skipLabel);
}

// --- ASSIGNMENT PARSING ---
void assignment() {
    skip_spaces();
    char var[20], res[10];
    int i = 0;
    while (isalnum(input[pos])) var[i++] = input[pos++];
    var[i] = '\0';
    skip_spaces();
    if (input[pos] == '=') {
        pos++;
        e(res);
        printf("%s = %s\n", var, res);
    }
}

// --- STATEMENT & CONTROL FLOW PARSING ---
void statement() {
    skip_spaces();
    
    // Block of statements { ... }
    if (input[pos] == '{') {
        pos++;
        skip_spaces();
        while (input[pos] != '}' && input[pos] != '\0') {
            statement();
            skip_spaces();
        }
        match('}');
    } 
    // IF BLOCK
    else if (strncmp(&input[pos], "if", 2) == 0 && !isalnum(input[pos+2])) {
        pos += 2; match('(');
        char l_false[10]; newlabel(l_false);
        
        condition(l_false); 
        match(')');
        statement();
        
        printf("%s:\n", l_false);
    } 
    // WHILE BLOCK
    else if (strncmp(&input[pos], "while", 5) == 0 && !isalnum(input[pos+5])) {
        pos += 5; match('(');
        char l_start[10], l_end[10];
        newlabel(l_start); newlabel(l_end);
        
        printf("%s:\n", l_start);
        condition(l_end); 
        match(')');
        
        statement();
        
        printf("goto %s\n", l_start);
        printf("%s:\n", l_end);
    } 
    // FOR BLOCK
    else if (strncmp(&input[pos], "for", 3) == 0 && !isalnum(input[pos+3])) {
        pos += 3; match('(');
        
        // 1. Init
        assignment(); match(';');
        
        char l_start[10], l_end[10];
        newlabel(l_start); newlabel(l_end);
        
        // 2. Condition
        printf("%s:\n", l_start);
        condition(l_end); match(';');
        
        // 3. Skip over the update statement for now
        int update_pos = pos;
        int paren_count = 1;
        while (input[pos] != '\0') {
            if (input[pos] == '(') paren_count++;
            if (input[pos] == ')') {
                paren_count--;
                if (paren_count == 0) break;
            }
            pos++;
        }
        match(')');
        
        // 4. Parse the Body (condition() true branch naturally falls through here)
        statement();
        
        // 5. Go back and parse the update statement so it prints right after the body
        int body_end_pos = pos;
        pos = update_pos;
        assignment();
        
        // 6. Jump back to condition
        printf("goto %s\n", l_start);
        
        // 7. Restore position and print end label
        pos = body_end_pos; 
        printf("%s:\n", l_end);
    } 
    // SWITCH CASE BLOCK
    else if (strncmp(&input[pos], "switch", 6) == 0 && !isalnum(input[pos+6])) {
        pos += 6; match('(');
        char switch_var[10];
        e(switch_var); 
        match(')'); match('{');
        
        char l_end[10]; newlabel(l_end);
        
        while (input[pos] != '}' && input[pos] != '\0') {
            skip_spaces();
            if (strncmp(&input[pos], "case", 4) == 0) {
                pos += 4; skip_spaces();
                
                // Parse case value
                char val[20]; int i = 0;
                while (isalnum(input[pos])) val[i++] = input[pos++];
                val[i] = '\0';
                match(':');
                
                char l_next[10]; newlabel(l_next);
                printf("if %s != %s goto %s\n", switch_var, val, l_next);
                
                statement();
                
                // Handle optional break
                skip_spaces();
                if (strncmp(&input[pos], "break", 5) == 0) {
                    pos += 5; match(';');
                    printf("goto %s\n", l_end);
                }
                
                printf("%s:\n", l_next);
            } 
            else if (strncmp(&input[pos], "default", 7) == 0) {
                pos += 7; match(':');
                statement();
            } 
        }
        match('}');
        printf("%s:\n", l_end);
    } 
    // BASIC ASSIGNMENT
    else if (isalpha(input[pos])) {
        assignment();
        skip_spaces();
        if (input[pos] == ';') pos++; // Consume trailing semicolon if present
    }
}

int main() {
    printf("Enter code block:\n");
    
    // Read multiline inputs
    char line[200];
    input[0] = '\0';
    while (fgets(line, sizeof(line), stdin)) {
        if (line[0] == '\n') break; // Press Enter twice to finish input
        strcat(input, line);
    }

    printf("\n--- Generated Three Address Code (TAC) ---\n");
    statement();
    
    skip_spaces();
    if (input[pos] != '\0') {
        printf("\nWarning: Extra characters ignored at the end: %s\n", &input[pos]);
    }
    
    return 0;
}