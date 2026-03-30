#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>

char input[100];
int pos = 0;

int E();
int T();
int F();

error(){
    printf("Syntax Error at position %d\n", pos);
    exit(1);
}

int E(){
    int val = T();
    while (input[pos] == '+'){
        pos++;
        val = val + T();
    }
    return val;
}

int T(){
    int val = F();
    while(input[pos] == '*'){
        pos++;
        val = val * F();
    }
    return val;
}

int F(){
    int val;
    if(input[pos] == '('){
        pos++;
        val = E();
        if(input[pos] == ')'){
            pos++;
        }
        else{
            error();
        }
    }
    else if (isdigit(input[pos])){
        val = input[pos] - '0';
        pos++;
    }
    else{
        error();
    }
    return val;
}

int main(){
    printf("Enter Expression: ");
    scanf("%s", input);
    int result = E();
    if (input[pos] != '\0'){
        error();
    }
    printf("Result = %d\n", result);
    return 0;
}