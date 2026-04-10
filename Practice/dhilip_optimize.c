#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

typedef struct{
    char res[10],op1[10],op2[10];
    char op;
    int used;
} tac;

tac t[100];
int n = 0;
int isnumber(char*s)
{
    for (int i = 0; s[i];i++)
    {
        if(!isdigit(s[i]))
            return 0;
    }
    return 1;
}
int compute(int a, int b, char op) {
    if (op == '+') return a + b;
    if (op == '-') return a - b;
    if (op == '*') return a * b;
    if (op == '/' && b != 0) return a / b;
    return 0;
}
void parse(char*line)
{
    int count = sscanf(line, "%s = %s %c %s", t[n].res, t[n].op1, &t[n].op, t[n].op2);
    if(count==2)
    {
        count = sscanf(line, "%s = %s", t[n].res, t[n].op1);
        t[n].op =' ';
        strcpy(t[n].op2, "");
    }
    t[n].used = 1;
    n++;
}
void print_tac()
{
    for (int i = 0; i < n;i++)
    {
        if(t[i].used)
        {
            if(t[i].op==' ')
                printf("%s = %s\n", t[i].res, t[i].op1);
            else
            printf("%s = %s %c %s\n", t[i].res, t[i].op1, t[i].op, t[i].op2);
        }
        // else{
        //     printf("%s eliminated\n", t[i].res);
        // }
    }
}
int constant_folding()
{
    printf("Constant folding\n");
    for (int i = 0; i < n;i++)
    {
        if(t[i].op!=' '&&isnumber(t[i].op1)&&isnumber(t[i].op2))
        {
            sprintf(t[i].op1, "%d", compute(atoi(t[i].op1), atoi(t[i].op2), t[i].op));
            t[i].op = ' ';
            strcpy(t[i].op2, "");
        }
    }
    print_tac();
}

int constant_propagation()
{
    printf("Constant propogation\n");
    for (int i = 0; i < n;i++)
    {
        if(t[i].op==' '&&isnumber(t[i].op1))
        {    
            for (int j = i + 1; j < n;j++)
            {
                if (strcmp(t[i].res, t[j].res) == 0) break;
                if(strcmp(t[i].res,t[j].op1)==0){
                    strcpy(t[j].op1,t[i].op1);
                }
                else if(strcmp(t[i].res,t[j].op2)==0){
                    strcpy(t[j].op2,t[i].op1);
                }
            }
        }
    }
    print_tac();
}
int copy_propagation()
{
    printf("Copy propogation\n");
    for (int i = 0; i < n;i++)
    {
        if(t[i].op==' '&&!isnumber(t[i].op1))
        {
            for (int j = i + 1; j < n;j++)
            {
                if (strcmp(t[i].res, t[j].res) == 0) break; // ADD THIS to copy_propagation
                if(strcmp(t[i].res,t[j].op1)==0){
                    strcpy(t[j].op1,t[i].op1);
                }
                else if(strcmp(t[i].res,t[j].op2)==0){
                    strcpy(t[j].op2,t[i].op1);
                }
            }
        }
    }
    print_tac();
}
int cse()
{
    printf("Cse\n");
    for (int i = 0; i < n;i++)
    {
        for (int j = i + 1; j < n;j++)
        {
            if(strcmp(t[i].res,t[j].op1)==0||strcmp(t[i].res,t[j].op2)==0)
                break;
            if(t[i].op==t[j].op&&((strcmp(t[i].op1,t[j].op1)==0&&strcmp(t[i].op2,t[j].op2)==0)||(strcmp(t[i].op1,t[j].op2)==0&&strcmp(t[i].op2,t[j].op1)==0)))
            {
                strcpy(t[j].op1, t[i].res);
                t[j].op = ' ';
                strcpy(t[j].op2,"");
            }
        }
    }
    print_tac();
}

int algebraic_simplification()
{
    printf("Algebraic Simulation\n");

    for (int i = 0; i < n;i++)
    {
        if(t[i].op=='+')
        {
            if(strcmp(t[i].op1,"0")==0)
            {
                strcpy(t[i].op1, t[i].op2);
                t[i].op = ' ';
                strcpy(t[i].op2, "");
            }
            else if (strcmp(t[i].op2,"0")==0)
            {
                t[i].op = ' ';
                strcpy(t[i].op2, "");
            }

        }
        else if(t[i].op=='*')
        {
            if(strcmp(t[i].op1,"1")==0)
            {
                strcpy(t[i].op1, t[i].op2);
                t[i].op = ' ';
                strcpy(t[i].op2, "");
            }
            else if (strcmp(t[i].op2,"1")==0)
            {
                t[i].op = ' ';
                strcpy(t[i].op2, "");
            }
            else if(strcmp(t[i].op1,"0")==0||strcmp(t[i].op2,"0")==0)
            {
                strcpy(t[i].op1,"0");
                t[i].op = ' ';
                strcpy(t[i].op2, "");
            }
        }
    }
    print_tac();
}

void dead_code_elimination() {
    printf("Dead code elimination:\n");
    for (int i = 0; i < n;i++)
    {
        t[i].used = 0;
        if(i==n-1)
        {
            t[i].used = 1;
        }
        for (int j = i + 1; j < n;j++)
        {
            if(strcmp(t[i].res,t[j].op1)==0||strcmp(t[i].res,t[j].op2)==0)
            {
                t[i].used = 1;
                break;
            }
        }
    }
    print_tac();
}
int main()
{
    char line[100];
    printf("Enter tac and type end to stop:\n");
    while(1)
    {
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = 0;
        if(strcmp(line,"END")==0)
            break;
        parse(line);
    }
    constant_folding();
    constant_propagation();
    cse();
    copy_propagation();
    algebraic_simplification();
    dead_code_elimination();
}