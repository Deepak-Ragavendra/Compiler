#include<stdio.h>
#include<string.h>
#include<ctype.h>

int keyword(char *token){
    char *key[]={"int","main","void","float","for","while"};
    for(int i=0;i<6;i++){
        if(strcmp(token,key[i])==0){
            return 1;
        }
    }
    return 0;
}

int constant(char *token){
    char *con[]={"1","2","3","4","5","6","7","8","9","10","0"};
    for(int i=0;i<11;i++){
        if(strcmp(token,con[i])==0){
            return 1;
        }
    }
    return 0;
}

int Operator(char a){
    char oper[]={'+','-','/','*','<','>','='};
    for(int i=0;i<7;i++){
        if(a==oper[i]){
            return 1;
        }
    }
    return 0;
}

int special(char a){
    char spe[]={'{','}',';',',','(',')',',','.'};
    for(int i=0;i<8;i++){
        if(a==spe[i]){
            return 1;
        }
    }
    return 0;
}

void Identifier(char *token,char *arr_i[],int arr_ic[]){
    int i=0;
    while(arr_i[i]!=NULL){
        if(strcmp(token,arr_i[i])==0){
            arr_ic[i]=arr_ic[i]+1;
            return;
        }
        i++;
    }
    arr_i[i]=strdup(token);
    arr_ic[i]=1;
}

int Identify(char *token){
    if(!isalpha(token[0])&&(token[0]!='_')){
        return 0;
    }
    for(int i=0;token[i]!='\0';i++){
        if(!isalnum(token[i]) && token[i]!='_'){
            return 0;
        }
    }
    return 1;
}


int main(){
    FILE *file;
    file=fopen("code.txt","r");
    char text[5000];
    char ch;
    char token[100];
    char *arr_i[100]={NULL};
    int arr_ic[100];
    int j=0;
    while((ch=fgetc(file))!=EOF){
        text[j++]=ch;
    }
    text[j]='\0';
    j=0;
    int l=1;
    printf("Line: %d\n",l++);
    for(int i=0;text[i]!='\0';i++){
        if(isspace(text[i])){
            if(text[i]=='\n'){
                printf("Line: %d\n",l++);
            }
            if(j!=0){
                token[j]='\0';
                if(keyword(token)){
                    printf("Keyword: %s\n",token);
                }
                else if(constant(token)){
                    printf("Constant: %s\n",token);
                }
                else if(Identify(token)){
                    Identifier(token,arr_i,arr_ic);
                    printf("Identifier: %s\n",token);
                }
                else printf("Unknown\n");
                j=0;
            }
            if(j==0){
                continue;
            }
        }
        if(Operator(text[i])){
            if(j!=0){
                token[j]='\0';
                if(keyword(token)){
                    printf("Keyword: %s\n",token);
                }
                else if(constant(token)){
                    printf("Constant: %s\n",token);
                }
                else if(Identify(token)){
                    Identifier(token,arr_i,arr_ic);
                    printf("Identifier: %s\n",token);
                }
                else{
                    printf("Unkown\n");
                }
                j=0;
            }
            printf("Operator: %c\n",text[i]);
            if(j==0) continue;
        }
        if(special(text[i])){
            if(j!=0){
                token[j]='\0';
                if(keyword(token)){
                    printf("Keyword: %s\n",token);
                }
                else if(constant(token)){
                    printf("Constant: %s\n",token);
                }
                else if(Identify(token)){
                    Identifier(token,arr_i,arr_ic);
                    printf("Identifier: %s\n",token);
                }
                j=0;
            }
            printf("Special Character: %c\n",text[i]);
            if(j==0) continue;
        }
        else{
            token[j++]=text[i];
        }
    }
    printf("\n");
    printf("\n");
    printf("Unique Identifiers\n");
    int i=0;
    while(arr_i[i]!=NULL){
        printf("%s : %d\n",arr_i[i],arr_ic[i]);
        i++;
    }
    return 0;
}