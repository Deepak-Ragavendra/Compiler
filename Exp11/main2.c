#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define M 100

char l[M][10],a[M][10],b[M][10],o[M][3];
int n;
int num(char*s){for(int i=0;s[i];i++)if(!isdigit(s[i]))return 0;return 1;}
void fold()
{
    for(int i=0;i<n;i++)
    {
        if(num(a[i])&&num(b[i]))
        {
            int x=atoi(a[i]),y=atoi(b[i]),r;
            if(o[i][0]=='+') r=x+y;
            else if(o[i][0]=='-') r=x-y;
            else if (o[i][0]=='*') r=x*y;
            else if(o[i][0]=='/')r=x/y;
            else continue;
            sprintf(a[i],"%d",r);strcpy(o[i],"=");b[i][0]=0;
        }
    }
}

void prop()
{
    for(int i=0;i<n;i++)
    {
        if(!strcmp(o[i],"="))
        {
            for(int j=i+1;j<n;j++)
            {
                if(!strcmp(a[j],l[i])) strcpy(a[j],a[i]);
                if(!strcmp(b[j],l[i])) strcpy(b[j],a[i]);
            }
        }
    }
}

void alg()
{
    for(int i=0;i<n;i++)
    {
        if(!strcmp(o[i],"+")&&!strcmp(b[i],"0")){strcmp(o[i],"=");b[i][0]=0;}
        if(!strcmp(o[i],"*")&&!strcmp(b[i],"1")){strcmp(o[i],"=");b[i][0]=0;}
    }
}

void cse()
{
    for(int i=0;i<n;i++)
    {
        for(int j=i+1;j<n;j++)
        {
            if(!strcmp(o[i],o[j])&&!strcmp(a[i],a[j])&&!strcmp(b[i],b[j])){
                strcmp(o[j],"=");strcpy(a[j],l[i]);b[j][0]=0;
            }
        }
    }
}

void dead()
{
    int u[M]={0};
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            if(!strcmp(l[i],a[j])||!strcmp(l[i],b[j])) u[i]=1;
        }
    }
    for(int i=0;i<n;i++) u[i]=1;
    for(int i=0;i<n;i++) if(!u[i]) l[i][0]=0;
}

int main()
{
    char s[100];
    while(1)
    {
        fgets(s,100,stdin);
        if(!strncmp(s,"END",3)) break;
        
        char *x=strtok(s," =\n"),*y=strtok(NULL," =\n"),*z=strtok(NULL," =\n"),*w=strtok(NULL," =\n");
        strcpy(l[n],x);
        if(z==NULL)
        {
            strcpy(o[n],"=");
            strcpy(a[n],y);
            b[n][0]=0;
        }
        else
        {
            strcpy(a[n],y);
            strcpy(o[n],z);
            strcpy(b[n],w);
        }
        if(!w){strcpy(o[n],"=");strcpy(a[n],y);b[n][0]=0;}
        else{strcpy(a[n],y);strcpy(o[n],z);strcpy(b[n],w);}
        n++;
    }
    fold();prop();alg();cse();dead();
    for(int i=0;i<n;i++)
    {
        if(l[i][0])
        {
            if(!strcmp(o[i],"=")) printf("%s = %s\n",l[i],a[i]);
            else printf("%s = %s %s %s\n",l[i],a[i],o[i],b[i]);
        }
    }
}