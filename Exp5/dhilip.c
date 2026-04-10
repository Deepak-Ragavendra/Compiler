#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int e(), e1(), t(), t1(), f();
char *p;
int main()
{
    FILE *fp = fopen("input.txt", "r");
    if(fp==NULL)
    {
        printf("File not found");
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    char *s = malloc(size + 1);
    rewind(fp);
    fread(s, 1, size, fp);
    s[size] = '\0';
    p = s;
    if(e()&&*p=='\0')        printf("The string %s is syntactically correct", s);
    else    printf("The string %s is syntactically wrong", s);
    fclose(fp);
    free(s);
    return 0;
}
int e()
{
    if(!t())
        return 0;
    if(!e1())
        return 0;
    return 1;
}
int e1()
{
    if(*p=='+'||*p=='-')
    {
        p++;
        if(!t())
            return 0;
        return e1();
    }
    return 1;
}
int t()
{
    if(!f())
        return 0;
    if(!t1())
        return 0;
    return 1;
}
int t1()
{
    if(*p=='*'||*p=='/')
    {
        p++;
        if(!f())
            return 0;
        return t1();
    }
    return 1;
}
int f()
{
    if(*p=='(')
    {
        p++;
        if(!e())
            return 0;
        if(*p==')'){
            p++;
            return 1;
        }
        return 0;
    }
    else if((*p>='a'&&*p<='z')||(*p>='A'&&*p<='Z'))
    {
        p++;
        return 1;
    }
    else if(*p>='0'&&*p<='9')
    {
        p++;
        while(*p>='0'&&*p<='9')
            p++;
        return 1;
    }
    else
        return 0;
}