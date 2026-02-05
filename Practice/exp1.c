#include<stdio.h>
#include<string.h>
#include<ctype.h>

int Constant(char *token){
    char *c[]={"10","20"};
    for(int i=0;i<2;i++){
        if(strcmp(token,c[i])==0){
            return 1;
        }
    }
    return 0;
}

int Keyword(char *token){
    char *c[]={"int","main","void"};
    for(int i=0;i<3;i++){
        if(strcmp(token,c[i])==0){
            return 1;
        }
    }
    return 0;
}

int Identifier(char *token){
    char *c[]={"a","b","num1"};
    for(int i=0;i<3;i++){
        if(strcmp(token,c[i])==0){
            return 1;
        }
    }
    return 0;
}

int Special(char token){
    char c[]={'{','}','(',')',';'};
    for(int i=0;i<5;i++){
        if(token==c[i]){
            return 1;
        }
    }
    return 0;
}

int Operator(char token){
    char c[]={'+','-','=','/'};
    for(int i=0;i<4;i++){
        if(token==c[i]){
            return 1;
        }
    }
    return 0;
}



int main(){
    char text[]="int main () \n int a ; \n a = a + 10 ; \n }";
    char token[100];
    int j=0;
    int line=1;
    printf("Line:1\n");
    
    for(int i=0;text[i]!='\0';i++){
        if(isspace(text[i])){
            if(text[i]=='\n'){
                printf("Line:%d\n",++line);
            }
            if(j!=0){
                token[j]='\0';
                if(Constant(token)){
                    printf("Constant: %s\n",token);
                }
                else if(Keyword(token)){
                    printf("Keyword: %s\n",token);
                }
                else if(Identifier(token)){
                    printf("Identifier: %s\n",token);
                }
                else{
                    printf("Unknown\n");
                }
                j=0;
            }
            continue;
        }
        else if(Operator(text[i])){
            if(j!=0){
                token[j]='\0';
                if(Constant(token)){
                    printf("Constant: %s\n",token);
                }
                else if(Keyword(token)){
                    printf("Keyword: %s\n",token);
                }
                else if(Identifier(token)){
                    printf("Identifier: %s\n",token);
                }
                else{
                    printf("Unknown\n");
                }
                j=0;
            }
            printf("Operator: %c\n",text[i]);
            continue;
        }
        else if(Special(text[i])){
            if(j!=0){
                token[j]='\0';
                if(Constant(token)){
                    printf("Constant: %s\n",token);
                }
                else if(Keyword(token)){
                    printf("Keyword: %s\n",token);
                }
                else if(Identifier(token)){
                    printf("Identifier: %s\n",token);
                }
                else{
                    printf("Unknown\n");
                }
                j=0;
            }
            printf("Special Character: %c\n",text[i]);
            continue;
        }
        token[j++]=text[i];
        
    }
    return 0;
}