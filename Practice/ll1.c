#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const char *T[5][6]={
    { 0,     0,      "TX",  0,    "TX",  0   },
    { "+TX", 0,      0,     "e",  0,     "e" },
    { 0,     0,      "FY",  0,    "FY",  0   },
    { "e",   "*FY",  0,     "e",  0,     "e" },
    { 0,     0,      "(E)", 0,    "i",   0   },
};

char st[200];
int top=-1;
void push(char a){ st[++top]=a;}
char pop(){ return st[top--];}
char peek(){ return st[top];}

int ni(char c){ return c=='X'?1:c=='T'?2:c=='E'?0:c=='Y'?3:c=='F'?4:-1;}
int ti(char c){ return c=='+'?0:c=='*'?1:c=='('?2:c==')'?3:c=='$'?5:c=='i'?4:-1;}
int Nt(char c){ return ni(c)!=-1;}


int main(){
    char raw[20],in[20];
    printf("Enter the input satement (id as for identifier): ");
    scanf("%s",raw);
    int ii=0,ri=0;

    while(raw[ri]){
        if(raw[ri]=='i' && raw[ri+1]=='d'){ in[ii++]='i';ri+=2;}
        else if(ti(raw[ri])!=-1 && raw[ri]!='i' && raw[ri]!='$'){ in[ii++]=raw[ri++];}
        else {printf("Enter a valid token %c",raw[ri]);return 1;}
    }

    in[ii++]='$';
    in[ii]=0;

    push('$');
    push('E');
    int ip=0;
    printf("\nStack\t\tInput\t\tAction\n");
    printf("----------------------------------------\n");

    while(1){
        char X=peek();
        char a=in[ip];
        for(int i=0;i<=top;i++){
            printf("%c",st[i]);
        }
        printf("\t\t");
        for(int i=ip;in[i];i++){
            printf("%c",in[i]);
        }
        printf("\t\t");

        if(X=='$' && a=='$'){ printf("Accept\n");return 0;}

        if(!Nt(X)){
            if(X==a){
                printf("Match %c\n",X);
                pop();
                ip++;
            }
            else{
                printf("Error \n");
                return 1;
            }
        }
        else{
            int r=ni(X),c=ti(a);
            if(c==-1 || !T[r][c]){
                printf("Error\n");
                return 1;
            }
            printf("%c -> %s\n",X,T[r][c]);
            pop();
            if(T[r][c][0]!='e'){
                for(int k=strlen(T[r][c])-1;k>=0;k--){
                    push(T[r][c][k]);
                }
            }
        }

    }
}