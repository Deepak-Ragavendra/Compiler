#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX 100

typedef struct{
    char res[10],op1[10],op2[10];
    char op;
    int isValid;
} TAC;

TAC t[MAX];
int n=0;

int isNum(char* s){
    for(int i=0;s[i];i++){
        if(!isdigit(s[i])) return 0;
    }
    return 1;
}

void parse(char* line){
    TAC x;
    
    int c=sscanf(line,"%s = %s %c %s",x.res,x.op1,&x.op,x.op2);

    if(c==2){
        sscanf(line,"%s = %s",x.res,x.op1);
        x.op='=';
        strcpy(x.op2,"");
    }

    x.isValid=1;
    t[n++]=x;
}

void printexpr(TAC e){
    if(e.op=='='){
        printf("%s = %s",e.res,e.op1);
    }
    else{
        printf("%s = %s %c %s",e.res,e.op1,e.op,e.op2);
    }
}

int compute(int a,int b,char op){
    if(op=='+') return a+b;
    if(op=='-') return a-b;
    if(op=='/' && b!=0) return a/b;
    else return a*b;
}

void constantfolding(){
    printf("\nConstant Folding:\n");
    for(int i=0;i<n;i++){
        if(t[i].op!='=' && isNum(t[i].op1) && isNum(t[i].op2)){
            int val=compute(atoi(t[i].op1),atoi(t[i].op2),t[i].op);
            sprintf(t[i].op1,"%d",val);
            strcpy(t[i].op2,"");
            t[i].op='=';
        }
        printexpr(t[i]);
        printf("\n");
    }
}

void constantPropagation(){
    printf("\nConstant Propagation:\n");
    for(int i=0;i<n;i++){
        if(t[i].op=='=' && isNum(t[i].op1)){
            for(int j=i+1;j<n;j++){
                if(strcmp(t[j].op1,t[i].res)==0){
                    strcpy(t[j].op1,t[i].op1);
                }
                if(strcmp(t[j].op2,t[i].res)==0){
                    strcpy(t[j].op2,t[i].op1);
                }
            }
        }
        printexpr(t[i]);
        printf("\n");
    }
}

void constantcopy(){
    printf("\nCopy Propagation:\n");
    for(int i=0;i<n;i++){
        if(t[i].op=='=' && !isNum(t[i].op1)){
            for(int j=i+1;j<n;j++){
                if(strcmp(t[j].op1,t[i].res)==0){
                    strcpy(t[j].op1,t[i].op1);
                }
                if(strcmp(t[j].op2,t[i].res)==0){
                    strcpy(t[j].op2,t[i].op1);
                }
            }
        }
        printexpr(t[i]);
        printf("\n");
    }
}

void CSE(){
    printf("\nCommon Subexpression Elimination:\n");
    for(int i=0;i<n;i++){
        for(int j=0;j<i;j++){
            if(t[i].op==t[j].op && ((strcmp(t[i].op1,t[j].op1)==0 && strcmp(t[i].op2,t[j].op2)==0) || (strcmp(t[i].op1,t[j].op2)==0 && strcmp(t[i].op2,t[j].op1)))){
                strcpy(t[i].op1,t[j].res);
                strcpy(t[i].op2,"");
                t[i].op='=';
                break;
            }
        }
        printexpr(t[i]);
        printf("\n");
    }
}

void algebraisimplification(){
    printf("\nAlgebraic Simplification:\n");
    for(int i=0;i<n;i++){
        if(t[i].op=='+'){
            if(strcmp(t[i].op1,"0")==0){
                t[i].op='=';
                strcpy(t[i].op1,t[i].op2);
                strcpy(t[i].op2,"");
            }
            else if(strcmp(t[i].op2,"0")==0){
                t[i].op='=';
                strcpy(t[i].op2,"");
            }
        }
        else if(t[i].op=='*'){
            if(strcmp(t[i].op2,"1")==0){
                t[i].op='=';
                strcpy(t[i].op2,"");
            }
            else if(strcmp(t[i].op2,"0")==0){
                t[i].op='=';
                strcpy(t[i].op1,"0");
                strcpy(t[i].op2,"");
            }
        }
        printexpr(t[i]);
        printf("\n");
    }
}

void deadcodeElimination(){
    printf("\nElimination:\n");
    for(int i=0;i<n;i++){
        int used=0;
        for(int j=i+1;j<n;j++){
            if(strcmp(t[i].res,t[j].op1)==0 || strcmp(t[i].res,t[j].op2)==0){
                used=1;
                break;
            }
        }
        if(used || i==n-1){
            printexpr(t[i]);
        }
        else{
            printf("Eliminated: %s\n",t[i].res);
        }
    }
}

int main(){
    char line[100];
    printf("Enter the TAC code (END to stop):\n");
    while(1){
        fgets(line,sizeof(line),stdin);
        line[strcspn(line,"\n")]=0;

        if(strcmp(line,"END")==0) break;
        parse(line);
    }
    constantfolding();
    constantPropagation();
    constantfolding();
    CSE();
    constantcopy();
    algebraisimplification();
    deadcodeElimination();
}