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

unique_ptr<token> p_variable_line();
unique_ptr<token> p_constant_line();
bool p_raw_line();
unique_ptr<token> p_label_line();

bool p_immed();
bool p_number(tny_word &v);
unique_ptr<token> p_plus_or_minus();

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

unique_ptr<token> p_variable_line() {
    unique_ptr<token> val = nullptr;
    int save = tnext;
    if(term(T_VARIABLE) && term(T_IDENTIFIER) && p_immed() && term(T_EOL)) {
        val = unique_ptr<token>(new token(parse_line[save + 1]));

        /* TODO: create the variable and map the immediate */
    }
    return val;
}

unique_ptr<token> p_constant_line() {
    unique_ptr<token> val = nullptr;
    int save = tnext;
    if(term(T_CONSTANT) && term(T_IDENTIFIER) && p_immed() && term(T_EOL)) {
        val = unique_ptr<token>(new token(parse_line[save + 1]));

        /* TODO: create the constanst and map the immediate */
    }
    return val;
}

unique_ptr<token> p_label_line() {
    unique_ptr<token> val = nullptr;
    int save = tnext;
    if(term(T_LABEL) && term(T_EOL)) {
        *val = parse_line[save];
    }
    return val;
}

/*
 * number ::= NUMBER.
 * number ::= plus_or_minus NUMBER.
 */
unique_ptr<tny_word> p_number() {
    unique_ptr<tny_word> val = nullptr;
    int save = tnext;
    unique_ptr<token> A, B;
    if(tnext = save, term(T_NUMBER)) {
        val = unique_ptr<tny_word>(new tny_word);
        *val = parse_line[save].value;
    }
    else if(tnext = save, A = p_plus_or_minus() && B = term(T_NUMBER)) {
        val = unique_ptr<tny_word>(new tny_word);
        val->s = A->value.s * (B->id == T_PLUS ? 1 : -1);
    }
    return val;
}

/*
 * plus_or_minus ::= PLUS.
 * plus_or_minus ::= MINUS.
 */
unique_ptr<token> p_plus_or_minus() {
    unique_ptr<token> val = nullptr;
    int save = tnext;

    (tnext = save, val = term(T_PLUS)) ||
    (tnext = save, val = term(T_MINUS));

    return val;
}
