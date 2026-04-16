#include<stdio.h>
#include<string.h>

typedef struct {
    char arr[100];
    int top;
} stack;

void push(stack *s, char c) {
    s->arr[++s->top] = c;
}

char pop(stack *s) {
    return s->arr[s->top--];
}

char peek(stack *s) {
    return s->arr[s->top];
}

int pos(char arr[], int n, char x) {
    for(int i = 0; i < n; i++) {
        if(arr[i] == x)
            return i;
    }
    return -1;
}

void printstack(stack *s) {
    for(int i = 0; i <= s->top; i++)
        printf("%c", s->arr[i]);
}

int main() {

    char nt[] = {'E', 'X', 'T'};
    char t[]  = {'a', '+', '$'};

    int r = 3, c = 3;

    // LL(1) Table
    char table[3][3][10] = {
        // a       +       $
        {"TX",    "-",    "-"},   // E
        {"-",     "+TX",  "e"},   // X
        {"a",     "-",    "-"}    // T
    };

    char input[100];
    printf("Enter input string (end with $): ");
    scanf("%s", input);

    stack s;
    s.top = -1;

    push(&s, '$');
    push(&s, 'E'); 

    int i = 0;

    printf("\nStack\tInput\tAction\n");

    while(1) {
        char top = peek(&s);

        if(top == input[i]) {
            printstack(&s);
            printf("\t%s\tMatch %c\n", &input[i], top);
            pop(&s);
            i++;
        }
        else {
            int x = pos(nt, r, top);
            int y = pos(t, c, input[i]);

            if(x == -1 || y == -1) {
                printf("Error parsing\n");
                return 0;
            }

            char *rule = table[x][y];

            if(rule[0] == '-') {
                printf("Error parsing\n");
                return 0;
            }

            printstack(&s);
            printf("\t%s\tApply %c -> %s\n", &input[i], top, rule);

            pop(&s);

            if(rule[0] != 'e') {
                int n = strlen(rule);
                for(int j = n - 1; j >= 0; j--) {
                    push(&s, rule[j]);
                }
            }
        }

        if(peek(&s) == '$' && input[i] == '$') {
            printstack(&s);
            printf("\t%s\tMatch $\n", &input[i]);
            break;
        }
    }

    printf("\nString Accepted\n");
    return 0;
}