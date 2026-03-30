#include <stdio.h>
#include <string.h>

/*
 * Grammar:
 *   E  -> T E'      E' -> + T E' | e      T  -> F T'
 *   T' -> * F T' | e      F  -> ( E ) | i
 *
 * Internals: E'='X', T'='Y', id='i'
 *
 *        +      *      (      )      i      $
 *  E     -      -      TX     -      TX     -
 *  X(E') +TX    -      -      e      -      e
 *  T     -      -      FY     -      FY     -
 *  Y(T') e      *FY    -      e      -      e
 *  F     -      -      (E)    -      i      -
 */

const char *T[5][6] = {
    { 0,     0,      "TX",  0,    "TX",  0   },
    { "+TX", 0,      0,     "e",  0,     "e" },
    { 0,     0,      "FY",  0,    "FY",  0   },
    { "e",   "*FY",  0,     "e",  0,     "e" },
    { 0,     0,      "(E)", 0,    "i",   0   },
};

int ni(char c){ return c=='E'?0:c=='X'?1:c=='T'?2:c=='Y'?3:c=='F'?4:-1; }
int ti(char c){ return c=='+'?0:c=='*'?1:c=='('?2:c==')'?3:c=='i'?4:c=='$'?5:-1; }
int isNT(char c){ return ni(c)!=-1; }

char stk[200]; int top=-1;
void push(char c){ stk[++top]=c; }
char pop(){ return stk[top--]; }
char peek(){ return stk[top]; }

int main(){
    char raw[100], in[100];
    printf("Enter input (use 'id' for identifier): ");
    scanf("%s", raw);

    /* validate and convert: id->'i' */
    int ri=0, ii=0;
    while(raw[ri]){
        if(raw[ri]=='i'&&raw[ri+1]=='d'){ in[ii++]='i'; ri+=2; }
        else if(ti(raw[ri])!=-1&&raw[ri]!='$'&&raw[ri]!='i'){ in[ii++]=raw[ri++]; }
        else{ printf("Invalid token '%c'\n",raw[ri]); return 1; }
    }
    in[ii++]='$'; in[ii]=0;

    push('$'); push('E');
    int ip=0;

    printf("\nStack\t\tInput\t\tAction\n");
    printf("----------------------------------------\n");

    while(1){
        char X=peek(), a=in[ip];

        /* print stack */
        for(int i=0;i<=top;i++) printf("%c",stk[i]); printf("\t\t");
        /* print remaining input */
        for(int i=ip;in[i];i++) printf("%c",in[i]); printf("\t\t");

        if(X=='$'&&a=='$'){ printf("Accept\n"); return 0; }

        if(!isNT(X)){
            if(X==a){ printf("Match %c\n",X); pop(); ip++; }
            else    { printf("Error: mismatch\n"); return 1; }
        } else {
            int r=ni(X), c=ti(a);
            if(c==-1||!T[r][c]){ printf("Error: no rule\n"); return 1; }
            printf("%c -> %s\n", X, T[r][c]);
            pop();
            if(T[r][c][0]!='e')
                for(int k=strlen(T[r][c])-1;k>=0;k--) push(T[r][c][k]);
        }
    }
}