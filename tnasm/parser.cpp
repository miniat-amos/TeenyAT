#include "../token.h"
#include "parser.h"

using namespace std;

struct instruction {
    tny_word low;
    tny_word high;
    int line_no;
}

Token_Line parse_line;  // The current line of tokens being parsed
int i next;  // index of the next token in the line to consider

bool p_variable_line();
bool p_constant_line();
bool p_raw_line();
bool p_label_line();

bool p_immed();
bool p_number(tny_word &v);

bool term(Token_Type id) {
    return next < parse_line.size() && parse_line[next++].id == id;
}

bool p_loc() {
    int save = next;
    return (next = save, p_variable_line()) ||
           (next = save, p_constant_line()) ||
           (next = save, p_raw_line()) ||
           (next = save, p_label_line());
}

bool p_variable_line() {
    return term(T_VARIABLE) && term(T_IDENTIFIER) && p_immed() && term(T_EOL);
}

bool p_constant_line() {
    return term(T_CONSTANT) && term(T_IDENTIFIER) && p_immed() && term(T_EOL);
}

bool p_label_line() {
    return term(T_LABEL) && term(T_EOL);
}

/* number ::= (PLUS | MINUS)? NUMBER. */
bool p_number(tny_word &v) {
    bool result = false;
    int save = next;
    if(next = save, term(T_PLUS) && term(T_NUMBER)) {
        v = parse_line[save + 1].value;
        result = true;
    }
    else if(next = save, term(T_MINUS)) && term(T_NUMBER)) {
        v.s = -(parse_line[save + 1].value.s);
        result = true;
    }
    else if(next = save, term(T_NUMBER)) {
        v = parse_line[save + 1].value;
        result = true;
    }
    return result;
}
