#include<stdio.h>
#include<stdlib.h>

char term[10],nterm[10];
char lhs[10],rhs[10][20];

int main(){
    int n;
    printf("Enter the number of productions: ");
    scanf("%d",&n);
    printf("Enter the productions in this format (E TX |)");
    for(int i=0;i<n;i++){
        scanf(" %c %s",&lhs,rhs);
    }
    

    return 0;
}