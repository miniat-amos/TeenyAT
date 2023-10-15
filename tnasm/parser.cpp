#include <memory>

#include "../teenyat.h"
#include "token.h"
#include "parser.h"

using namespace std;

struct instruction {
    tny_word low;
    tny_word high;
    int line_no;
};

token_line parse_line;  // The current line of tokens being parsed
int tnext;  // index of the next token in the line to consider

bool p_variable_line();
bool p_constant_line();
bool p_raw_line();
unique_ptr<token> p_label_line();

bool p_immed();
bool p_number(tny_word &v);

/**
 * @brief
 *   Verifies the next token in the line is a particular type and moves to the
 *   next
 *
 * @param id
 *   The type of the token to check for
 *
 * @return
 *   A smart pointer to the matching token or nullptr if the type doesn't match
 *
 * @note
 *   It is important to realize the global tnext token index will always be
 *   incremented, regardless of whether the token type matched.
 */
unique_ptr<token> term(token_type id) {
    unique_ptr<token> val = nullptr;
    if(tnext < parse_line.size() && parse_line[tnext].id == id) {
        val = unique_ptr<token>(new token(parse_line[tnext]));
    }
    tnext++;
    return val;
}

bool p_loc() {
    int save = tnext;
    return (tnext = save, p_variable_line()) ||
           (tnext = save, p_constant_line()) ||
           (tnext = save, p_raw_line()) ||
           (tnext = save, p_label_line());
}

bool p_variable_line() {
    return term(T_VARIABLE) && term(T_IDENTIFIER) && p_immed() && term(T_EOL);
}

bool p_constant_line() {
    return term(T_CONSTANT) && term(T_IDENTIFIER) && p_immed() && term(T_EOL);
}

unique_ptr<token> p_label_line() {
    unique_ptr<token> val = nullptr;
    int save = tnext;
    if(term(T_LABEL) && term(T_EOL)) {
        *val = parse_line[save];
    }
    return val;
}

/* number ::= (PLUS | MINUS)? NUMBER. */
bool p_number(tny_word &v) {
    bool result = false;
    int save = tnext;
    if(tnext = save, term(T_PLUS) && term(T_NUMBER)) {
        v = parse_line[save + 1].value;
        result = true;
    }
    else if(tnext = save, term(T_MINUS) && term(T_NUMBER)) {
        v.s = -(parse_line[save + 1].value.s);
        result = true;
    }
    else if(tnext = save, term(T_NUMBER)) {
        v = parse_line[save + 1].value;
        result = true;
    }
    return result;
}
