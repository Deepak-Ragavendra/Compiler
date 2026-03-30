#include <stdio.h>
#include <string.h>

#define MAX 20

/* Stack structure */
typedef struct {
    char arr[100];
    int top;
} stack;

/* Stack operations */
void push(stack *s, char c) {
    s->arr[++s->top] = c;
}

char pop(stack *s) {
    if (s->top == -1) return '\0';
    return s->arr[s->top--];
}

char peek(stack *s) {
    if (s->top == -1) return '\0';
    return s->arr[s->top];
}

void printStack(stack *s) {
    for (int i = 0; i <= s->top; i++)
        printf("%c", s->arr[i]);
}

/* Utility functions */
int pos(char arr[], int n, char c) {
    for (int i = 0; i < n; i++)
        if (arr[i] == c) return i;
    return -1;
}

int isTerminal(char term[], int tCount, char c) {
    return pos(term, tCount, c) != -1;
}

int main() {
    int r, c;

    char nt[MAX];              // non-terminals
    char t[MAX];               // terminals
    char table[MAX][MAX][MAX]; // LL(1) table

    char input[100];
    stack s;
    s.top = -1;

    /* Input non-terminals */
    printf("Enter number of non-terminals: ");
    scanf("%d", &r);

    printf("Enter non-terminals: ");
    for (int i = 0; i < r; i++)
        scanf(" %c", &nt[i]);

    /* Input terminals */
    printf("Enter number of terminals: ");
    scanf("%d", &c);

    printf("Enter terminals: ");
    for (int i = 0; i < c; i++)
        scanf(" %c", &t[i]);

    /* Input parsing table */
    printf("\nEnter LL(1) Parsing Table (- for empty, e for epsilon):\n");
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            printf("M[%c,%c] = ", nt[i], t[j]);
            scanf("%s", table[i][j]);
        }
    }

    /* Input string */
    printf("\nEnter input string: ");
    scanf("%s", input);
    strcat(input, "$");

    /* Initialize stack */
    push(&s, '$');
    push(&s, nt[0]);  // start symbol

    int ip = 0;

    printf("\nStack\tInput\tAction\n");
    printf("-----------------------------------\n");

    while (1) {
        char X = peek(&s);
        char a = input[ip];

        /* Accept condition */
        if (X == '$' && a == '$') {
            printStack(&s);
            printf("\t%s\tAccept\n", &input[ip]);
            printf("\nString Accepted\n");
            return 0;
        }

        /* If terminal */
        if (isTerminal(t, c, X)) {
            if (X == a) {
                printStack(&s);
                printf("\t%s\tMatch %c\n", &input[ip], X);
                pop(&s);
                ip++;
            } else {
                printf("Error: terminal mismatch\n");
                return 1;
            }
        }
        /* If non-terminal */
        else {
            int row = pos(nt, r, X);
            int col = pos(t, c, a);

            if (row == -1 || col == -1 || table[row][col][0] == '-') {
                printf("Error parsing\n");
                return 1;
            }

            printStack(&s);
            printf("\t%s\tApply %c -> %s\n",
                   &input[ip], X, table[row][col]);

            pop(&s);

            if (table[row][col][0] != 'e') {
                for (int k = strlen(table[row][col]) - 1; k >= 0; k--)
                    push(&s, table[row][col][k]);
            }
        }
    }
}






#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct
{
    char arr[100];
    int top;
}stack;
void push(stack*x,char y)
{
    x->arr[++x->top]=y;
}
char pop(stack*x)
{
    if(x->top==-1){
    printf("Stack is empty\n");
    return '\0';
    }
    return x->arr[x->top--];
}
char peek(stack*x){
return x->arr[x->top];
}
void printStack(stack*s)
{
    for(int i=0;i<=s->top;i++)
    printf("%c",s->arr[i]);
}
int pos(char*x,char y)
{
    for(int i=0;x[i]!='\0';i++)
    {
        if(x[i]==y)
        return i;
    }
    return -1;
}
int main()
{
    int r,c;
    printf("Enter number of non-terminals: ");
    scanf("%d",&r);
    printf("Enter non-terminals: ");
    char *nt=malloc(sizeof(char)*r);
    for(int i=0;i<r;i++)
        scanf(" %c",&nt[i]);
    printf("\nEnter number of non-terminals: ");
    scanf("%d",&c);
    char *t=malloc(sizeof(char)*c);
    printf("Enter non-terminals: ");
    for(int i=0;i<c;i++)
        scanf(" %c",&t[i]);
    char ***m=malloc(r*sizeof(char **));
    for (int i=0;i<r;i++)
    {
        m[i]=malloc(c*sizeof(char *));
        for(int j=0;j<c;j++)
            m[i][j]=malloc(20*sizeof(char));
    }
    printf("\nEnter LL(1) Parsing Table:\n");
    for(int i=0;i<r;i++)
    {
        for(int j=0;j<c;j++)
        {
            printf("M[%c,%c] = ",nt[i],t[j]);
            scanf("%s",m[i][j]);
        }
        printf("\n");
    }
    char input[100];
    printf("\nEnter input string: ");
    scanf("%s",input);
    strcat(input,"$");
    stack *s=(stack*)malloc(sizeof(stack));
    s->top=-1;
    push(s,'$');
    push(s,nt[0]);
    int i=0;
    printf("\nStack\tInput\tAction\n");
    printf("-----------------------------------\n");
    while(1)
    {
        char top=peek(s);
        if(top==input[i])
        {
            printStack(s);
            printf("\t%s\tMatch %c\n",&input[i],top);
            pop(s);
            i++;
        }
        else
        {
            int x=pos(nt,top);
            int y=pos(t,input[i]);
            if(x==-1||y==-1)
            {
                printf("Error parsing\n");
                return 1;
            }
            char *rule=m[x][y];
            if(rule[0]=='-')
            {
                printf("Error parsing\n");
                return 1;
            }
            printStack(s);
            printf("\t%s\tApply %c -> %s\n",&input[i],top,rule);
            pop(s);
            if(rule[0]!='e')
            {
                int n=strlen(rule);
                for(int j=n-1;j>=0;j--)
                push(s,rule[j]);
            }
        }
    if(peek(s)=='$'&&input[i]=='$')
    {
        printStack(s);
        printf("\t%s\tMatch $\n",&input[i]);
        break;
    }
}
printf("\nString Accepted\n");
return 0;
}