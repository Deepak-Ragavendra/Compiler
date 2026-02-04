#include<stdio.h>
#include<string.h>
#include<ctype.h>

int E(),T(),E_(),T_(),F();
const char *cursor;
char str[100];

int E(){
    if(T()){
        if(E_()){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }
}

int T(){
    if(F()){
        if(T_()){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }
}

int E_(){
    if(*cursor=='+'){
        cursor++;
        if(T()){
            if(E_()){
                return 1;
            }
            else{
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    else if(*cursor=='-'){
        cursor++;
        if(T()){
            if(E_()){
                return 1;
            }
            else{
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    else{
        return 1;
    }
}

int T_(){
    if(*cursor=='*'){
        cursor++;
        if(F()){
            if(T_()){
                return 1;
            }
            else{
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    else if(*cursor=='/'){
        cursor++;
        if(F()){
            if(T_()){
                return 1;
            }
            else{
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    else{
        return 1;
    }
}

int F(){
    if(*cursor=='('){
        cursor++;
        if(E()){
            if(*cursor==')'){
                cursor++;
                return 1;
            }
            else{
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    else if(isdigit(*cursor)){
        cursor++;
        while(isdigit(*cursor)){
            cursor++;
        }
        return 1;
    }
    else if(isalpha(*cursor)){
        cursor++;
        return 1;
    }
    else{
        return 0;
    }
}


int main(){
    scanf("%s",str);
    cursor=str;
    
    if(E() && *cursor=='\0'){
        printf("Parsing Successful\n");
    }
    else{
        printf("Parsing Failed\n");
    }
}