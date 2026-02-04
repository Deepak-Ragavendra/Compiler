#include<stdio.h>
#include<ctype.h>
#include<string.h>

#define MAX 5000

int FOR(),BLOCK(),EXPR(),EXPR_(),TERM(),TERM_(),FACTOR(),IF(),STMT_LIST(),STMT();

const char *cursor;
char str[MAX];

void skipspaces() {
	while(*cursor==' ' || *cursor=='\n' || *cursor=='\t') {
		cursor++;
	}
}

int FOR() {
	skipspaces();
	if(*cursor=='f') {
		cursor++;
		if(*cursor=='o') {
			cursor++;
			if(*cursor=='r') {
				cursor++;
				skipspaces();
				if(*cursor=='(') {
					cursor++;
					if(EXPR()) {
						skipspaces();
						if(*cursor==';') {
							cursor++;
							if(EXPR()) {
								skipspaces();
								if(*cursor==';') {
									cursor++;
									if(EXPR()) {
										skipspaces();
										if(*cursor==')') {
											cursor++;
											skipspaces();
											if(BLOCK()) return 1;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

int IF() {
	skipspaces();
	if(*cursor=='i') {
		cursor++;
		if(*cursor=='f') {
			cursor++;
			skipspaces();
			if(*cursor=='(') {
				cursor++;
				if(EXPR()) {
					skipspaces();
					if(*cursor==')') {
						cursor++;
						skipspaces();
						if(BLOCK()) return 1;
					}
				}
			}
		}
	}
	return 0;
}

int BLOCK() {
	skipspaces();
	if(*cursor=='{') {
		cursor++;
		STMT_LIST();
		skipspaces();
		if(*cursor=='}') {
			cursor++;
			return 1;
		}
		return 0;
	}
	return 0;
}

int STMT() {
	skipspaces();
	if(FOR()) return 1;
	if(IF()) return 1;
	if(BLOCK()) return 1;
	if(EXPR()) {
		skipspaces();
		if(*cursor==';') {
			cursor++;
			return 1;
		}
		return 0;
	}
	return 0;
}

int STMT_LIST() {
	skipspaces();
	while(STMT()) {
		skipspaces();
	}
	return 1;
}

int EXPR() {
	if(!TERM()) return 0;
	return EXPR_();
}

int EXPR_() {
	skipspaces();
	if(*cursor=='+' || *cursor=='-') {
		cursor++;
		if(!TERM()) return 0;
		return EXPR_();
	}
	return 1;
}

int TERM() {
	skipspaces();
	if(FACTOR()) {
		if(TERM_()) return 1;
		else return 0;
	} else return 0;
}

int TERM_() {
	skipspaces();
	if(*cursor=='*') {
		cursor++;
		if(FACTOR()) {
			if(TERM_()) return 1;
			else return 0;
		} else return 0;
	}
	else if(*cursor=='/') {
		cursor++;
		if(FACTOR()) {
			if(TERM_()) return 1;
			else return 0;
		} else return 0;
	}
	else return 1;
}

int FACTOR() {
	skipspaces();
	if(isdigit(*cursor) || isalpha(*cursor)) {
		cursor++;
		return 1;
	}
	else if(*cursor=='(') {
		cursor++;
		if(EXPR()) {
			skipspaces();
			if(*cursor==')') {
				return 1;
			} else return 0;
		} else return 0;
	}
	else {
		return 0;
	}
}

int main() {
	int len=0;
	int open=0,close=0;
	char line[500];

	while(1) {
		if(!fgets(line,sizeof(line),stdin)) break;
		int linelen=strlen(line);
		if(len+linelen>=MAX-1) break;
		strcpy(str+len,line);
		len+=linelen;

		for(int i=0; i<linelen; i++) {
			if(line[i]=='{') open++;
			else if(line[i]=='}') close++;
		}

		if(open>0 && open==close) break;
	}
	str[len]='\0';
	cursor=str;

	if(STMT()) {
		skipspaces();
		if(*cursor=='\0') {
			printf("Valid Syntax\n");
		}
	}
	else {
		printf("Invalid Syntax\n");
	}
}