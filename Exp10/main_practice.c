#include <stdio.h>
#include <ctype.h>
#include <string.h>

char expr[100];
int pos=0;
int tempCount=1;

char code[100][100];
int linecount=0;

void skipspaces(){
    while(expr[pos]==' ' || expr[pos]=='\t' || expr[pos]=='\n'){
        pos++;
    }
}

void newtemp(char temp[]){
    sprintf(temp,"t%d",tempCount++);
}

void addCode(const char *result,const char *arg1,char op,const char *arg2){
    sprintf(code[linecount++],"%s = %s %c %s",result,arg1,op,arg2);
}

void Copystr(char *dest,char *src){
    strcpy(dest,src);
}

void F(char place[]);
void T(char place[]);
void E(char place[]);

void F(char place[]){
    skipspaces();

    if(expr[pos]=='('){
        pos++;
        E(place);
        skipspaces();
        if(expr[pos]==')'){
            pos++;
        }
    }
    else if(isalnum(expr[pos])){
        int i=0;
        while(isalnum(expr[pos])){
            place[i++]=expr[pos];
            pos++;
        }
        place[i]='\0';
    }
}

void T(char place[]){
    char left[20],right[20],temp[20];
    F(left);
    skipspaces();

    while(expr[pos]=='*' || expr[pos]=='/'){
        char op=expr[pos++];
        F(right);
        newtemp(temp);
        addCode(temp,left,op,right);
        Copystr(left,temp);
        skipspaces();   
    }

    Copystr(place,left);
}

void E(char place[]){
    char left[20],right[20],temp[20];
    T(left);

    skipspaces();
    while(expr[pos]=='+' || expr[pos]=='-'){
        char op=expr[pos++];
        T(right);
        newtemp(temp);
        addCode(temp,left,op,right);
        Copystr(left,temp);
        skipspaces();
    }
    Copystr(place,left);
    
}

int main(){
    char result[20];

    printf("Enter the Expression: ");
    fgets(expr,sizeof(expr),stdin);

    expr[strcspn(expr,"\n")]='\0';

    E(result);

    printf("The Generated TAC:\n");
    for(int i=0;i<linecount;i++){
        printf("%s\n",code[i]);
    }

    return 0;
}