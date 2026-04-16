#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

char input[100];
int pos = 0;

int L(), A(), R(), e(), t(), f();

void skip_spaces() {
    while(isspace(input[pos])) pos++;
}

// OR ||
int L() {
    int val = A();
    skip_spaces();
    while(input[pos]=='|' && input[pos+1]=='|') {
        pos += 2;
        int val2 = A();
        val = val || val2;
        skip_spaces();
    }
    return val;
}

// AND &&
int A() {
    int val = R();
    skip_spaces();
    while(input[pos]=='&' && input[pos+1]=='&') {
        pos += 2;
        int val2 = R();
        val = val && val2;
        skip_spaces();
    }
    return val;
}

// Relational
int R() {
    int val = e();
    skip_spaces();

    if(input[pos]=='>' ) {
        pos++;
        int val2 = e();
        return val > val2;
    }
    else if(input[pos]=='<' ) {
        pos++;
        int val2 = e();
        return val < val2;
    }

    return val; // no relation → just arithmetic result
}

// Arithmetic (+)
int e() {
    int val = t();
    skip_spaces();
    while(input[pos]=='+') {
        pos++;
        int val2 = t();
        val += val2;
        skip_spaces();
    }
    return val;
}

// Arithmetic (*)
int t() {
    int val = f();
    skip_spaces();
    while(input[pos]=='*') {
        pos++;
        int val2 = f();
        val *= val2;
        skip_spaces();
    }
    return val;
}

// Factor
int f() {
    skip_spaces();

    if(input[pos]=='(') {
        pos++;
        int val = L();  // 🔥 now calls logical level
        if(input[pos]!=')') {
            printf("Missing )\n");
            exit(1);
        }
        pos++;
        return val;
    }

    if(isdigit(input[pos])) {
        int val = 0;
        while(isdigit(input[pos])) {
            val = val*10 + (input[pos]-'0');
            pos++;
        }
        return val;
    }

    printf("Syntax Error\n");
    exit(1);
}

int main() {
    printf("Enter expression: ");
    fgets(input, 100, stdin);

    int result = L();

    printf("Result: %d\n", result);
    return 0;
}