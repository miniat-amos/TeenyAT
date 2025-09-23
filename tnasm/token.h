#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <regex>
#include <vector>

using namespace std;

enum token_type {
    T_IGNORE = 0,
    T_EOL,
    T_LABEL,
    T_CONSTANT,
    T_VARIABLE,
    T_RAW,
    T_REGISTER,
    T_SET,
    T_LOD,
    T_STR,
    T_PSH,
    T_POP,
    T_BTS,
    T_BTC,
    T_BTF,
    T_CAL,
    T_ADD,
    T_SUB,
    T_MPY,
    T_DIV,
    T_MOD,
    T_AND,
    T_OR,
    T_XOR,
    T_SHF,
    T_SHL,
    T_SHR,
    T_ROT,
    T_ROL,
    T_ROR,
    T_NEG,
    T_CMP,
    T_DLY,
    T_INT,
    T_RTI,
    T_JMP,
    T_JE,
    T_JNE,
    T_JL,
    T_JLE,
    T_JG,
    T_JGE,
    T_LUP,
    T_INC,
    T_DEC,
    T_INV,
    T_RET,
    T_IDENTIFIER,
    T_NUMBER,
    T_PLUS,
    T_MINUS,
    T_CHARACTER,
    T_STRING,
    T_PACKED_STRING,
    T_COMMA,
    T_LBRACKET,
    T_RBRACKET,
    T_BAD
};

inline string tstr[] = {
    "T_IGNORE",
    "T_EOL",
    "T_LABEL",
    "T_CONSTANT",
    "T_VARIABLE",
    "T_RAW",
    "T_REGISTER",
    "T_SET",
    "T_LOD",
    "T_STR",
    "T_PSH",
    "T_POP",
    "T_BTS",
    "T_BTC",
    "T_BTF",
    "T_CAL",
    "T_ADD",
    "T_SUB",
    "T_MPY",
    "T_DIV",
    "T_MOD",
    "T_AND",
    "T_OR",
    "T_XOR",
    "T_SHF",
    "T_SHL",
    "T_SHR",
    "T_ROT",
    "T_ROL",
    "T_ROR",
    "T_NEG",
    "T_CMP",
    "T_DLY",
    "T_INT",
    "T_RTI",
    "T_JMP",
    "T_JE",
    "T_JNE",
    "T_JL",
    "T_JLE",
    "T_JG",
    "T_JGE",
    "T_LUP",
    "T_INC",
    "T_DEC",
    "T_INV",
    "T_RET",
    "T_IDENTIFIER",
    "T_NUMBER",
    "T_PLUS",
    "T_MINUS",
    "T_CHARACTER",
    "T_STRING",
    "T_PACKED_STRING",
    "T_COMMA",
    "T_LBRACKET",
    "T_RBRACKET",
    "T_BAD"
};

struct token {
    token_type id;
    string token_str;
    int line_no;
    int ram_addr;
    tny_word value;
};

typedef vector <token> token_line;
typedef vector <token_line> token_lines;

struct symbol {
	int line_no;
	string str;
	tny_word data;
};

#endif /* __TOKEN_H__ */
