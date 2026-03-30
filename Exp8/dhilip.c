#include<stdio.h>
#include<string.h>
#include<ctype.h>

int numbers[100];
char operators[100];

int numTop=-1;
int opTop=-1;

int calculate(int a,int b,char op)
{
    if(op=='+') return a+b;
    if(op=='*') return a*b;
    return 0;
}

int priority(char op)
{
    if(op=='*') return 2;
    if(op=='+') return 1;
    return 0;
}

void performOperation()
{
    int second=numbers[numTop--];
    int first=numbers[numTop--];
    char op=operators[opTop--];
    numbers[++numTop]=calculate(first,second,op);
}

int main()
{
    char expression[100];
    scanf("%s",expression);

    int length=strlen(expression);

    for(int i=0;i<length;i++)
    {
        char ch=expression[i];

        if(isdigit(ch))
        {
            numbers[++numTop]=ch-'0';
        }

        else if(ch=='(')
        {
            operators[++opTop]=ch;
        }

        else if(ch==')')
        {
            while(operators[opTop]!='(')
            {
                performOperation();
            }
            opTop--;
        }

        else if(ch=='+' || ch=='*')
        {
            while(opTop>=0 && priority(operators[opTop])>=priority(ch))
            {
                performOperation();
            }
            operators[++opTop]=ch;
        }
    }

    while(opTop>=0)
    {
        performOperation();
    }

    printf("%d",numbers[numTop]);

    return 0;
}