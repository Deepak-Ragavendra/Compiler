#include <stdio.h>
#include <string.h>

/*
 * Grammar:
 *   1: E -> E + T
 *   2: E -> T
 *   3: T -> T * F
 *   4: T -> F
 *   5: F -> ( E )
 *   6: F -> id
 *
 * action: >0 shift, <0 reduce, 99 accept, 0 error
 */

int action[12][6] = {
/*       id   +    *    (    )    $  */
/* 0 */ { 5,  0,   0,   4,   0,   0 },
/* 1 */ { 0,  6,   0,   0,   0,  99 },
/* 2 */ { 0, -2,   7,   0,  -2,  -2 },
/* 3 */ { 0, -4,  -4,   0,  -4,  -4 },
/* 4 */ { 5,  0,   0,   4,   0,   0 },
/* 5 */ { 0, -6,  -6,   0,  -6,  -6 },
/* 6 */ { 5,  0,   0,   4,   0,   0 },
/* 7 */ { 5,  0,   0,   4,   0,   0 },
/* 8 */ { 0,  6,   0,   0,  11,   0 },
/* 9 */ { 0, -1,   7,   0,  -1,  -1 },
/*10 */ { 0, -3,  -3,   0,  -3,  -3 },
/*11 */ { 0, -5,  -5,   0,  -5,  -5 },
};

int gt[12][3] = {
/*      E  T  F */
/* 0 */ {1, 2, 3},
/* 1 */ {0, 0, 0},
/* 2 */ {0, 0, 0},
/* 3 */ {0, 0, 0},
/* 4 */ {8, 2, 3},
/* 5 */ {0, 0, 0},
/* 6 */ {0, 9, 3},
/* 7 */ {0, 0,10},
/* 8 */ {0, 0, 0},
/* 9 */ {0, 0, 0},
/*10 */ {0, 0, 0},
/*11 */ {0, 0, 0},
};

int rlen[] = {0, 3, 1, 3, 1, 3, 1};   /* RHS length per rule */
int rlhs[] = {0, 0, 0, 1, 1, 2, 2};   /* LHS index per rule: E=0,T=1,F=2 */

int stk[200], top = -1;
void push(int x) { stk[++top] = x; }
void pop(int n)  { top -= n; }

int ti(char c){ return c=='i'?0:c=='+'?1:c=='*'?2:c=='('?3:c==')'?4:c=='$'?5:-1; }

int main(){
    char raw[100], in[100];
    printf("Enter input (use id): ");
    scanf("%s", raw);

    /* convert id->'i', validate */
    int ri=0, ii=0;
    while(raw[ri]){
        if(raw[ri]=='i'&&raw[ri+1]=='d'){ in[ii++]='i'; ri+=2; }
        else if(ti(raw[ri])!=-1&&raw[ri]!='i'&&raw[ri]!='$'){ in[ii++]=raw[ri++]; }
        else{ printf("Invalid: %c\n",raw[ri]); return 1; }
    }
    in[ii++]='$'; in[ii]=0;

    push(0);
    int ip = 0;

    printf("\nStack\t\tInput\t\tAction\n");
    printf("----------------------------------------------\n");

    while(1){
        int s = stk[top];
        char a = in[ip];
        int act = action[s][ti(a)];

        /* print stack */
        for(int i=0; i<=top; i++) printf("%d ", stk[i]);
        printf("\t\t");

        /* print input */
        for(int i=ip; in[i]; i++) printf("%c", in[i]);
        printf("\t\t");

        if(act == 99){
            printf("Accept\n");
            return 0;
        } else if(act > 0){
            printf("Shift %d\n", act);
            push(act); ip++;
        } else if(act < 0){
            int r = -act;
            printf("Reduce R%d (%c->...)\n", r, "ETF"[rlhs[r]]);
            pop(rlen[r]);
            push(gt[stk[top]][rlhs[r]]);
        } else {
            printf("Error\n");
            return 1;
        }
    }
}