%option noyywrap
%option outfile="lexer.cpp"
%option case-insensitive

%{
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
//#include <cstdio>

#include "token.h"

using namespace std;

#define T_SET	0
#define T_LOD	0
#define T_STR	0
#define T_PSH	0
#define T_POP	0
#define T_BTS	0
#define T_BTC	0
#define T_BTF	0
#define T_CAL	0
#define T_ADD	0
#define T_SUB	0
#define T_MPY	0
#define T_DIV	0
#define T_MOD	0
#define T_AND	0
#define T_OR	0
#define T_XOR	0
#define T_SHF	0
#define T_ROT	0
#define T_NEG	0
#define T_CMP	0
#define T_JMP	0
#define T_DJZ	0
#define T_INC	0
#define T_DEC	0
#define T_RET	0
#define T_LABEL	0
#define T_VARIABLE	0
#define T_CONSTANT	0
#define T_REGISTER	0
#define T_IDENTIFIER	0
#define T_NUMBER	0
#define T_PLUS	0
#define T_MINUS	0
#define T_COMMA	0
#define T_LBRACKET	0
#define T_RBRACKET	0

Token pack_token(int type);

extern void *ParseAlloc(void *(*mallocProc)(size_t));
extern void Parse(void *yyp, int yymajor, Token *yyminor);
extern void ParseTrace(FILE *TraceFILE, char *zTracePrompt);
extern void ParseFree(void *p, void (*freeProc)(void*));

void *parser;

vector <TokenLine> lines;
TokenLine current_line;
int line_no = 1;
int pass = 1;
tny_uword addr = 0;

vector <Symbol> variables;
vector <Symbol> constants;
vector <Symbol> labels;

Token pack_token(int type) {
	Token t;
	t.line_no = line_no;
	t.token_str = yytext;
	t.type = type;
	t.data.u = 0;
	t.immed.u = 0;
	return t;
}


%}

%%

[ \t\b\v\f\r]+	/* Whitespace */

";".*	/* Line comment */

\n	{ /* EOL */
		if(!current_line.toks.empty())
			lines.push_back(current_line);
		line_no++;
		current_line.toks.clear();
		current_line.line_no = line_no;
	}

"!"[^ \t\b\v\r\n;]+	{ current_line.toks.push_back(pack_token(T_LABEL)); }

".var"	{ current_line.toks.push_back(pack_token(T_VARIABLE)); }
".const"	{ current_line.toks.push_back(pack_token(T_CONSTANT)); }

PC	{ Token t = pack_token(T_REGISTER); t.data.u = TNY_REG_PC; current_line.toks.push_back(t); }
SP	{ Token t = pack_token(T_REGISTER); t.data.u = TNY_REG_SP; current_line.toks.push_back(t); }
rZ	{ Token t = pack_token(T_REGISTER); t.data.u = TNY_REG_ZERO; current_line.toks.push_back(t); }
rA	{ Token t = pack_token(T_REGISTER); t.data.u = TNY_REG_A; current_line.toks.push_back(t); }
rB	{ Token t = pack_token(T_REGISTER); t.data.u = TNY_REG_B; current_line.toks.push_back(t); }
rC	{ Token t = pack_token(T_REGISTER); t.data.u = TNY_REG_C; current_line.toks.push_back(t); }
rD	{ Token t = pack_token(T_REGISTER); t.data.u = TNY_REG_D; current_line.toks.push_back(t); }
rE	{ Token t = pack_token(T_REGISTER); t.data.u = TNY_REG_E; current_line.toks.push_back(t); }

"set"	{ current_line.toks.push_back(pack_token(T_SET)); }
"lod"	{ current_line.toks.push_back(pack_token(T_LOD)); }
"str"	{ current_line.toks.push_back(pack_token(T_STR)); }
"psh"	{ current_line.toks.push_back(pack_token(T_PSH)); }
"pop"	{ current_line.toks.push_back(pack_token(T_POP)); }
"bts"	{ current_line.toks.push_back(pack_token(T_BTS)); }
"btc"	{ current_line.toks.push_back(pack_token(T_BTC)); }
"btf"	{ current_line.toks.push_back(pack_token(T_BTF)); }
"cal"	{ current_line.toks.push_back(pack_token(T_CAL)); }
"add"	{ current_line.toks.push_back(pack_token(T_ADD)); }
"sub"	{ current_line.toks.push_back(pack_token(T_SUB)); }
"mpy"	{ current_line.toks.push_back(pack_token(T_MPY)); }
"div"	{ current_line.toks.push_back(pack_token(T_DIV)); }
"mod"	{ current_line.toks.push_back(pack_token(T_MOD)); }
"and"	{ current_line.toks.push_back(pack_token(T_AND)); }
"or"	{ current_line.toks.push_back(pack_token(T_OR)); }
"xor"	{ current_line.toks.push_back(pack_token(T_XOR)); }
"shf"	{ current_line.toks.push_back(pack_token(T_SHF)); }
"rot"	{ current_line.toks.push_back(pack_token(T_ROT)); }
"neg"	{ current_line.toks.push_back(pack_token(T_NEG)); }
"cmp"	{ current_line.toks.push_back(pack_token(T_CMP)); }
"jmp"	{ current_line.toks.push_back(pack_token(T_JMP)); }
"djz"	{ current_line.toks.push_back(pack_token(T_DJZ)); }

"inc"	{ current_line.toks.push_back(pack_token(T_INC)); }
"dec"	{ current_line.toks.push_back(pack_token(T_DEC)); }
"ret"	{ current_line.toks.push_back(pack_token(T_RET)); }

[a-z][a-z0-9]*	{ current_line.toks.push_back(pack_token(T_IDENTIFIER)); }

[0-9]+	|
0x[0-9a-f]+	{ /* Number (decimal or hex) */
		Token t = pack_token(T_NUMBER);
		t.data.u = tny_uword(strtol(yytext, NULL, 0));
		current_line.toks.push_back(t);
	}

0b[01]+	{ /* Number (binary) */
		Token t = pack_token(T_NUMBER);
		t.data.u = tny_uword(strtol(yytext + 2, NULL, 2));
		current_line.toks.push_back(t);
	}

"+"	{ current_line.toks.push_back(pack_token(T_PLUS)); }
"-"	{ current_line.toks.push_back(pack_token(T_MINUS)); }
","	{ current_line.toks.push_back(pack_token(T_COMMA)); }
"["	{ current_line.toks.push_back(pack_token(T_LBRACKET)); }
"]"	{ current_line.toks.push_back(pack_token(T_RBRACKET)); }

.	{ cerr << "unexpected character \"" << yytext << "\" on line " << line_no << endl; }

%%

int main(int argc, char *argv[]) {
	if(argc != 2) {
		cerr << "Expected one assembly file for processing..." << endl;
		cerr << "usage:   " << argv[0] << " <asm-file-path>" << endl;
		exit(EXIT_FAILURE);
	}

	current_line.line_no = 1;

	yyin = fopen(argv[1], "r");
	string binname(argv[1]);
	binname += ".bin";
	ofstream outfile(binname, ios::binary);

	yylex();

	fclose(yyin);

	outfile.close();

	return EXIT_SUCCESS;
}
