
/*
 * Full SLR(1) Parser — No Hardcoding
 *
 * Dynamically:
 *  1. Reads any grammar from a file
 *  2. Augments it:  S' -> S
 *  3. Computes FIRST and FOLLOW sets
 *  4. Builds LR(0) canonical item collection
 *  5. Constructs ACTION and GOTO tables
 *  6. Parses any input string
 *
 * Grammar file format (one rule per line):
 *   E -> E + T | T
 *   T -> T * F | F
 *   F -> ( E ) | id
 *
 * Conventions:
 *   - Uppercase first letter  => Non-terminal
 *   - Everything else         => Terminal
 *   - 'e'                     => Epsilon (empty)
 *   - '$'                     => End marker (added automatically)
 *   - '|'                     => Separates alternatives on same line
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ── Limits ─────────────────────────────────────────────────────────── */
#define MAX_SYMS    64
#define MAX_PRODS   64
#define MAX_RHS     12
#define MAX_STATES  300
#define MAX_ITEMS   100

#define ACCEPT_VAL  9999
#define ERROR_VAL  -9999

/* ── Symbol Table ───────────────────────────────────────────────────── */
typedef struct { char name[20]; int is_term; } Symbol;

Symbol sym_tab[MAX_SYMS];
int    sym_cnt = 0;

int find_sym(const char *n) {
    for (int i = 0; i < sym_cnt; i++)
        if (!strcmp(sym_tab[i].name, n)) return i;
    return -1;
}

int add_sym(const char *n, int is_t) {
    int i = find_sym(n);
    if (i >= 0) return i;
    strcpy(sym_tab[sym_cnt].name, n);
    sym_tab[sym_cnt].is_term = is_t;
    return sym_cnt++;
}

#define IS_TERM(s)  (sym_tab[s].is_term)
#define IS_NT(s)   (!sym_tab[s].is_term)
#define SYM(s)      sym_tab[s].name

/* ── Productions ────────────────────────────────────────────────────── */
typedef struct { int lhs; int rhs[MAX_RHS]; int len; } Prod;

Prod prods[MAX_PRODS];
int  prod_cnt = 0;

int start_sym = -1;  /* augmented start */
int end_sym   = -1;  /* '$' */
int eps_sym   = -1;  /* 'e' */

/* ── FIRST / FOLLOW ─────────────────────────────────────────────────── */
int first [MAX_SYMS][MAX_SYMS], first_cnt [MAX_SYMS];
int follow[MAX_SYMS][MAX_SYMS], follow_cnt[MAX_SYMS];

static void set_add(int *arr, int *cnt, int v) {
    for (int i = 0; i < *cnt; i++) if (arr[i] == v) return;
    arr[(*cnt)++] = v;
}
static int set_has(int *arr, int cnt, int v) {
    for (int i = 0; i < cnt; i++) if (arr[i] == v) return 1;
    return 0;
}

void first_of_string(int *alpha, int len, int *out, int *ocnt) {
    *ocnt = 0;
    if (len == 0) { set_add(out, ocnt, eps_sym); return; }
    int all_eps = 1;
    for (int i = 0; i < len; i++) {
        int B = alpha[i];
        for (int k = 0; k < first_cnt[B]; k++)
            if (first[B][k] != eps_sym)
                set_add(out, ocnt, first[B][k]);
        if (!set_has(first[B], first_cnt[B], eps_sym)) { all_eps = 0; break; }
    }
    if (all_eps) set_add(out, ocnt, eps_sym);
}

void compute_first() {
    for (int i = 0; i < sym_cnt; i++)
        if (IS_TERM(i)) set_add(first[i], &first_cnt[i], i);

    int changed = 1;
    while (changed) {
        changed = 0;
        for (int p = 0; p < prod_cnt; p++) {
            int A = prods[p].lhs, old = first_cnt[A];
            int all_eps = 1;
            for (int j = 0; j < prods[p].len; j++) {
                int B = prods[p].rhs[j];
                for (int k = 0; k < first_cnt[B]; k++)
                    if (first[B][k] != eps_sym)
                        set_add(first[A], &first_cnt[A], first[B][k]);
                if (!set_has(first[B], first_cnt[B], eps_sym)) { all_eps = 0; break; }
            }
            if (all_eps) set_add(first[A], &first_cnt[A], eps_sym);
            if (first_cnt[A] != old) changed = 1;
        }
    }
}

void compute_follow() {
    set_add(follow[start_sym], &follow_cnt[start_sym], end_sym);
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int p = 0; p < prod_cnt; p++) {
            int A = prods[p].lhs;
            for (int i = 0; i < prods[p].len; i++) {
                int B = prods[p].rhs[i];
                if (IS_TERM(B)) continue;
                int old = follow_cnt[B];

                int blen = prods[p].len - i - 1;
                int fb[MAX_SYMS], fbc = 0;
                first_of_string(&prods[p].rhs[i+1], blen, fb, &fbc);

                for (int k = 0; k < fbc; k++)
                    if (fb[k] != eps_sym)
                        set_add(follow[B], &follow_cnt[B], fb[k]);

                if (set_has(fb, fbc, eps_sym) || blen == 0)
                    for (int k = 0; k < follow_cnt[A]; k++)
                        set_add(follow[B], &follow_cnt[B], follow[A][k]);

                if (follow_cnt[B] != old) changed = 1;
            }
        }
    }
}

/* ── LR(0) Items & States ───────────────────────────────────────────── */
typedef struct { int prod; int dot; } Item;

typedef struct {
    Item items[MAX_ITEMS]; int cnt;
    int  next[MAX_SYMS];          /* transitions */
} State;

State states[MAX_STATES];
int   state_cnt = 0;

static void state_add(State *st, Item it) {
    for (int i = 0; i < st->cnt; i++)
        if (st->items[i].prod == it.prod && st->items[i].dot == it.dot) return;
    st->items[st->cnt++] = it;
}

void closure(State *st) {
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < st->cnt; i++) {
            Item it = st->items[i];
            Prod *pr = &prods[it.prod];
            if (it.dot >= pr->len) continue;
            int B = pr->rhs[it.dot];
            if (IS_TERM(B)) continue;
            for (int p = 0; p < prod_cnt; p++) {
                if (prods[p].lhs != B) continue;
                Item ni = {p, 0};
                int old = st->cnt;
                state_add(st, ni);
                if (st->cnt != old) changed = 1;
            }
        }
    }
}

State goto_fn(State *st, int X) {
    State ns; ns.cnt = 0;
    memset(ns.next, -1, sizeof(ns.next));
    for (int i = 0; i < st->cnt; i++) {
        Item it = st->items[i];
        Prod *pr = &prods[it.prod];
        if (it.dot < pr->len && pr->rhs[it.dot] == X)
            state_add(&ns, (Item){it.prod, it.dot+1});
    }
    closure(&ns);
    return ns;
}

int states_eq(State *a, State *b) {
    if (a->cnt != b->cnt) return 0;
    for (int i = 0; i < a->cnt; i++) {
        int found = 0;
        for (int j = 0; j < b->cnt; j++)
            if (a->items[i].prod == b->items[j].prod &&
                a->items[i].dot  == b->items[j].dot) { found = 1; break; }
        if (!found) return 0;
    }
    return 1;
}

void build_states() {
    State s0; s0.cnt = 0;
    memset(s0.next, -1, sizeof(s0.next));
    state_add(&s0, (Item){0, 0});
    closure(&s0);
    states[state_cnt++] = s0;

    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < state_cnt; i++) {
            for (int X = 0; X < sym_cnt; X++) {
                if (X == eps_sym || states[i].next[X] != -1) continue;
                State ns = goto_fn(&states[i], X);
                if (ns.cnt == 0) continue;
                /* find existing or add */
                int idx = -1;
                for (int j = 0; j < state_cnt; j++)
                    if (states_eq(&states[j], &ns)) { idx = j; break; }
                if (idx == -1) {
                    if (state_cnt >= MAX_STATES) {
                        fprintf(stderr, "Too many states!\n"); exit(1);
                    }
                    states[state_cnt] = ns;
                    idx = state_cnt++;
                }
                states[i].next[X] = idx;
                changed = 1;
            }
        }
    }
}

/* ── ACTION / GOTO Tables ───────────────────────────────────────────── */
int action_tab[MAX_STATES][MAX_SYMS];
int goto_tab  [MAX_STATES][MAX_SYMS];

void build_tables() {
    for (int i = 0; i < MAX_STATES; i++)
        for (int j = 0; j < MAX_SYMS; j++) {
            action_tab[i][j] = ERROR_VAL;
            goto_tab  [i][j] = -1;
        }

    int conflicts = 0;
    for (int i = 0; i < state_cnt; i++) {
        State *st = &states[i];

        /* shifts & goto from transitions */
        for (int X = 0; X < sym_cnt; X++) {
            if (st->next[X] == -1) continue;
            if (IS_TERM(X)) {
                if (action_tab[i][X] != ERROR_VAL) {
                    printf("  [conflict] state %d on '%s'\n", i, SYM(X));
                    conflicts++;
                }
                action_tab[i][X] = st->next[X];  /* positive => shift */
            } else {
                goto_tab[i][X] = st->next[X];
            }
        }

        /* reduce / accept from dot-at-end items */
        for (int k = 0; k < st->cnt; k++) {
            Item it = st->items[k];
            Prod *pr = &prods[it.prod];
            if (it.dot < pr->len) continue;

            if (it.prod == 0) {
                /* augmented production: accept */
                if (action_tab[i][end_sym] != ERROR_VAL) {
                    printf("  [conflict] accept state %d\n", i); conflicts++;
                }
                action_tab[i][end_sym] = ACCEPT_VAL;
            } else {
                /* reduce on FOLLOW(lhs) */
                for (int f = 0; f < follow_cnt[pr->lhs]; f++) {
                    int t = follow[pr->lhs][f];
                    if (action_tab[i][t] != ERROR_VAL) {
                        printf("  [conflict] state %d on '%s'\n", i, SYM(t));
                        conflicts++;
                    }
                    action_tab[i][t] = -(it.prod);  /* negative => reduce */
                }
            }
        }
    }
    if (conflicts == 0) printf("  Grammar is SLR(1) — no conflicts.\n");
    else printf("  %d conflict(s). Grammar may not be SLR(1).\n", conflicts);
}

/* ── Parser ─────────────────────────────────────────────────────────── */
void parse(int *toks, int n) {
    int stk[500]; int sp = 0;
    stk[sp] = 0;

    printf("\n%-32s %-22s %s\n",
           "Stack", "Remaining Input", "Action");
    printf("%-32s %-22s %s\n",
           "--------------------------------",
           "----------------------", "------");

    int pos = 0;
    while (1) {
        /* build stack string */
        char sbuf[120] = "";
        for (int i = 0; i <= sp; i++) {
            char t[8]; sprintf(t, "%d ", stk[i]); strcat(sbuf, t);
        }
        /* build input string */
        char ibuf[120] = "";
        for (int i = pos; i < n; i++) {
            strcat(ibuf, SYM(toks[i]));
            strcat(ibuf, " ");
        }

        int s   = stk[sp];
        int a   = toks[pos];
        int act = action_tab[s][a];

        if (act == ACCEPT_VAL) {
            printf("%-32s %-22s ACCEPT\n", sbuf, ibuf);
            printf("\n  Input accepted!\n");
            return;
        } else if (act == ERROR_VAL) {
            printf("%-32s %-22s ERROR\n", sbuf, ibuf);
            printf("\n  Syntax error at token '%s'\n", SYM(a));
            return;
        } else if (act > 0) {
            /* shift */
            char abuf[32]; sprintf(abuf, "shift s%d", act);
            printf("%-32s %-22s %s\n", sbuf, ibuf, abuf);
            stk[++sp] = act;
            pos++;
        } else {
            /* reduce */
            int r = -act;
            Prod *pr = &prods[r];
            char abuf[80];
            sprintf(abuf, "reduce %s ->", SYM(pr->lhs));
            for (int j = 0; j < pr->len; j++) {
                strcat(abuf, " "); strcat(abuf, SYM(pr->rhs[j]));
            }
            printf("%-32s %-22s %s\n", sbuf, ibuf, abuf);
            sp -= pr->len;
            int go = goto_tab[stk[sp]][pr->lhs];
            if (go == -1) { printf("  GOTO error!\n"); return; }
            stk[++sp] = go;
        }
    }
}

/* ── Grammar Reader ─────────────────────────────────────────────────── */
/*
 * Reads lines of the form:
 *   A -> sym1 sym2 ... | alt1 alt2 ...
 * Uppercase first char => non-terminal; else terminal.
 * 'e' => epsilon.
 */
void read_grammar(FILE *f) {
    char lines[MAX_PRODS][256];
    int  lc = 0;
    char line[256];

    /* Pass 1: register all LHS as non-terminals, remember first */
    int first_nt_seen = 0;
    int orig_start = -1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (!strlen(line)) continue;
        strcpy(lines[lc++], line);
        char tmp[256]; strcpy(tmp, line);
        char *t = strtok(tmp, " \t");
        if (!t) continue;
        int s = add_sym(t, 0);
        if (!first_nt_seen) { orig_start = s; first_nt_seen = 1; }
    }

    /* Special symbols */
    end_sym = add_sym("$", 1);
    eps_sym = add_sym("e", 1);

    /* Augment: S' -> S  as production 0 */
    char aug[24]; sprintf(aug, "%s'", SYM(orig_start));
    int aug_s = add_sym(aug, 0);
    prods[prod_cnt].lhs    = aug_s;
    prods[prod_cnt].rhs[0] = orig_start;
    prods[prod_cnt].len    = 1;
    prod_cnt++;
    start_sym = aug_s;

    /* Pass 2: parse each line into productions */
    for (int i = 0; i < lc; i++) {
        char tmp[256]; strcpy(tmp, lines[i]);
        char *tok = strtok(tmp, " \t");
        if (!tok) continue;
        int lhs = find_sym(tok);     /* already registered */
        strtok(NULL, " \t");         /* skip "->" */

        int rhs[MAX_RHS], rlen = 0;
        while ((tok = strtok(NULL, " \t")) != NULL) {
            if (!strcmp(tok, "|")) {
                prods[prod_cnt].lhs = lhs;
                if (rlen == 1 && rhs[0] == eps_sym) {
                    prods[prod_cnt].len = 0;
                } else {
                    memcpy(prods[prod_cnt].rhs, rhs, rlen * sizeof(int));
                    prods[prod_cnt].len = rlen;
                }
                prod_cnt++;
                rlen = 0;
            } else {
                /* auto-detect terminal/non-terminal */
                int is_t = !isupper((unsigned char)tok[0]);
                int s = add_sym(tok, is_t);
                rhs[rlen++] = s;
            }
        }
        /* last (or only) alternative */
        if (rlen > 0) {
            prods[prod_cnt].lhs = lhs;
            /* If sole symbol is epsilon, treat as empty production */
            if (rlen == 1 && rhs[0] == eps_sym) {
                prods[prod_cnt].len = 0;
            } else {
                memcpy(prods[prod_cnt].rhs, rhs, rlen * sizeof(int));
                prods[prod_cnt].len = rlen;
            }
            prod_cnt++;
        }
    }
}

/* ── Print Helpers ──────────────────────────────────────────────────── */
void print_grammar() {
    printf("\n=== Augmented Grammar ===\n");
    for (int i = 0; i < prod_cnt; i++) {
        printf("  (%d) %s ->", i, SYM(prods[i].lhs));
        for (int j = 0; j < prods[i].len; j++) printf(" %s", SYM(prods[i].rhs[j]));
        printf("\n");
    }
}

void print_first_follow() {
    printf("\n=== FIRST Sets ===\n");
    for (int i = 0; i < sym_cnt; i++) {
        if (IS_TERM(i)) continue;
        printf("  FIRST(%-8s) = { ", SYM(i));
        for (int j = 0; j < first_cnt[i]; j++) printf("%s ", SYM(first[i][j]));
        printf("}\n");
    }
    printf("\n=== FOLLOW Sets ===\n");
    for (int i = 0; i < sym_cnt; i++) {
        if (IS_TERM(i)) continue;
        printf("  FOLLOW(%-8s) = { ", SYM(i));
        for (int j = 0; j < follow_cnt[i]; j++) printf("%s ", SYM(follow[i][j]));
        printf("}\n");
    }
}

void print_states() {
    printf("\n=== LR(0) Canonical Collection (%d states) ===\n", state_cnt);
    for (int i = 0; i < state_cnt; i++) {
        printf("  I%d:\n", i);
        for (int k = 0; k < states[i].cnt; k++) {
            Item  it = states[i].items[k];
            Prod *pr = &prods[it.prod];
            printf("    %s ->", SYM(pr->lhs));
            for (int j = 0; j <= pr->len; j++) {
                if (j == it.dot) printf(" .");
                if (j < pr->len) printf(" %s", SYM(pr->rhs[j]));
            }
            printf("\n");
        }
    }
}

void print_tables() {
    /* Collect terminal/non-terminal lists (exclude eps) */
    int terms[MAX_SYMS], tc = 0;
    int nts  [MAX_SYMS], nc = 0;
    for (int j = 0; j < sym_cnt; j++) {
        if (IS_TERM(j) && j != eps_sym) terms[tc++] = j;
        else if (IS_NT(j))              nts  [nc++] = j;
    }

    printf("\n=== ACTION Table ===\n  %-6s", "St");
    for (int j = 0; j < tc; j++) printf("%-8s", SYM(terms[j]));
    printf("\n");
    for (int i = 0; i < state_cnt; i++) {
        printf("  %-6d", i);
        for (int j = 0; j < tc; j++) {
            int a = action_tab[i][terms[j]];
            char b[10] = "";
            if      (a == ACCEPT_VAL) strcpy(b, "acc");
            else if (a == ERROR_VAL ) strcpy(b, "");
            else if (a > 0)           sprintf(b, "s%d", a);
            else                      sprintf(b, "r%d", -a);
            printf("%-8s", b);
        }
        printf("\n");
    }

    printf("\n=== GOTO Table ===\n  %-6s", "St");
    for (int j = 0; j < nc; j++) printf("%-8s", SYM(nts[j]));
    printf("\n");
    for (int i = 0; i < state_cnt; i++) {
        printf("  %-6d", i);
        for (int j = 0; j < nc; j++) {
            int g = goto_tab[i][nts[j]];
            char b[10] = "";
            if (g != -1) sprintf(b, "%d", g);
            printf("%-8s", b);
        }
        printf("\n");
    }
}

/* ── main ───────────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    printf("╔═══════════════════════════════════╗\n");
    printf("║  SLR(1) Parser  —  Dynamic        ║\n");
    printf("╚═══════════════════════════════════╝\n");

    FILE *gf;
    if (argc >= 2) {
        gf = fopen(argv[1], "r");
        if (!gf) { perror(argv[1]); return 1; }
        printf("\nReading grammar from: %s\n", argv[1]);
    } else {
        printf("\nEnter grammar (blank line when done):\n");
        printf("  Uppercase = non-terminal, lowercase/symbol = terminal\n");
        printf("  'e' = epsilon, '|' = alternative\n\n");
        gf = stdin;
    }

    read_grammar(gf);
    if (gf != stdin) fclose(gf);

    print_grammar();
    compute_first();
    compute_follow();
    print_first_follow();
    build_states();
    print_states();

    printf("\n=== Building SLR(1) Tables ===\n");
    build_tables();
    print_tables();

    /* Interactive parsing */
    printf("\n─────────────────────────────────────\n");
    printf("Enter tokens space-separated (end with $).\nType 'quit' to exit.\n\n");

    char line[256];
    while (1) {
        printf("Input> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "quit")) break;
        if (!strlen(line)) continue;

        int toks[200], tc = 0;
        char tmp[256]; strcpy(tmp, line);
        char *tok = strtok(tmp, " \t");
        int ok = 1;
        while (tok) {
            int s = find_sym(tok);
            if (s < 0) { printf("Unknown symbol '%s'\n", tok); ok = 0; break; }
            toks[tc++] = s;
            tok = strtok(NULL, " \t");
        }
        if (!ok) continue;
        if (tc == 0 || toks[tc-1] != end_sym) toks[tc++] = end_sym;
        parse(toks, tc);
    }
    return 0;
}
