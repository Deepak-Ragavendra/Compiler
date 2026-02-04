#include<stdio.h>
#include<ctype.h>
#include<string.h>

int FOR(),BLOCK(),EXPR(),EXPR_(),TERM(),TERM_(),FACTOR();

const char *cursor;
char str[100];

int FOR(){
    if(*cursor=='f'){
        cursor++;
        if(*cursor=='o'){
            cursor++;
            if(*cursor=='r'){
                cursor++;
                if(*cursor=='('){
                    cursor++;
                    if(EXPR()){
                        if(*cursor==';'){
                            cursor++;
                            if(EXPR()){
                                if(*cursor==';'){
                                    cursor++;
                                    if(EXPR()){
                                        if(*cursor==')'){
                                            cursor++;
                                            if(BLOCK()){
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
                                        return 0;
                                    }
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
                            return 0;
                        }
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
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }
}

int BLOCK(){
    if(*cursor=='{'){
        cursor++;
        if(*cursor=='}'){
            cursor++;
            return 1;
        }
        else return 0;
    }
    else return 0;
}

int EXPR(){
    if(TERM()){
        if(EXPR_()){
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

int EXPR_(){
    if(*cursor=='+'){
        cursor++;
        if(TERM()){
            if(EXPR_()){
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
        if(TERM()){
            if(EXPR_()){
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
    else return 1;
}

int TERM(){
    if(FACTOR()){
        if(TERM_()) return 1;
        else return 0;
    }else return 0;
}

int TERM_(){
    if(*cursor=='*'){
        cursor++;
        if(FACTOR()){
            if(TERM_()) return 1;
            else return 0;
        }else return 0;
    }
    else if(*cursor=='/'){
        cursor++;
        if(FACTOR()){
            if(TERM_()) return 1;
            else return 0;
        }else return 0;
    }
    else return 1;
}

int FACTOR(){
    if(isdigit(*cursor)){
        cursor++;
        return 1;
    }
    else if(isalpha(*cursor)){
        cursor++;
        return 1;
    }
    else if(*cursor=='('){
        cursor++;
        if(EXPR()){
            if(*cursor==')'){
                return 1;
            }else return 0;
        }else return 0;
    }
    else{
        return 0;
    }
}

int main(){
    scanf("%s",str);
    cursor=str;
    if(FOR() && *cursor=='\0'){
        printf("Valid for-loop syntax\n");
    }
    else{
        printf("Invalid for-loop syntax\n");
    }
}