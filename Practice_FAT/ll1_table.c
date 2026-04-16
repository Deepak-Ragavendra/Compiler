#include <stdio.h>
#include <string.h>

typedef struct {
    char name;
    char first[10];
    char follow[10];
} Symbol;

typedef struct {
    char lhs;
    char rhs[10];
} Production;

// Your specific pos function
int pos(char arr[], int n, char c) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == c)
            return i;
    }
    return -1;
}

int main() {
    int n, nt_cnt = 0, t_cnt = 0;
    Production prods[10];
    Symbol nt[10];         
    char nonterminals[10]; 
    char terminals[10];
    char table[10][10][15];

    printf("Enter number of productions: ");
    scanf("%d", &n);
    getchar(); 

    printf("Enter productions (format: 'S AB', use '#' for epsilon):\n");
    for (int p_idx = 0; p_idx < n; p_idx++) {
        char buffer[25];
        fgets(buffer, 25, stdin);
        sscanf(buffer, "%c->%s", &prods[p_idx].lhs, prods[p_idx].rhs);
        
        if (pos(nonterminals, nt_cnt, prods[p_idx].lhs) == -1) {
            nt[nt_cnt].name = prods[p_idx].lhs;
            nonterminals[nt_cnt] = prods[p_idx].lhs; 
            nt[nt_cnt].first[0] = '\0';
            nt[nt_cnt].follow[0] = '\0';
            nt_cnt++;
        }
    }

    for (int i = 0; i < nt_cnt; i++) {
        printf("\nEnter First(%c): ", nt[i].name);
        scanf("%s", nt[i].first);
        printf("Enter Follow(%c): ", nt[i].name);
        scanf("%s", nt[i].follow);
    }

    printf("\nEnter all unique Terminals (e.g., ab$): ");
    scanf("%s", terminals);
    t_cnt = strlen(terminals);

    // Initialize Table
    for (int nt_row = 0; nt_row < nt_cnt; nt_row++)
        for (int col = 0; col < t_cnt; col++)
            strcpy(table[nt_row][col], " - ");

    // Build Table Logic
    for (int p_idx = 0; p_idx < n; p_idx++) {

        int nt_row = pos(nonterminals, nt_cnt, prods[p_idx].lhs);
        char alpha = prods[p_idx].rhs[0];

        // Rule 1: A -> #
        if (alpha == '#') {
            for (int f_idx = 0; nt[nt_row].follow[f_idx]; f_idx++) {
                int col = pos(terminals, t_cnt, nt[nt_row].follow[f_idx]);
                if (col != -1)
                    sprintf(table[nt_row][col], "%c->%s", prods[p_idx].lhs, prods[p_idx].rhs);
            }
        }

        // Rule 2: A -> terminal
        else if (pos(nonterminals, nt_cnt, alpha) == -1) {
            int col = pos(terminals, t_cnt, alpha);
            if (col != -1)
                sprintf(table[nt_row][col], "%c->%s", prods[p_idx].lhs, prods[p_idx].rhs);
        }

        // Rule 3: A -> non-terminal
        else {
            int target_nt_idx = pos(nonterminals, nt_cnt, alpha);
            for (int first_idx = 0; nt[target_nt_idx].first[first_idx]; first_idx++) {
                char f = nt[target_nt_idx].first[first_idx];

                if (f != '#') {
                    int col = pos(terminals, t_cnt, f);
                    if (col != -1)
                        sprintf(table[nt_row][col], "%c->%s", prods[p_idx].lhs, prods[p_idx].rhs);
                }
                else {
                    for (int follow_idx = 0; nt[nt_row].follow[follow_idx]; follow_idx++) {
                        int col = pos(terminals, t_cnt, nt[nt_row].follow[follow_idx]);
                        if (col != -1)
                            sprintf(table[nt_row][col], "%c->%s", prods[p_idx].lhs, prods[p_idx].rhs);
                    }
                }
            }
        }
    }

    // Output Table
    printf("\nLL(1) Parsing Table:\n\n\t");
    for (int col = 0; col < t_cnt; col++) printf("%c\t", terminals[col]);
    printf("\n--------------------------------------------------\n");
    for (int nt_row = 0; nt_row < nt_cnt; nt_row++) {
        printf("%c |\t", nt[nt_row].name);
        for (int col = 0; col < t_cnt; col++) printf("%s\t", table[nt_row][col]);
        printf("\n");
    }

    return 0;
}