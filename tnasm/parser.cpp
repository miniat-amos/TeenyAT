#include "../token.h"
#include "parser.h"

using namespace std;

Token_Line parse_line;  // The current line of tokens being parsed
int i next;  // index of the next token in the line to consider

bool p_variable_line();
bool p_constant_line();
bool p_raw_line();
bool p_label_line();

bool p_immed();

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
