#include <iostream>
#include <memory>
#include <map>

#include "../teenyat.h"
#include "token.h"
#include "parser.h"

using namespace std;

struct instruction {
    tny_word first;
    tny_word second;
    int line_no;
};

token_line parse_line;  // The current line of tokens being parsed
unsigned int tnext;  // index of the next token in the line to consider
int pass;
tny_uword address;

map <string, tny_word> constants;
map <string, tny_word> variables;

map <string, tny_word> labels;
bool labels_updated_this_pass;  

bool parse(token_lines &parse_lines, vector <string> asm_lines);

shared_ptr <token> term(token_type id);
bool p_loc();
shared_ptr <token> p_variable_line();
shared_ptr <token> p_constant_line();
bool p_raw_line();
shared_ptr <token> p_label_line();

shared_ptr <tny_word> p_immediate();
shared_ptr <tny_word> p_number();
shared_ptr <token> p_plus_or_minus();

bool p_code_1_line();
bool p_code_2_line();
bool p_code_3_line();

tny_uword token_to_opcode(int id);

shared_ptr <token> p_code_1_inst();
shared_ptr <token> p_code_2_inst();
shared_ptr <token> p_code_3_inst();
shared_ptr <token> p_code_4_inst();
shared_ptr <token> p_code_5_inst();

vector <tny_word> bin_words;

/**
 * @brief
 *   Parses the token, line by line, to generate the encoded version of the
 *   program.
 *
 * @return
 *   True if entire program is correct.  False otherwise.
 */
bool parse(token_lines &parse_lines, vector <string> asm_lines) {
    bool result = true;  // assume the assembly input is syntactically correct

    /*
     * In pass 1:
     *   - create the variables table and determine the address of each
     *   - create the constants symbol table w/ associated values
     *   - create the labels symbol table and determine the address of each
     *   - report errors for any line of code that doesn't match the p_loc()
     *     grammar
     */
    pass = 1;
    address = 0x0000;

    for(auto &line: parse_lines) {
        int line_no = line[0].line_no;
        parse_line = line;
        if(p_loc() == false) {
            result = false;
            cerr << "Error, line(" << line_no << "): " << asm_lines[line_no - 1] << endl;
        }
    }

    /* If anything went wrong, there's no point in going on to pass 2 */
    if(result == false) {
        return false;
    }

    /*
     * In pass 2:
     *   - generate a vector of encoded instructions, one per line of operable
     *     code, variable memory, and raw data
     */
    pass = 2;
    address = 0x0000;

    /*
     * TODO: Eventually, once the parser auto-detects it can use the teeny
     * form of some instructions, the second pass will have to be replaced by
     * multiple passes that continue until no label addresses are modified.
     */
    for(auto &line: parse_lines) {
        parse_line = line;
        if(p_loc() == false) {
            result = false;
            /* TODO: assumption that failed p_X_line() reported error already */
        }
    }

    if(result) {
        for(auto w : bin_words) {
            cout << hex << w.u << endl;
        }
    }
    else {
        cerr << "There were errors.  No binary output." << endl;
    }

    return result;
}

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
shared_ptr <token> term(token_type id) {
    shared_ptr <token> val = nullptr;
    if(tnext < parse_line.size() && parse_line[tnext].id == id) {
        val = shared_ptr <token>(new token(parse_line[tnext]));
    }
    tnext++;
    return val;
}

bool p_loc() {
    int save = tnext = 0;
    return (tnext = save, p_variable_line()) ||
           (tnext = save, p_constant_line()) ||
           (tnext = save, p_raw_line())      ||
           (tnext = save, p_label_line())    ||
           (tnext = save, p_code_1_line())   ||
           (tnext = save, p_code_2_line())   ||
           (tnext = save, p_code_3_line());
}

/*
 * variable_line ::= VARIABLE IDENTIFIER immediate.
 */
shared_ptr <token> p_variable_line() {
    shared_ptr <token> val = nullptr, ident;
    shared_ptr <tny_word> immed;
    int save = tnext;
    if(term(T_VARIABLE) && (ident = term(T_IDENTIFIER)) && (immed = p_immediate()) && term(T_EOL)) {
        if(pass == 1) {
            bool constant_exists = (constants.count(ident->token_str) > 0);
            bool variable_exists = (variables.count(ident->token_str) > 0);
            if(!constant_exists && !variable_exists) {
                /* New variable found */
                variables[ident->token_str] = tny_word{.u = address};
            }
            else {
                cerr << "ERROR, Line " << ident->line_no << ": ";
                cerr << (constant_exists ? "Constant" : "Variable") << " \""  << ident->token_str << "\" already defined" << endl;
            }
        }
        else if(pass == 2) {
            bin_words.push_back(*immed);
        } // TODO: put the immediate in the binary at this address
        address++;
        val = ident;
    }
    return val;
}

/*
 * constant_line ::= CONSTANT IDENTIFIER immediate.
 */
shared_ptr <token> p_constant_line() {
    shared_ptr <token> val = nullptr, ident;
    shared_ptr <tny_word> immed;
    int save = tnext;
    if(term(T_CONSTANT) && (ident = term(T_IDENTIFIER)) && (immed = p_immediate()) && term(T_EOL)) {
        if(pass == 1) {
            bool constant_exists = (constants.count(ident->token_str) > 0);
            bool variable_exists = (variables.count(ident->token_str) > 0);
            if(!constant_exists && !variable_exists) {
                /* New constant found */
                constants[ident->token_str] = *immed;
            }
            else {
                cerr << "ERROR, Line " << ident->line_no << ": ";
                cerr << (constant_exists ? "Constant" : "Variable") << " \""  << ident->token_str << "\" already defined" << endl;
            }
        }
        val = ident;
    }
    return val;
}

/*
 * raw_line ::= NUMBER+.
 * We'll implement the "one-or-more" NUMBER check manually rather than via the
 * recursive descent approach.
 */
bool p_raw_line() {
    bool all_good = true;
    vector <shared_ptr <tny_word> > data;

    while(all_good) {
        int save = tnext;
        shared_ptr <tny_word> d;

        if((d = p_number())) {
            data.push_back(d);
        }
        else if(tnext = save, (term(T_EOL) != nullptr)) {
            break;
        }
        else {
            all_good = false;
        }
    }

    if(all_good) {
        address += data.size();
        if(pass == 2) {
            /* Add raw data to bin_words */
            for(auto d: data) {
                bin_words.push_back(*d);
            }
        }
    }

    return all_good;
}

/*
 * label_line ::= LABEL.
 */
shared_ptr <token> p_label_line() {
    shared_ptr <token> val = nullptr, label;
    int save = tnext;
    if((label = term(T_LABEL)) && term(T_EOL)) {
        if(pass == 1) {
            bool label_exists = (labels.count(label->token_str) > 0);
            if(!label_exists) {
                /* New label found */
                labels[label->token_str] = tny_word{.u = address};
            }
            else {
                cerr << "ERROR, Line " << label->line_no << ": ";
                cerr << "Constant " << label->token_str << " already defined" << endl;
            }
        }
        else if(pass == 2) {
            if(address != labels[label->token_str].u) {
                cerr << label->token_str << " changed from ";
                cerr << hex << labels[label->token_str].u << " to ";
                cerr << hex << address << " in pass " << dec << pass << endl;

                labels[label->token_str] = tny_word{.u = address};
                labels_updated_this_pass = true;
            }
        }

        val = label;
    }
    return val;
}

/*
 * immediate ::= number.
 * immediate ::= LABEL.
 * immediate ::= IDENTIFIER.
 */
shared_ptr <tny_word> p_immediate() {
    shared_ptr <tny_word> val = nullptr;
    shared_ptr <token> ident, label;
    int save = tnext;

    if(tnext = save, val = p_number()) {
        /* nothing to do */
    }
    else if(tnext = save, label = term(T_LABEL)) {
        /* TODO: look up label's address */
        if(pass == 1) {
            /*
             * Label address is likely unknown and definitely unecessary in this
             * pass, so just recognize the immediate grammar was satisfied
             * with an arbitrary value.
             */
            val = shared_ptr <tny_word>(new tny_word(label->value));
        }
        else if(pass == 2) {
            if(labels.count(label->token_str) > 0) {
                val = shared_ptr <tny_word>(new tny_word(labels[label->token_str]));
            }
            else {
                cerr << "Error, Line(" << label->line_no << "): ";
                cerr << label->token_str << " is not defined" << endl;
            }
        }
    }
    else if(tnext = save, ident = term(T_IDENTIFIER)) {
        /* As an immediate, ensure the identifier is a constant. */
        if(pass == 2) {
            if(constants.count(ident->token_str) > 0) {
                val = shared_ptr <tny_word>(new tny_word(constants[ident->token_str]));
            }
            else if(variables.count(ident->token_str) > 0) {
                val = shared_ptr <tny_word>(new tny_word(variables[ident->token_str]));
            }
            else {
                cerr << "Error, Line(" << ident->line_no << "): ";
                cerr << "\"" << ident->token_str << "\" is not a constant or identifier" << endl;
                val = nullptr;
            }
        }
    }

    return val;
}

/*
 * number ::= IDENTIFIER.
 * number ::= NUMBER.
 * number ::= plus_or_minus NUMBER.
 */
shared_ptr <tny_word> p_number() {
    shared_ptr <tny_word> val = nullptr;
    shared_ptr <token> ident, num, sign;
    int save = tnext;
    if(tnext = save, ident = term(T_IDENTIFIER)) {
        if(pass == 1) {
            /* just return a valid pointer to indicate success */
            val = shared_ptr <tny_word>(new tny_word(ident->value));
        }
        else if(pass == 2) {
            if(constants.count(ident->token_str) > 0) {
                val = shared_ptr <tny_word>(new tny_word(constants[ident->token_str]));
            }
            else if(variables.count(ident->token_str) > 0) {
                val = shared_ptr <tny_word>(new tny_word(variables[ident->token_str]));
            }
            else {
                cerr << "ERROR, Line " << ident->line_no << ": ";
                cerr << "Identifier \""  << ident->token_str << "\" is not defined" << endl;
            }
        }
    }
    else if(tnext = save, num = term(T_NUMBER)) {
        val = shared_ptr <tny_word>(new tny_word(num->value));
    }
    else if(tnext = save, (sign = p_plus_or_minus()) && (num = term(T_NUMBER))) {
        val = shared_ptr <tny_word>(new tny_word(num->value));
        if(sign->id == T_MINUS) {
            val->s *= -1;
        }
    }
    return val;
}

/*
 * plus_or_minus ::= PLUS.
 * plus_or_minus ::= MINUS.
 */
shared_ptr <token> p_plus_or_minus() {
    shared_ptr <token> val = nullptr;
    int save = tnext;

    (tnext = save, val = term(T_PLUS)) ||
    (tnext = save, val = term(T_MINUS));

    return val;
}

/*
 * code_1_line ::= code_1_inst REGISTER COMMA REGISTER.
 */
bool p_code_1_line() {
    bool result = false;
    shared_ptr <token> dreg, oper, sreg, sign;
    shared_ptr <tny_word> immed;
    int save = tnext;
    if((oper = p_code_1_inst()) && (dreg = term(T_REGISTER)) && term(T_COMMA) &&
       (sreg = term(T_REGISTER)) && term(T_EOL)) {

        instruction inst;
        inst.line_no = dreg->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 1;
        f.instruction.reg1 = dreg->value.u;
        f.instruction.reg2 = sreg->value.u;
        f.instruction.immed4 = 0;

        if(pass == 2) {
            bin_words.push_back(f);
        }

        address += 1;
        result = true;
    }

    return result;
}

/*
 * code_2_line ::= code_2_inst REGISTER COMMA REGISTER plus_or_minus immediate.
 */
bool p_code_2_line() {
    bool result = false;
    shared_ptr <token> dreg, oper, sreg, sign;
    shared_ptr <tny_word> immed;
    int save = tnext;
    if((oper = p_code_2_inst()) && (dreg = term(T_REGISTER)) && term(T_COMMA) &&
       (sreg = term(T_REGISTER)) && (sign = p_plus_or_minus()) && (immed = p_immediate()) &&
       term(T_EOL)) {

        instruction inst;
        inst.line_no = dreg->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = dreg->value.u;
        f.instruction.reg2 = sreg->value.u;
        f.instruction.immed4 = 0;

        inst.second.s = immed->s * (sign->id == T_PLUS ? +1 : -1);

        if(pass == 2) {
            bin_words.push_back(f);
            bin_words.push_back(inst.second);
        }

        address += 2;
        result = true;
    }

    return result;
}

/*
 * code_3_line ::= code_3_inst REGISTER COMMA immediate.
 */
bool p_code_3_line() {
    bool result = false;
    shared_ptr <token> dreg, oper;
    shared_ptr <tny_word> immed;
    int save = tnext;
    if((oper = p_code_3_inst()) && (dreg = term(T_REGISTER)) && term(T_COMMA) && (immed = p_immediate()) &&
        term(T_EOL)) {

        instruction inst;
        inst.line_no = dreg->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = dreg->value.u;
        f.instruction.reg2 = TNY_REG_ZERO;
        f.instruction.immed4 = 0;

        inst.second.s = immed->s;

        if(pass == 2) {
            bin_words.push_back(f);
            bin_words.push_back(inst.second);
        }

        address += 2;
        result = true;
    }

    return result;
}

tny_uword token_to_opcode(int id) {
    tny_uword result;
    switch(id) {
    case T_SET:   result = TNY_OPCODE_SET;   break;
    case T_LOD:   result = TNY_OPCODE_LOD;   break;
    case T_STR:   result = TNY_OPCODE_STR;   break;
    case T_PSH:   result = TNY_OPCODE_PSH;   break;
    case T_POP:   result = TNY_OPCODE_POP;   break;
    case T_BTS:   result = TNY_OPCODE_BTS;   break;
    case T_BTC:   result = TNY_OPCODE_BTC;   break;
    case T_BTF:   result = TNY_OPCODE_BTF;   break;
    case T_CAL:   result = TNY_OPCODE_CAL;   break;
    case T_ADD:   result = TNY_OPCODE_ADD;   break;
    case T_SUB:   result = TNY_OPCODE_SUB;   break;
    case T_MPY:   result = TNY_OPCODE_MPY;   break;
    case T_DIV:   result = TNY_OPCODE_DIV;   break;
    case T_MOD:   result = TNY_OPCODE_MOD;   break;
    case T_AND:   result = TNY_OPCODE_AND;   break;
    case T_OR:    result = TNY_OPCODE_OR;    break;
    case T_XOR:   result = TNY_OPCODE_XOR;   break;
    case T_SHF:   result = TNY_OPCODE_SHF;   break;
    case T_ROT:   result = TNY_OPCODE_ROT;   break;
    case T_NEG:   result = TNY_OPCODE_NEG;   break;
    case T_CMP:   result = TNY_OPCODE_CMP;   break;
    case T_JMP:   result = TNY_OPCODE_JMP;   break;
    case T_DJZ:   result = TNY_OPCODE_DJZ;   break;
    /*
     * The following are pseudo-instructions
     */
    case T_INC:   result = TNY_OPCODE_ADD;   break;
    case T_DEC:   result = TNY_OPCODE_SUB;   break;
    case T_RET:   result = TNY_OPCODE_POP;   break;
    default:
        cerr << "Error, line(" << parse_line[0].line_no << "): ";
        cerr << "token_to_opcode() has unknown id, " << id << endl;
        /*
         * TODO: We should cleanly terminiate the assembler here
         */
        result = 1234;
    }

    return result;
}

/*
 * code_1_inst ::= ADD.
 * code_1_inst ::= SUB.
 * code_1_inst ::= MPY.
 * code_1_inst ::= DIV.
 * code_1_inst ::= MOD.
 * code_1_inst ::= AND.
 * code_1_inst ::= OR.
 * code_1_inst ::= XOR.
 * code_1_inst ::= SHF.
 * code_1_inst ::= ROT.
 * code_1_inst ::= SET.
 * code_1_inst ::= LOD.
 * code_1_inst ::= STR.
 * code_1_inst ::= BTF.
 * code_1_inst ::= CMP.
 * code_1_inst ::= DJZ.
 */
shared_ptr <token> p_code_1_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_ADD)) ||
    (tnext = save, result = term(T_SUB)) ||
    (tnext = save, result = term(T_MPY)) ||
    (tnext = save, result = term(T_DIV)) ||
    (tnext = save, result = term(T_MOD)) ||
    (tnext = save, result = term(T_AND)) ||
    (tnext = save, result = term(T_OR))  ||
    (tnext = save, result = term(T_XOR)) ||
    (tnext = save, result = term(T_SHF)) ||
    (tnext = save, result = term(T_ROT)) ||
    (tnext = save, result = term(T_SET)) ||
    (tnext = save, result = term(T_LOD)) ||
    (tnext = save, result = term(T_STR)) ||
    (tnext = save, result = term(T_BTF)) ||
    (tnext = save, result = term(T_CMP)) ||
    (tnext = save, result = term(T_DJZ));

    return result;
}

/*
 * code_2_inst ::= ADD.
 * code_2_inst ::= SUB.
 * code_2_inst ::= MPY.
 * code_2_inst ::= DIV.
 * code_2_inst ::= MOD.
 * code_2_inst ::= AND.
 * code_2_inst ::= OR.
 * code_2_inst ::= XOR.
 * code_2_inst ::= SHF.
 * code_2_inst ::= ROT.
 * code_2_inst ::= SET.
 * code_2_inst ::= LOD.
 * code_2_inst ::= BTF.
 * code_2_inst ::= CMP.
 * code_2_inst ::= DJZ.
 */
shared_ptr <token> p_code_2_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_ADD)) ||
    (tnext = save, result = term(T_SUB)) ||
    (tnext = save, result = term(T_MPY)) ||
    (tnext = save, result = term(T_DIV)) ||
    (tnext = save, result = term(T_MOD)) ||
    (tnext = save, result = term(T_AND)) ||
    (tnext = save, result = term(T_OR))  ||
    (tnext = save, result = term(T_XOR)) ||
    (tnext = save, result = term(T_SHF)) ||
    (tnext = save, result = term(T_ROT)) ||
    (tnext = save, result = term(T_SET)) ||
    (tnext = save, result = term(T_LOD)) ||
    (tnext = save, result = term(T_BTF)) ||
    (tnext = save, result = term(T_CMP)) ||
    (tnext = save, result = term(T_DJZ));

    return result;
}

/*
 * code_3_inst ::= ADD.
 * code_3_inst ::= SUB.
 * code_3_inst ::= MPY.
 * code_3_inst ::= DIV.
 * code_3_inst ::= MOD.
 * code_3_inst ::= AND.
 * code_3_inst ::= OR.
 * code_3_inst ::= XOR.
 * code_3_inst ::= SHF.
 * code_3_inst ::= ROT.
 * code_3_inst ::= SET.
 * code_3_inst ::= LOD.
 * code_3_inst ::= BTF.
 * code_3_inst ::= CMP.
 * code_3_inst ::= DJZ.
 */
shared_ptr <token> p_code_3_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_ADD)) ||
    (tnext = save, result = term(T_SUB)) ||
    (tnext = save, result = term(T_MPY)) ||
    (tnext = save, result = term(T_DIV)) ||
    (tnext = save, result = term(T_MOD)) ||
    (tnext = save, result = term(T_AND)) ||
    (tnext = save, result = term(T_OR))  ||
    (tnext = save, result = term(T_XOR)) ||
    (tnext = save, result = term(T_SHF)) ||
    (tnext = save, result = term(T_ROT)) ||
    (tnext = save, result = term(T_SET)) ||
    (tnext = save, result = term(T_LOD)) ||
    (tnext = save, result = term(T_BTF)) ||
    (tnext = save, result = term(T_CMP)) ||
    (tnext = save, result = term(T_DJZ));

    return result;
}

/*
 * code_4_inst ::= NEG.
 * code_4_inst ::= POP.
 */
shared_ptr <token> p_code_4_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_NEG)) ||
    (tnext = save, result = term(T_POP));

    return result;
}

/*
 * code_5_inst ::= INC.
 * code_5_inst ::= DEC.
 */
shared_ptr <token> p_code_5_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_INC)) ||
    (tnext = save, result = term(T_DEC));

    return result;
}
