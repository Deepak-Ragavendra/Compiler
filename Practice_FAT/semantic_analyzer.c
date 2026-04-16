#include<stdio.h>
#include<string.h>

typedef struct{
    char name;
    int type;//0=int 1=float 2=char 3=double
} var;

var v[20];
int n;

int gettype(char name)
{
    for (int i = 0; i < n;i++)
    {
        if(v[i].name==name)
            return v[i].type;
    }
    return -1;
}

int validop(int op1,char op,int op2)
{
    if(op1==2||op2==2){
        printf("Char not allowed with operator %c\n", op);
        return 0;
    }
    return 1;
}

int result(int op1,int op2)
{
    if(op1==3||op2==3)
        return 3;
    if (op1==1||op2==1)
        return 1;
    return 0;
}

int canassign(int op1,int op2)
{
    if(op1==op2)
        return 1;
    if(op1==3&&(op2==1||op2==0))
        return 1;
    if(op1==1&&op2==0)
        return 1;
    return 0;
}
int main()
{
    printf("Enter no of variables: ");
    scanf("%d", &n);
    printf("0->int\n1->float\n2->char\n3->double\n");
    printf("Enter variable and its type 1 x (int x):\n");
    for (int i = 0; i < n;i++)
    {
        char name;
        int type;
        scanf(" %d %c", &type, &name);
        if(gettype(name)==-1)
        {
            v[i].name = name;
            v[i].type = type;
        }
        else{
            printf("Multiple Declaration of %c", name);
            return -1;
        }
    }
    printf("Enter the expr: ");
    char input[100];
    scanf(" %s", input);

    int tlhs = gettype(input[0]);
    if(tlhs==-1)
    {
        printf("Type Error:  Undeclared variable");
        return -1;
    }

    int tres = gettype(input[2]);
    if(tres==-1)
    {
        printf("Type Error:  Undeclared variable");
        return -1;
    }
    for (int i = 3; input[i];i+=2)
    {
        char op = input[i];
        char next = input[i + 1];
        int t2=gettype(next);
        if(t2==-1)
        {
            printf("Type Error:Undeclared variable");
            return -1;
        }
        if(!validop(tres,op,t2))
        {
            printf("Type mismatch");
            return -1;
        }

        tres = result(tres,t2);
    }
    if(!canassign(tlhs,tres))
    {
        printf("Type Error: uncompatible types");
        return -1;
    }

    printf("Type checking sucessfull");
    return 1;
}