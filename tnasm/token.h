#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "../teenyat.h"

using namespace std;

typedef struct Token Token;
typedef struct TokenLine TokenLine;
typedef struct Symbol Symbol;

struct Token {
	int line_no;
	string token_str;
	int type;
	/*
	 * These two words could be used for the data of a single lexical token or
	 * as the first and second words of an encoded instruction.
	 */
	tny_word data;
	tny_word immed;
};

struct TokenLine {
	int line_no;
	vector <Token> toks;
};

struct Symbol {
	int line_no;
	string str;
	tny_word data;
};
#endif /* __TOKEN_H__ */
