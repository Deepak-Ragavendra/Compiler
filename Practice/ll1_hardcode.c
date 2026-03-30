#include <stdio.h>
#include <string.h>
#include <ctype.h>

char stack[100];
int top = -1;

char input[100];
int ip = 0;

/* Push */
void push(char c) {
    stack[++top] = c;
}

/* Pop */
char pop() {
    return stack[top--];
}

/* Check terminal */
int isTerminal(char c) {
    return (c == 'i' || c == '+' || c == '*' || c == '(' || c == ')' || c == '$');
}

/* LL(1) Parsing Table (hard coded) */
char* table(char nonTerm, char term) {
    if (nonTerm == 'E' && term == 'i') return "TX";
    if (nonTerm == 'E' && term == '(') return "TX";

    if (nonTerm == 'X' && term == '+') return "+TX";
    if (nonTerm == 'X' && (term == ')' || term == '$')) return "#";

    if (nonTerm == 'T' && term == 'i') return "FY";
    if (nonTerm == 'T' && term == '(') return "FY";

    if (nonTerm == 'Y' && term == '*') return "*FY";
    if (nonTerm == 'Y' && (term == '+' || term == ')' || term == '$')) return "#";

    if (nonTerm == 'F' && term == 'i') return "i";
    if (nonTerm == 'F' && term == '(') return "(E)";

    return NULL;
}

int main() {
    printf("Enter input string (end with $): ");
    scanf("%s", input);

    push('$');
    push('E');

    while (top >= 0) {
        char X = stack[top];
        char a = input[ip];

        if (X == a) {
            pop();
            ip++;
        }
        else if (isTerminal(X)) {
            printf("Rejected\n");
            return 0;
        }
        else {
            char* prod = table(X, a);
            if (prod == NULL) {
                printf("Rejected\n");
                return 0;
            }

            pop();
            if (prod[0] != '#') {
                for (int i = strlen(prod) - 1; i >= 0; i--)
                    push(prod[i]);
            }
        }
    }

    if (input[ip] == '\0')
        printf("Accepted\n");
    else
        printf("Rejected\n");

    return 0;
}