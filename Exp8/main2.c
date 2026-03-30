#include<stdio.h>
#include<string.h>

char name[20][20], type[20][10];
int count=0;

// find variable
int find(char s[]){
    for(int i=0;i<count;i++)
        if(strcmp(name[i],s)==0) return i;
    return -1;
}

int main(){
    int n;
    char t[10], a[20], b[20];

    scanf("%d",&n);

    for(int i=0;i<n;i++){

        // declaration: int a;
        scanf("%s %[^;];",t,a);

        if(strcmp(t,"int")==0 || strcmp(t,"float")==0 || strcmp(t,"char")==0){

            if(find(a)!=-1)
                printf("Multiple declaration of %s\n",a);
            else{
                strcpy(name[count],a);
                strcpy(type[count],t);
                count++;
            }
        }

        else{
            // assignment: a=b;
            sscanf(t,"%[^=]",a);
            scanf("=%[^;];",b);

            int i=find(a), j=find(b);

            if(i==-1)
                printf("Undeclared variable %s\n",a);
            else if(j!=-1 && strcmp(type[i],type[j])!=0)
                printf("Type mismatch in %s\n",a);
        }
    }

    printf("\nSymbol Table:\n");
    for(int i=0;i<count;i++)
        printf("%s %s\n",name[i],type[i]);

    return 0;
}