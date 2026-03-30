#include<stdio.h>
#include<string.h>
#include<ctype.h>

struct Symbol
{
    char name[50];
    char type[20];
};

struct Symbol table[100];
int count=0;

int isKeyword(char s[])
{
    return strcmp(s,"int")==0 || strcmp(s,"float")==0 || strcmp(s,"char")==0;
}

int findSymbol(char s[])
{
    for(int i=0;i<count;i++)
        if(strcmp(table[i].name,s)==0)
            return i;
    return -1;
}

void insertSymbol(char name[],char type[])
{
    strcpy(table[count].name,name);
    strcpy(table[count].type,type);
    count++;
}

int isNumber(char s[])
{
    int i=0,dot=0;
    if(s[0]=='\0') return 0;

    while(s[i])
    {
        if(s[i]=='.')
        {
            dot++;
            if(dot>1) return 0;
        }
        else if(!isdigit(s[i]))
            return 0;
        i++;
    }
    return 1;
}

int isCharLiteral(char s[])
{
    return strlen(s)==3 && s[0]=='\'' && s[2]=='\'';
}

char* getType(char s[])
{
    if(isCharLiteral(s)) return "char";
    if(isNumber(s))
    {
        for(int i=0;s[i];i++)
            if(s[i]=='.')
                return "float";
        return "int";
    }

    int idx=findSymbol(s);
    if(idx!=-1) return table[idx].type;

    return "invalid";
}

int compatible(char left[],char right[])
{
    if(strcmp(left,right)==0) return 1;
    if(strcmp(left,"float")==0 && strcmp(right,"int")==0) return 1;
    return 0;
}

void trim(char s[])
{
    int i=0,j=0;
    char t[100];
    while(s[i])
    {
        if(s[i]!=' ' && s[i]!='\t' && s[i]!='\n')
            t[j++]=s[i];
        i++;
    }
    t[j]='\0';
    strcpy(s,t);
}

int main()
{
    int n;
    char line[100],type[20],lhs[50],rhs[50];

    printf("Enter number of statements: ");
    scanf("%d",&n);
    getchar();

    for(int i=0;i<n;i++)
    {
        fgets(line,sizeof(line),stdin);
        line[strcspn(line,"\n")]='\0';

        if(sscanf(line,"%s %[^;];",type,lhs)==2 && isKeyword(type))
        {
            trim(lhs);
            if(findSymbol(lhs)!=-1)
                printf("Semantic Error: Multiple declaration of %s\n",lhs);
            else
                insertSymbol(lhs,type);
        }
        else if(sscanf(line,"%[^=]=%[^;];",lhs,rhs)==2)
        {
            trim(lhs);
            trim(rhs);

            int li=findSymbol(lhs);
            if(li==-1)
            {
                printf("Semantic Error: Undeclared variable %s\n",lhs);
                continue;
            }

            char *rt=getType(rhs);
            if(strcmp(rt,"invalid")==0)
                printf("Semantic Error: Undeclared variable or invalid literal %s\n",rhs);
            else if(!compatible(table[li].type,rt))
                printf("Semantic Error: Type mismatch in assignment to %s\n",lhs);
        }
        else
        {
            printf("Invalid statement: %s\n",line);
        }
    }

    printf("\nSymbol Table:\n");
    printf("Name\tType\n");
    for(int i=0;i<count;i++)
        printf("%s\t%s\n",table[i].name,table[i].type);

    return 0;
}