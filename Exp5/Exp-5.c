#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX 5000
char str[MAX];
const char *cursor;

// ---------- Utility ----------
void skipSpaces() {
    while(*cursor == ' ' || *cursor == '\n' || *cursor == '\t')
        cursor++;
}

int isIdentifierChar(char c) {
    return isalnum(c) || c == '_';
}

// ---------- Expression Parsing ----------
int FACTOR() {
    skipSpaces();
    // Parse identifiers or numbers (multi-char)
    if(isalpha(*cursor) || *cursor == '_') {
        cursor++;
        while(isIdentifierChar(*cursor)) cursor++;
        return 1;
    } else if(isdigit(*cursor)) {
        cursor++;
        while(isdigit(*cursor)) cursor++;
        return 1;
    } else if(*cursor=='(') {
        cursor++;
        if(EXPR()) {
            skipSpaces();
            if(*cursor==')') {
                cursor++;
                return 1;
            }
        }
        return 0;
    }
    return 0;
}

int TERM_() {
    skipSpaces();
    if(*cursor=='*' || *cursor=='/') {
        cursor++;
        if(!FACTOR()) return 0;
        return TERM_();
    }
    return 1;
}

int TERM() {
    if(!FACTOR()) return 0;
    return TERM_();
}

int EXPR_() {
    skipSpaces();
    if(*cursor=='+' || *cursor=='-') {
        cursor++;
        if(!TERM()) return 0;
        return EXPR_();
    }
    return 1;
}

int EXPR() {
    if(!TERM()) return 0;
    return EXPR_();
}

// ---------- Statement Parsing ----------
int BLOCK(); // forward declaration

int FOR() {
    skipSpaces();
    if(strncmp(cursor,"for",3)==0 && !isIdentifierChar(cursor[3])) {
        cursor += 3;
        skipSpaces();
        if(*cursor=='(') {
            cursor++;
            if(EXPR()) {
                skipSpaces();
                if(*cursor==';') {
                    cursor++;
                    if(EXPR()) {
                        skipSpaces();
                        if(*cursor==';') {
                            cursor++;
                            if(EXPR()) {
                                skipSpaces();
                                if(*cursor==')') {
                                    cursor++;
                                    skipSpaces();
                                    if(BLOCK()) return 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int IF() {
    skipSpaces();
    if(strncmp(cursor,"if",2)==0 && !isIdentifierChar(cursor[2])) {
        cursor += 2;
        skipSpaces();
        if(*cursor=='(') {
            cursor++;
            if(EXPR()) {
                skipSpaces();
                if(*cursor==')') {
                    cursor++;
                    skipSpaces();
                    if(BLOCK()) return 1;
                }
            }
        }
    }
    return 0;
}

int STMT(); // forward declaration

int STMT_LIST() {
    skipSpaces();
    while(STMT()) {
        skipSpaces();
    }
    return 1; // empty block allowed
}

int BLOCK() {
    skipSpaces();
    if(*cursor=='{') {
        cursor++;
        STMT_LIST();
        skipSpaces();
        if(*cursor=='}') {
            cursor++;
            return 1;
        }
        return 0; // unmatched }
    }
    return 0;
}

int STMT() {
    skipSpaces();
    if(FOR()) return 1;
    if(IF()) return 1;
    if(BLOCK()) return 1;
    if(EXPR()) {
        skipSpaces();
        if(*cursor==';') {
            cursor++;
            return 1;
        }
        return 0;
    }
    return 0;
}

// ---------- Main ----------
int main() {
    printf("Enter your code (nested for/if allowed, multiline):\n");

    int len = 0;
    int openBraces = 0, closeBraces = 0;
    char line[500];

    while(1) {
        if(!fgets(line,sizeof(line),stdin)) break; // EOF
        int lineLen = strlen(line);
        if(len + lineLen >= MAX-1) break;
        strcpy(str+len,line);
        len += lineLen;

        // count braces
        for(int i=0;i<lineLen;i++){
            if(line[i]=='{') openBraces++;
            else if(line[i]=='}') closeBraces++;
        }

        // stop when braces are balanced and we have at least one statement
        if(openBraces>0 && openBraces==closeBraces) break;
    }

    str[len]='\0';
    cursor = str;

    if(STMT()) {
        skipSpaces();
        if(*cursor=='\0') {
            printf("Valid syntax\n");
        } else {
            printf("Invalid syntax (extra characters)\n");
        }
    } else {
        printf("Invalid syntax\n");
    }

    return 0;
}