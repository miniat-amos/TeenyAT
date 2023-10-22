#include <iostream>
#include <memory>
#include <map>

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
int pass = 1;

map<string, tny_word> constants;

unique_ptr<token> p_variable_line();
unique_ptr<token> p_constant_line();
bool p_raw_line();
unique_ptr<token> p_label_line();

unique_ptr<tny_word> p_immediate();
unique_ptr<tny_word> p_number();
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
    int save = tnext = 0;
    return (tnext = save, p_variable_line()) ||
           (tnext = save, p_constant_line()) ||
           (tnext = save, p_raw_line()) ||
           (tnext = save, p_label_line());
}

/*
 * variable_line ::= VARIABLE IDENTIFIER immediate.
 */
unique_ptr<token> p_variable_line() {
    unique_ptr<token> val = nullptr, A;
    unique_ptr<tny_word> B;
    int save = tnext;
    if(term(T_VARIABLE) && (A = term(T_IDENTIFIER)) && (B = p_immediate()) && term(T_EOL)) {
        val = move(A);

        /* TODO: create the variable and map the immediate */
    }
    return val;
}

/*
 * constant_line ::= CONSTANT IDENTIFIER immediate.
 */
unique_ptr<token> p_constant_line() {
    unique_ptr<token> val = nullptr, A;
    unique_ptr<tny_word> B;
    int save = tnext;
    if(term(T_CONSTANT) && (A = term(T_IDENTIFIER)) && (B = p_immediate()) && term(T_EOL)) {
        val = move(A);

        /* TODO: create the constanst and map the immediate */
        if(pass == 1) {
            if(constants.count(A->token_str) == 0) {
                /* New constant found */
                constants[A->token_str] = *B;
            }
            else {
                cerr << "ERROR, Line " << A->line_no << ": ";
                cerr << "Constant \"" << A->token_str << "\" already defined" << endl;
            }
        }
    }
    return val;
}

/*
 * label_line ::= LABEL.
 */
unique_ptr<token> p_label_line() {
    unique_ptr<token> val = nullptr, A;
    int save = tnext;
    if((A = term(T_LABEL)) && term(T_EOL)) {
        val = move(A);
    }
    return val;
}

/*
 * immediate ::= number.
 * immediate ::= LABEL.
 * immediate ::= IDENTIFIER.
 */
unique_ptr<tny_word> p_immediate() {
    unique_ptr<tny_word> val = nullptr;
    unique_ptr<token> A;
    int save = tnext;

    if(tnext = save, val = p_number()) {
        /* nothing to do */
    }
    else if(tnext = save, A = term(T_LABEL)) {
        /* TODO: look up label's address */
    }
    else if(tnext = save, A = term(T_IDENTIFIER)) {
        /* As an immediate, ensure the identifier is a constant. */
        if(pass == 2) {
            if(constants.count(A->token_str) > 0) {
                val = unique_ptr<tny_word>(new tny_word(constants[A->token_str]));
            }
            else {
                val = nullptr;
            }
        }
    }

    return val;
}

/*
 * number ::= NUMBER.
 * number ::= plus_or_minus NUMBER.
 */
unique_ptr<tny_word> p_number() {
    unique_ptr<tny_word> val = nullptr;
    unique_ptr<token> A, B;
    int save = tnext;
    if(tnext = save, A = term(T_NUMBER)) {
        val = unique_ptr<tny_word>(new tny_word(A->value));
    }
    else if(tnext = save, (A = p_plus_or_minus()) && (B = term(T_NUMBER))) {
        val = unique_ptr<tny_word>(new tny_word(B->value));
        if(B->id == T_MINUS) {
            val->s *= -1;
        }
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
