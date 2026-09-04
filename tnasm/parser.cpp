#include <iostream>
#include <iomanip>
#include <memory>
#include <map>

#include "../teenyat.h"
#include "token.h"
#include "parser.h"
#include "listing.h"
#include "warnings.h"

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

map <string, identifier_data> constants;
map <string, identifier_data> variables;

map <string, tny_word> labels;
bool labels_updated_this_pass;

bool parse(token_lines &parse_lines, vector <string> asm_lines);

shared_ptr <token> term(token_type id);
bool p_loc();
bool p_variable_line();
shared_ptr <token> p_variable_line_empty();
shared_ptr <token> p_variable_line_immediate();
shared_ptr <token> p_constant_line();
bool p_raw_line();
shared_ptr <token> p_label_line();
shared_ptr <token> p_string();
shared_ptr <tny_word> p_raw_value();

shared_ptr <tny_word> p_immediate();
shared_ptr <tny_word> p_number();
shared_ptr <token> p_plus_or_minus();

bool is_teeny(tny_sword n);

bool p_code_1_line();
bool p_code_2_line();
bool p_code_3_line();
bool p_code_4_line();
bool p_code_5_line();
bool p_code_6_line();
bool p_code_7_line();
bool p_code_8_line();
bool p_code_9_line();
bool p_code_10_line();
bool p_code_11_line();
bool p_code_12_line();
bool p_code_13_line();
bool p_code_14_line();
bool p_code_15_line();
bool p_code_16_line();
bool p_code_17_line();

tny_uword token_to_opcode(int id);

shared_ptr <token> p_code_1_inst();
shared_ptr <token> p_code_1_mem_inst();
shared_ptr <token> p_code_2_inst();
shared_ptr <token> p_code_2_mem_inst();
shared_ptr <token> p_code_3_inst();
shared_ptr <token> p_code_3_mem_inst();
shared_ptr <token> p_code_4_inst();
shared_ptr <token> p_code_5_inst();
shared_ptr <token> p_code_6_inst();
shared_ptr <token> p_code_7_inst();
shared_ptr <token> p_code_8_inst();
shared_ptr <token> p_code_9_inst();
shared_ptr <token> p_code_10_inst();
shared_ptr <token> p_code_11_inst();
shared_ptr <token> p_code_12_inst();
shared_ptr <token> p_code_13_inst();
shared_ptr <token> p_code_14_inst();
shared_ptr <token> p_code_15_inst();
shared_ptr <token> p_code_16_inst();
shared_ptr <token> p_code_17_inst();

extern vector <tny_word> bin_words;

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
            cerr << "ERROR, line " << line_no << ": " << asm_lines[line_no - 1] << endl;
        }
    }

    /* If anything went wrong, there's no point in going on to pass 2 */
    if(result == false) {
        return false;
    }

    /*
     * In passes 2+:
     *   - generate encoded instructions, and add them to bin_words
     *   - generate listing data
     */
    labels_updated_this_pass = true;

    while(labels_updated_this_pass && (result == true)) {
        labels_updated_this_pass = false;
        pass++;
        address = 0x0000;
        bin_words.clear();
        listing_blocks.clear();

        /*
         * TODO: the parser should auto-detect if it can use the teeny
         * form of some instructions
         */
        for(auto &line: parse_lines) {
            parse_line = line;
            listing_block lb;
            lb.line_no = line[0].line_no;
            lb.asm_line = asm_lines[lb.line_no - 1];
            lb.address = address;
            if(p_loc() == false) {
                result = false;
                /* TODO: assumption that failed p_X_line() reported error already */
            }
            int words_used = address - lb.address;
            if(words_used > 0) {
                lb.uses_words = true;
                auto word = bin_words.end() - words_used;
                while(word != bin_words.end()) {
                    lb.bin_uwords.push_back((*word).u);
                    word++;
                }
            }
            listing_blocks.push_back(lb);
        }
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
           (tnext = save, p_code_3_line())   ||
           (tnext = save, p_code_4_line())   ||
           (tnext = save, p_code_5_line())   ||
           (tnext = save, p_code_6_line())   ||
           (tnext = save, p_code_7_line())   ||
           (tnext = save, p_code_8_line())   ||
           (tnext = save, p_code_9_line())   ||
           (tnext = save, p_code_10_line())  ||
           (tnext = save, p_code_11_line())  ||
           (tnext = save, p_code_12_line())  ||
           (tnext = save, p_code_13_line())  ||
           (tnext = save, p_code_14_line())  ||
           (tnext = save, p_code_15_line())  ||
           (tnext = save, p_code_16_line())  ||
           (tnext = save, p_code_17_line());
}

bool p_variable_line() {
    int save = tnext;
    return (tnext = save, p_variable_line_empty())     ||
           (tnext = save, p_variable_line_immediate());
}

bool new_identifier(shared_ptr <token> ident, tny_word val, bool is_constant) {
    bool is_new_identifier = false;
    bool constant_exists = (constants.count(ident->token_str) > 0);
    bool variable_exists = (variables.count(ident->token_str) > 0);
    if(!constant_exists && !variable_exists) {
        /* New identifier found */
        is_new_identifier = true;
        identifier_data tmp;
        tmp.addr.u = address;
        tmp.val = val;
        tmp.line_no = ident->line_no;
        if(is_constant) {
            constants[ident->token_str] = tmp;
        }
        else {
            variables[ident->token_str] = tmp;
        }
    }
    else {
        cerr << "ERROR, line " << ident->line_no << ": ";
        cerr << (constant_exists ? "Constant" : "Variable") << " \""
             << ident->token_str << "\" already defined on line ";
        if(constant_exists) {
            cerr << constants[ident->token_str].line_no << endl;
        }
        else {
            cerr << variables[ident->token_str].line_no << endl;
        }
    }

    return is_new_identifier;
}

bool new_constant(shared_ptr <token> ident, tny_word val) {
    return new_identifier(ident, val, true);
}

bool new_variable(shared_ptr <token> ident, tny_word val) {
    return new_identifier(ident, val, false);
}

/*
 * variable_line ::= VARIABLE IDENTIFIER.
 */
shared_ptr <token> p_variable_line_empty() {
    shared_ptr <token> val = nullptr, ident;
    if(term(T_VARIABLE) && (ident = term(T_IDENTIFIER)) && term(T_EOL)) {
        val = ident;
        if(pass == 1) {
            if(!new_variable(ident, tny_word{.u = 0})) {
                val = nullptr;
            }
            
            if(address == 0x0000) {
                new_warning(ident->line_no, "A variable at address 0x0000 is unsafe.  It will be executed as code.");
            }
        }
        else if(pass > 1) {
            bin_words.push_back({.u = 0});
        }
        address += 1;
    }
    return val;
}

/*
 * variable_line ::= VARIABLE IDENTIFIER immediate.
 */
shared_ptr <token> p_variable_line_immediate() {
    shared_ptr <token> val = nullptr, ident;
    shared_ptr <tny_word> immed;
    if(term(T_VARIABLE) && (ident = term(T_IDENTIFIER)) && (immed = p_immediate()) && term(T_EOL)) {
        val = ident;
        if(pass == 1) {
            if(!new_variable(ident, *immed)) {
                val = nullptr;
            }
            
            if(address == 0x0000) {
                new_warning(ident->line_no, "A variable at address 0x0000 is unsafe.  It will be executed as code.");
            }
        }
        else if(pass > 1) {
            bin_words.push_back(*immed);
        }
        address += 1;
    }
    return val;
}

/*
 * constant_line ::= CONSTANT IDENTIFIER immediate.
 */
shared_ptr <token> p_constant_line() {
    shared_ptr <token> val = nullptr, ident;
    shared_ptr <tny_word> immed;
    if(term(T_CONSTANT) && (ident = term(T_IDENTIFIER)) && (immed = p_immediate()) && term(T_EOL)) {
        val = ident;
        if(pass == 1) {
            if(!new_constant(ident, *immed)) {
                val = nullptr;
            }
        }
    }
    return val;
}

/*
 * raw_line ::= RAW (immediate | string)+.
 * We'll implement the "one-or-more" NUMBER check manually rather than via the
 * recursive descent approach.
 */
bool p_raw_line() {
    vector <shared_ptr <tny_word> > data;
    bool all_good = true;

    if(term(T_RAW) == nullptr) {
        all_good = false;
    }

    while(all_good) {
        int save = tnext;
        shared_ptr <tny_word> d;
        shared_ptr <token> val = nullptr;
        if((d = p_immediate())) {
            data.push_back(d);
        }
        else if((tnext = save, (val = term(T_STRING)))) {
            /* Copy the string's characters, excluding bounding quotes */
            tny_word tmp;
            for(size_t i = 1; i < val->token_str.length() - 1; i++) {
                tmp.u = val->token_str[i];
                shared_ptr <tny_word> v = shared_ptr<tny_word>(new tny_word(tmp));
                data.push_back(v);
            }
            /* null-terminate the string */
            tmp.u = 0;
            shared_ptr <tny_word> v = shared_ptr<tny_word>(new tny_word(tmp));
            data.push_back(v);
        }
        else if((tnext = save, (val = term(T_PACKED_STRING)))) {
            /* Copy the string's characters, excluding bounding quotes */
            size_t i = 1;
            size_t last_char_pos = val->token_str.length() - 2;
            tny_word tmp;
            bool filled_word_last_time = false;
            while(i <= last_char_pos) {
                filled_word_last_time = false;

                /* first character for this word */
                tmp.u = val->token_str[i];
                shared_ptr <tny_word> v = shared_ptr<tny_word>(new tny_word(tmp));
                i++;

                /* second character, if needed */
                if(i <= last_char_pos) {
                    filled_word_last_time = true;
                    tmp.u = val->token_str[i];
                    tmp.u <<= 8;  /* Shift this char to the high byte */
                    v->u |= tmp.u; 
                    i++;                
                }

                data.push_back(v);
            }

            if(filled_word_last_time) {
                /* null-terminate the string because last word is all data */
                tmp.u = 0;
                shared_ptr <tny_word> v = shared_ptr<tny_word>(new tny_word(tmp));
                data.push_back(v);
            }
        }
        else if((tnext = save, (term(T_EOL) != nullptr))) {
            break;
        }
        else {
            all_good = false;
        }
    }

    if(all_good) {
        address += data.size();
        if(pass > 1) {
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
    if((label = term(T_LABEL)) && term(T_EOL)) {
        if(pass == 1) {
            bool label_exists = (labels.count(label->token_str) > 0);
            if(!label_exists) {
                /* New label found */
                labels[label->token_str] = tny_word{.u = address};
                val = label;
            }
            else {
                cerr << "ERROR, line " << label->line_no << ": ";
                cerr << "Label " << label->token_str << " already defined at 0x";
                cerr << hex << setw(4) << setfill('0');
                cerr << labels[label->token_str].u;
                cerr << dec << setw(0) << endl;
            }
        }
        else if(pass > 1) {
            if(address != labels[label->token_str].u) {
                labels[label->token_str] = tny_word{.u = address};
                labels_updated_this_pass = true;
            }
    
            val = label;
        }
    }
    return val;
}

/*
 * raw_value ::= number.
 * raw_value ::= CHARACTER.
 */
shared_ptr<tny_word> p_raw_value() {
    shared_ptr<tny_word> val = nullptr;
    shared_ptr<token> num, c;
    int save = tnext;
    if ((tnext = save, c = term(T_CHARACTER)))
    {
        val = shared_ptr<tny_word>(new tny_word(c->value));
    }
    else if ((tnext = save, val = p_number()))
    {
        /* nothing to do */
    }

    return val;
}

/*
 * immediate ::= raw_value.
 * immediate ::= LABEL.
 * immediate ::= IDENTIFIER.
 */
shared_ptr <tny_word> p_immediate() {
    shared_ptr <tny_word> val = nullptr;
    shared_ptr <token> ident, label;
    int save = tnext;

    if((tnext = save, label = term(T_LABEL))) {
        /* TODO: look up label's address */
        if(pass == 1) {
            /*
             * Label address is likely unknown and definitely unecessary in this
             * pass, so just recognize the immediate grammar was satisfied
             * with an arbitrary value.
             */
            val = shared_ptr <tny_word>(new tny_word(label->value));
        }
        else if(pass > 1) {
            if(labels.count(label->token_str) > 0) {
                val = shared_ptr <tny_word>(new tny_word(labels[label->token_str]));
            }
            else {
                cerr << "ERROR, line " << label->line_no << ": ";
                cerr << label->token_str << " is not defined" << endl;
            }
        }
    }
    else if((tnext = save, ident = term(T_IDENTIFIER))) {
        /* As an immediate, ensure the identifier is a constant. */
        if(pass > 1) {
            if(constants.count(ident->token_str) > 0) {
                val = shared_ptr <tny_word>(new tny_word(constants[ident->token_str].val));
            }
            else if(variables.count(ident->token_str) > 0) {
                val = shared_ptr <tny_word>(new tny_word(variables[ident->token_str].addr));
            }
            else {
                cerr << "ERROR, line " << ident->line_no << ": ";
                cerr << "\"" << ident->token_str << "\" is not a constant or identifier" << endl;
                val = nullptr;
            }
        }
        else {
            /* 
             * First pass still needs to ensure val is valid, since the syntax 
             * is valid, but the constant or variable may not yet have been
             * defined.  We'll catch the undefined ones on pass > 1.
             * 
             * We'll just sen back a pointer to a zero.
             */
            val = shared_ptr <tny_word>(new tny_word{.u = 0x0});
        }
    }
    else if((tnext = save, val = p_raw_value())) {
        /* nothing to do */
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
    if((tnext = save, ident = term(T_IDENTIFIER))) {
        if(pass == 1) {
            /* just return a valid pointer to indicate success */
            val = shared_ptr <tny_word>(new tny_word(ident->value));
        }
        else if(pass > 1) {
            if(constants.count(ident->token_str) > 0) {
                val = shared_ptr <tny_word>(new tny_word(constants[ident->token_str].val));
            }
            else if(variables.count(ident->token_str) > 0) {
                val = shared_ptr <tny_word>(new tny_word(variables[ident->token_str].addr));
            }
            else {
                cerr << "ERROR, line " << ident->line_no << ": ";
                cerr << "Identifier \""  << ident->token_str << "\" is not defined" << endl;
            }
        }
    }
    else if((tnext = save, num = term(T_NUMBER))) {
        val = shared_ptr <tny_word>(new tny_word(num->value));
    }
    else if((tnext = save, (sign = p_plus_or_minus()) && (num = term(T_NUMBER)))) {
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

typedef struct reg_and_immed reg_and_immed;

struct reg_and_immed {
    tny_uword reg;
    tny_word immed;
};

/*
 * register_and_immedite ::= REGISTER plus_or_minus immediate.
 * register_and_immedite ::= immediate PLUS REGISTER.
 */
shared_ptr <reg_and_immed> p_register_and_immediate() {
    shared_ptr <reg_and_immed> result = nullptr;
    shared_ptr <token> reg, sign = nullptr;
    shared_ptr <tny_word> immed;
    int save = tnext;
    if(
        (tnext = save, 
         (reg = term(T_REGISTER)) && (sign = p_plus_or_minus()) &&
         (immed = p_immediate()))
        ||
        (tnext = save,
         (immed = p_immediate()) && term(T_PLUS) && (reg = term(T_REGISTER)))
    ) {
        // OK... what do I do???
        result = make_shared<reg_and_immed>();
        result->immed = *immed;
        if(sign && sign->id == T_MINUS) {
            result->immed.s *= -1;
        }
        result->reg = reg->value.u;
    }

    return result;
}

bool is_teeny(tny_sword n) {
    tny_word small;
    small.instruction.immed4 = n;

    return small.instruction.immed4 == n;
}

/*
 * code_1_line ::= code_1_inst REGISTER COMMA REGISTER.
 * code_1_line ::= code_1_mem_inst REGISTER COMMA LBRACKET REGISTER RBRACKET.
 */
bool p_code_1_line() {
    bool result = false;
    shared_ptr <token> dreg, oper, sreg;
    int save = tnext;
    if(
        (tnext = save,
         (oper = p_code_1_inst()) && (dreg = term(T_REGISTER)) && term(T_COMMA) &&
         (sreg = term(T_REGISTER)) && term(T_EOL))
        ||
        (tnext = save,
         (oper = p_code_1_mem_inst()) && (dreg = term(T_REGISTER)) && term(T_COMMA) &&
         term(T_LBRACKET) && (sreg = term(T_REGISTER)) && term(T_RBRACKET) &&
         term(T_EOL))
    ) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 1;
        f.instruction.reg1 = dreg->value.u;
        f.instruction.reg2 = sreg->value.u;
        f.instruction.immed4 = 0;

        address += 1;

        if(pass > 1) {
            bin_words.push_back(f);
        }

        result = true;
    }

    return result;
}

/*
 * code_2_line ::= code_2_inst REGISTER COMMA register_and_immediate.
 * code_2_line ::= code_2_mem_inst REGISTER COMMA LBRACKET register_and_immediate RBRACKET.
 */
bool p_code_2_line() {
    bool result = false;
    shared_ptr <token> dreg, oper;
    shared_ptr <reg_and_immed>reg_immed = nullptr;
    int save = tnext;
    if(
        (tnext = save,
         (oper = p_code_2_inst()) && (dreg = term(T_REGISTER)) && term(T_COMMA) &&
         (reg_immed = p_register_and_immediate()) && term(T_EOL))
         ||
         (tnext = save,
          (oper = p_code_2_mem_inst()) && (dreg = term(T_REGISTER)) && term(T_COMMA) &&
          term(T_LBRACKET) && (reg_immed = p_register_and_immediate()) && 
          term(T_RBRACKET) && term(T_EOL))
    ) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = dreg->value.u;
        f.instruction.reg2 = reg_immed->reg;
        f.instruction.immed4 = 0;

        inst.second.s = reg_immed->immed.s;

        bool make_teeny = is_teeny(inst.second.s);
        if(make_teeny) {
            f.instruction.immed4 = inst.second.s;
            f.instruction.teeny = 1;
            address += 1;
        }
        else {
            address += 2;
        }

        if(pass > 1) {
            bin_words.push_back(f);
            if(!make_teeny) {
                bin_words.push_back(inst.second);
            }
        }

        result = true;
    }

    return result;
}

/*
 * code_3_line ::= code_3_inst REGISTER COMMA immediate.
 * code_3_line ::= code_3_mem_inst REGISTER COMMA LBRACKET immediate RBRACKET.
 */
bool p_code_3_line() {
    bool result = false;
    shared_ptr <token> dreg, oper;
    shared_ptr <tny_word> immed;
    int save = tnext;
    if(
        (tnext = save,
         (oper = p_code_3_inst()) && (dreg = term(T_REGISTER)) && term(T_COMMA) &&
         (immed = p_immediate()) && term(T_EOL))
        ||
        (tnext = save,
         (oper = p_code_3_mem_inst()) && (dreg = term(T_REGISTER)) && term(T_COMMA) &&
         term(T_LBRACKET) && (immed = p_immediate()) && term(T_RBRACKET) &&
         term(T_EOL))
    ) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = dreg->value.u;
        f.instruction.reg2 = TNY_REG_ZERO;
        f.instruction.immed4 = 0;

        inst.second.s = immed->s;
        if(oper->id == T_ROL || oper->id == T_SHL) {
            inst.second.s = -inst.second.s;
        }
        /* NOTE: do NOT use "immed" after this as it's sign
         *  may not be modified by the above condition 
         */

        bool make_teeny = is_teeny(inst.second.s);
        if(make_teeny) {
            f.instruction.immed4 = inst.second.s;
            f.instruction.teeny = 1;
            address += 1;
        }
        else {
            address += 2;
        }

        if(pass > 1) {
            bin_words.push_back(f);
            if(!make_teeny) {
                bin_words.push_back(inst.second);
            }
        }

        result = true;
    }

    return result;
}

/*
 * code_4_line ::= code_4_inst REGISTER.
 */
bool p_code_4_line() {
    bool result = false;
    shared_ptr <token> dreg, oper;
    if((oper = p_code_4_inst()) && (dreg = term(T_REGISTER)) && term(T_EOL)) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 1;
        f.instruction.reg1 = dreg->value.u;
        f.instruction.reg2 = 0;
        f.instruction.immed4 = 0;

        /* In this grammar rule, POP uses the SP register implicitly */
        if(oper->id == T_POP) {
            f.instruction.reg2 = TNY_REG_SP;
        }

        address += 1;

        if(pass > 1) {
            bin_words.push_back(f);
        }

        result = true;
    }

    return result;
}

/*
 * code_5_line ::= code_5_inst REGISTER.
 */
bool p_code_5_line() {
    bool result = false;
    shared_ptr <token> dreg, oper;
    if((oper = p_code_5_inst()) && (dreg = term(T_REGISTER)) && term(T_EOL)) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 1;
        f.instruction.reg1 = dreg->value.u;
        f.instruction.reg2 = TNY_REG_ZERO;
        
        if(oper->id == T_INC || oper->id == T_DEC) {
            f.instruction.immed4 = 1;
        }
        else if(oper->id == T_INV) {
            f.instruction.immed4 = ~0;
        }

        address += 1;

        if(pass > 1) {
            bin_words.push_back(f);
        }

        result = true;
    }

    return result;
}

/*
 * code_6_line ::= code_6_inst LBRACKET register_and_immediate RBRACKET COMMA REGISTER.
 */
bool p_code_6_line() {
    bool result = false;
    shared_ptr <token> oper, sreg;
    shared_ptr <reg_and_immed>reg_immed;
    if((oper = p_code_6_inst()) && term(T_LBRACKET) &&
       (reg_immed = p_register_and_immediate()) && term(T_RBRACKET) &&
       term(T_COMMA) && (sreg = term(T_REGISTER)) && term(T_EOL)
    ) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = reg_immed->reg;
        f.instruction.reg2 = sreg->value.u;
        f.instruction.immed4 = 0;

        inst.second.s = reg_immed->immed.s;

        bool make_teeny = is_teeny(inst.second.s);
        if(make_teeny) {
            f.instruction.immed4 = inst.second.s;
            f.instruction.teeny = 1;
            address += 1;
        }
        else {
            address += 2;
        }

        if(pass > 1) {
            bin_words.push_back(f);
            if(!make_teeny) {
                bin_words.push_back(inst.second);
            }
        }

        result = true;
    }

    return result;
}

/*
 * code_7_line ::= code_7_inst LBRACKET immediate RBRACKET COMMA REGISTER.
 */
bool p_code_7_line() {
    bool result = false;
    shared_ptr <token> oper, sreg;
    shared_ptr <tny_word> immed;
    if((oper = p_code_7_inst()) && term(T_LBRACKET) && (immed = p_immediate()) &&
       term(T_RBRACKET) && term(T_COMMA) && (sreg = term(T_REGISTER)) &&
       term(T_EOL)) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = TNY_REG_ZERO;
        f.instruction.reg2 = sreg->value.u;
        f.instruction.immed4 = 0;

        inst.second.s = immed->s;

        bool make_teeny = is_teeny(inst.second.s);
        if(make_teeny) {
            f.instruction.immed4 = inst.second.s;
            f.instruction.teeny = 1;
            address += 1;
        }
        else {
            address += 2;
        }

        if(pass > 1) {
            bin_words.push_back(f);
            if(!make_teeny) {
                bin_words.push_back(inst.second);
            }
        }

        result = true;
    }

    return result;
}

/*
 * code_8_line ::= code_8_inst LBRACKET REGISTER RBRACKET COMMA REGISTER.
 */
bool p_code_8_line() {
    bool result = false;
    shared_ptr <token> dreg, oper, sreg;
    if((oper = p_code_8_inst()) && term(T_LBRACKET) && (dreg = term(T_REGISTER)) &&
       term(T_RBRACKET) && term(T_COMMA) && (sreg = term(T_REGISTER)) && term(T_EOL)) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 1;
        f.instruction.reg1 = dreg->value.u;
        f.instruction.reg2 = sreg->value.u;
        f.instruction.immed4 = 0;

        address += 1;

        if(pass > 1) {
            bin_words.push_back(f);
        }

        result = true;
    }

    return result;
}

/*
 * code_9_line ::= code_9_inst REGISTER.
 */
bool p_code_9_line() {
    bool result = false;
    shared_ptr <token> oper, sreg;
    shared_ptr <tny_word> immed;
    if((oper = p_code_9_inst()) && (sreg = term(T_REGISTER)) && term(T_EOL)) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 1;
        f.instruction.reg1 = 0;
        f.instruction.reg2 = sreg->value.u;
        f.instruction.immed4 = 0;

        /* In this grammar rule, PSH uses the SP register implicitly */
        if(oper->id == T_PSH){
            f.instruction.reg1 = TNY_REG_SP;
        }

        address += 1;

        if(pass > 1) {
            bin_words.push_back(f);
        }

        result = true;
    }

    return result;
}

/*
 * code_10_line ::= code_10_inst register_and_immediate.
 */
bool p_code_10_line() {
    bool result = false;
    shared_ptr <token> oper;
    shared_ptr <reg_and_immed>reg_immed;
    if((oper = p_code_10_inst()) && (reg_immed = p_register_and_immediate()) && term(T_EOL)) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = 0;
        f.instruction.reg2 = reg_immed->reg;
        f.instruction.immed4 = 0;

        /* In this grammar rule, PSH uses the SP register implicitly */
        if(oper->id == T_PSH){
            f.instruction.reg1 = TNY_REG_SP;
        }

        inst.second.s = reg_immed->immed.s;

        bool make_teeny = is_teeny(inst.second.s);
        if(make_teeny) {
            f.instruction.immed4 = inst.second.s;
            f.instruction.teeny = 1;
            address += 1;
        }
        else {
            address += 2;
        }

        if(pass > 1) {
            bin_words.push_back(f);
            if(!make_teeny) {
                bin_words.push_back(inst.second);
            }
        }

        result = true;
    }

    return result;
}

/*
 * code_11_line ::= code_11_inst immediate.
 */
bool p_code_11_line() {
    bool result = false;
    shared_ptr <token> oper;
    shared_ptr <tny_word> immed;
    if((oper = p_code_11_inst()) && (immed = p_immediate()) && term(T_EOL)) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = 0;
        f.instruction.reg2 = TNY_REG_ZERO;
        f.instruction.immed4 = 0;

        /* In this grammar rule, PSH uses the SP register implicitly */
        if(oper->id == T_PSH){
            f.instruction.reg1 = TNY_REG_SP;
        }

        inst.second.s = immed->s;

        bool make_teeny = is_teeny(inst.second.s);
        if(make_teeny) {
            f.instruction.immed4 = inst.second.s;
            f.instruction.teeny = 1;
            address += 1;
        }
        else {
            address += 2;
        }

        if(pass > 1) {
            bin_words.push_back(f);
            if(!make_teeny) {
                bin_words.push_back(inst.second);
            }
        }

        result = true;
    }

    return result;
}

/*
 * code_12_line ::= code_12_inst.
 */
bool p_code_12_line() {
    bool result = false;
    shared_ptr <token> oper;
    if((oper = p_code_12_inst()) && term(T_EOL)) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 1;
        f.instruction.reg2 = 0;
        f.instruction.immed4 = 0;

        switch(oper->id) {
        case T_RET:  /* implemented as POP PC, [SP] */
            f.instruction.reg1 = TNY_REG_PC;
            f.instruction.reg2 = TNY_REG_SP;
            break;
        case T_RTI:
            f.instruction.reg1 = 0;
        default:
            /* TODO: This should never happen, what to do if it does */
            break;
        }

        address += 1;

        if(pass > 1) {
            bin_words.push_back(f);
        }

        result = true;
    }

    return result;
}

/*
 * code_13_line ::= code_13_inst REGISTER.
 */
bool p_code_13_line() {
    bool result = false;
    shared_ptr <token> oper, sreg;
    if((oper = p_code_13_inst()) && (sreg = term(T_REGISTER)) && term(T_EOL)) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;  // jump instructions can never be teeny
        f.instruction.reg1 = sreg->value.u;
        f.instruction.reg2 = 0;

        f.instruction.immed4 = 0;
        if(oper->id == T_JNE || oper->id == T_JG || oper->id == T_JGE) {
            f.inst_flags.greater = 1;
        }
        if(oper->id == T_JNE || oper->id == T_JL || oper->id == T_JLE) {
            f.inst_flags.less = 1;
        }
        if(oper->id == T_JE || oper->id == T_JLE || oper->id == T_JGE) {
            f.inst_flags.equals = 1;
        }
        if(false) {
            f.inst_flags.carry = 1;
        }

        inst.second.s = 0;

        address+=2;

        if(pass > 1) {
            bin_words.push_back(f);
            bin_words.push_back(inst.second);
        }

        result = true;
    }

    return result;
}

/*
 * code_14_line ::= code_14_inst register_and_immediate.
 */
bool p_code_14_line() {
    bool result = false;
    shared_ptr <token> oper;
    shared_ptr <reg_and_immed>reg_immed;
    if((oper = p_code_14_inst()) && (reg_immed = p_register_and_immediate()) && term(T_EOL)) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = reg_immed->reg;
        f.instruction.reg2 = 0;

        f.instruction.immed4 = 0;
        if(oper->id == T_JNE || oper->id == T_JG || oper->id == T_JGE) {
            f.inst_flags.greater = 1;
        }
        if(oper->id == T_JNE || oper->id == T_JL || oper->id == T_JLE) {
            f.inst_flags.less = 1;
        }
        if(oper->id == T_JE || oper->id == T_JLE || oper->id == T_JGE) {
            f.inst_flags.equals = 1;
        }
        if(false) {
            f.inst_flags.carry = 1;
        }

        inst.second.s = reg_immed->immed.s;

        address += 2;

        if(pass > 1) {
            bin_words.push_back(f);
            bin_words.push_back(inst.second);
         }

        result = true;
    }

    return result;
}

/*
 * code_15_line ::= code_15_inst immediate.
 */
bool p_code_15_line() {
    bool result = false;
    shared_ptr <token> oper;
    shared_ptr <tny_word> immed;
    if((oper = p_code_15_inst()) && (immed = p_immediate()) && term(T_EOL)) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = TNY_REG_ZERO;
        f.instruction.reg2 = 0;

        f.instruction.immed4 = 0;
        if(oper->id == T_JNE || oper->id == T_JG || oper->id == T_JGE) {
            f.inst_flags.greater = 1;
        }
        if(oper->id == T_JNE || oper->id == T_JL || oper->id == T_JLE) {
            f.inst_flags.less = 1;
        }
        if(oper->id == T_JE || oper->id == T_JLE || oper->id == T_JGE) {
            f.inst_flags.equals = 1;
        }
        if(false) {
            f.inst_flags.carry = 1;
        }

        inst.second.s = immed->s;

        address += 2;

        if(pass > 1) {
            bin_words.push_back(f);
            bin_words.push_back(inst.second);
         }

        result = true;
    }

    return result;
}

/*
 * code_16_line ::= code_16_inst LBRACKET REGISTER RBRACKET COMMA register_and_immediate.
 */
bool p_code_16_line() {
    bool result = false;
    shared_ptr <token> dreg, oper;
    shared_ptr <reg_and_immed>reg_immed = nullptr;
    int save = tnext;
    if(
        (tnext = save,
         (oper = p_code_16_inst()) && term(T_LBRACKET) && (dreg = term(T_REGISTER)) && 
          term(T_RBRACKET) && term(T_COMMA) && (reg_immed = p_register_and_immediate()) && term(T_EOL))
    ) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = dreg->value.u;
        f.instruction.reg2 = reg_immed->reg;
        f.instruction.immed4 = 0;

        inst.second.s = reg_immed->immed.s;

        bool make_teeny = is_teeny(inst.second.s);
        if(make_teeny) {
            f.instruction.immed4 = inst.second.s;
            f.instruction.teeny = 1;
            address += 1;
        }
        else {
            address += 2;
        }

        if(pass > 1) {
            bin_words.push_back(f);
            if(!make_teeny) {
                bin_words.push_back(inst.second);
            }
        }

        result = true;
    }

    return result;
}

/*
 * code_17_line ::= code_17_inst LBRACKET REGISTER RBRACKET COMMA immediate.
 */
bool p_code_17_line() {
    bool result = false;
    shared_ptr <token> dreg, oper;
    shared_ptr <tny_word> immed;
    int save = tnext;
    if(
        (tnext = save,
         (oper = p_code_17_inst()) && term(T_LBRACKET) && (dreg = term(T_REGISTER)) && 
          term(T_RBRACKET) && term(T_COMMA) && (immed = p_immediate()) && term(T_EOL))
    ) {

        instruction inst;
        inst.line_no = oper->line_no;

        tny_word &f = inst.first;
        f.instruction.opcode = token_to_opcode(oper->id);
        f.instruction.teeny = 0;
        f.instruction.reg1 = dreg->value.u;
        f.instruction.reg2 = TNY_REG_ZERO;
        f.instruction.immed4 = 0;

        inst.second.s = immed->s;

        bool make_teeny = is_teeny(inst.second.s);
        if(make_teeny) {
            f.instruction.immed4 = inst.second.s;
            f.instruction.teeny = 1;
            address += 1;
        }
        else {
            address += 2;
        }

        if(pass > 1) {
            bin_words.push_back(f);
            if(!make_teeny) {
                bin_words.push_back(inst.second);
            }
        }

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
    case T_SHL:   result = TNY_OPCODE_SHF;   break;
    case T_SHR:   result = TNY_OPCODE_SHF;   break;
    case T_ROT:   result = TNY_OPCODE_ROT;   break;
    case T_ROL:   result = TNY_OPCODE_ROT;   break;
    case T_ROR:   result = TNY_OPCODE_ROT;   break;
    case T_NEG:   result = TNY_OPCODE_NEG;   break;
    case T_CMP:   result = TNY_OPCODE_CMP;   break;
    case T_DLY:   result = TNY_OPCODE_DLY;   break;
    case T_INT:   result = TNY_OPCODE_INT;   break;
    case T_RTI:   result = TNY_OPCODE_RTI;   break;
    case T_JMP:   result = TNY_OPCODE_JMP;   break;
    case T_JE:    result = TNY_OPCODE_JMP;   break;
    case T_JNE:   result = TNY_OPCODE_JMP;   break;
    case T_JL:    result = TNY_OPCODE_JMP;   break;
    case T_JLE:   result = TNY_OPCODE_JMP;   break;
    case T_JG:    result = TNY_OPCODE_JMP;   break;
    case T_JGE:   result = TNY_OPCODE_JMP;   break;
    case T_LUP:   result = TNY_OPCODE_LUP;   break;
    /*
     * The following are pseudo-instructions
     */
    case T_INC:   result = TNY_OPCODE_ADD;   break;
    case T_DEC:   result = TNY_OPCODE_SUB;   break;
    case T_INV:   result = TNY_OPCODE_XOR;   break;
    case T_RET:   result = TNY_OPCODE_POP;   break;
    default:
        cerr << "ERROR, line " << parse_line[0].line_no << ": ";
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
 * code_1_inst ::= BTS.
 * code_1_inst ::= BTC.
 * code_1_inst ::= BTF.
 * code_1_inst ::= CMP.
 * code_1_inst ::= LUP.
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
    (tnext = save, result = term(T_BTS)) ||
    (tnext = save, result = term(T_BTC)) ||
    (tnext = save, result = term(T_BTF)) ||
    (tnext = save, result = term(T_CMP)) ||
    (tnext = save, result = term(T_LUP)) ||
    (tnext = save, result = term(T_DLY));

    return result;
}

/*
 * code_1_mem_inst ::= LOD.
 * code_1_mem_inst ::= POP.
 */
shared_ptr <token> p_code_1_mem_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_LOD)) ||
    (tnext = save, result = term(T_POP));

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
 * code_2_inst ::= BTS.
 * code_2_inst ::= BTC.
 * code_2_inst ::= BTF.
 * code_2_inst ::= CMP.
 * code_2_inst ::= LUP.
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
    (tnext = save, result = term(T_BTS)) ||
    (tnext = save, result = term(T_BTC)) ||
    (tnext = save, result = term(T_BTF)) ||
    (tnext = save, result = term(T_CMP)) ||
    (tnext = save, result = term(T_LUP)) ||
    (tnext = save, result = term(T_DLY));

    return result;
}

/*
 * code_2_mem_inst ::= LOD.
 */
shared_ptr <token> p_code_2_mem_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_LOD));

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
 * code_3_inst ::= SHL.
 * code_3_inst ::= SHR.
 * code_3_inst ::= ROT.
 * code_3_inst ::= ROL.
 * code_3_inst ::= ROR.
 * code_3_inst ::= SET.
 * code_3_inst ::= LOD.
 * code_3_inst ::= BTS.
 * code_3_inst ::= BTC.
 * code_3_inst ::= BTF.
 * code_3_inst ::= CMP.
 * code_3_inst ::= LUP.
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
    (tnext = save, result = term(T_SHL)) ||
    (tnext = save, result = term(T_SHR)) ||
    (tnext = save, result = term(T_ROT)) ||
    (tnext = save, result = term(T_ROL)) ||
    (tnext = save, result = term(T_ROR)) ||
    (tnext = save, result = term(T_SET)) ||
    (tnext = save, result = term(T_BTS)) ||
    (tnext = save, result = term(T_BTC)) ||
    (tnext = save, result = term(T_BTF)) ||
    (tnext = save, result = term(T_CMP)) ||
    (tnext = save, result = term(T_LUP)) ||
    (tnext = save, result = term(T_DLY));

    return result;
}

/*
 * code_3_mem_inst ::= LOD.
 */
shared_ptr <token> p_code_3_mem_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_LOD));

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
 * code_5_inst ::= INV.
 */
shared_ptr <token> p_code_5_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_INC)) ||
    (tnext = save, result = term(T_DEC)) ||
    (tnext = save, result = term(T_INV));

    return result;
}

/*
 * code_6_inst ::= STR.
 */
shared_ptr <token> p_code_6_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_STR));

    return result;
}

/*
 * code_7_inst ::= STR.
 */
shared_ptr <token> p_code_7_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_STR));

    return result;
}

/*
 * code_8_inst ::= STR.
 * code_8_inst ::= PSH.
 */
shared_ptr <token> p_code_8_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_STR)) ||
    (tnext = save, result = term(T_PSH));

    return result;
}

/*
 * code_9_inst ::= PSH.
 * code_9_inst ::= DLY.
 * code_9_inst ::= CAL.
 * code_9_inst ::= INT.
 */
shared_ptr <token> p_code_9_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_PSH)) ||
    (tnext = save, result = term(T_DLY)) ||
    (tnext = save, result = term(T_CAL)) ||
    (tnext = save, result = term(T_INT));

    return result;
}

/*
 * code_10_inst ::= PSH.
 * code_10_inst ::= DLY.
 * code_10_inst ::= CAL.
 * code_10_inst ::= INT.
 */
shared_ptr <token> p_code_10_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_PSH)) ||
    (tnext = save, result = term(T_DLY)) ||
    (tnext = save, result = term(T_CAL)) ||
    (tnext = save, result = term(T_INT));

    return result;
}

/*
 * code_11_inst ::= PSH.
 * code_11_inst ::= DLY.
 * code_11_inst ::= CAL.
 * code_11_inst ::= INT.
 */
shared_ptr <token> p_code_11_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_PSH)) ||
    (tnext = save, result = term(T_DLY)) ||
    (tnext = save, result = term(T_CAL)) ||
    (tnext = save, result = term(T_INT));

    return result;
}

/*
 * code_12_inst ::= RET.
 * code_12_inst ::= RTI.
 */
shared_ptr <token> p_code_12_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_RET)) ||
    (tnext = save, result = term(T_RTI));

    return result;
}

/*
 * code_13_inst ::= JMP.
 * code_13_inst ::= JE.
 * code_13_inst ::= JNE.
 * code_13_inst ::= JL.
 * code_13_inst ::= JLE.
 * code_13_inst ::= JG.
 * code_13_inst ::= JGE.
 */
shared_ptr <token> p_code_13_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_JMP)) ||
    (tnext = save, result = term(T_JE)) ||
    (tnext = save, result = term(T_JNE)) ||
    (tnext = save, result = term(T_JL)) ||
    (tnext = save, result = term(T_JLE)) ||
    (tnext = save, result = term(T_JG)) ||
    (tnext = save, result = term(T_JGE));

    return result;
}

/*
 * code_14_inst ::= JMP.
 * code_14_inst ::= JE.
 * code_14_inst ::= JNE.
 * code_14_inst ::= JL.
 * code_14_inst ::= JLE.
 * code_14_inst ::= JG.
 * code_14_inst ::= JGE.
 */
shared_ptr <token> p_code_14_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_JMP)) ||
    (tnext = save, result = term(T_JE)) ||
    (tnext = save, result = term(T_JNE)) ||
    (tnext = save, result = term(T_JL)) ||
    (tnext = save, result = term(T_JLE)) ||
    (tnext = save, result = term(T_JG)) ||
    (tnext = save, result = term(T_JGE));

    return result;
}

/*
 * code_15_inst ::= JMP.
 * code_15_inst ::= JE.
 * code_15_inst ::= JNE.
 * code_15_inst ::= JL.
 * code_15_inst ::= JLE.
 * code_15_inst ::= JG.
 * code_15_inst ::= JGE.
 */
shared_ptr <token> p_code_15_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_JMP)) ||
    (tnext = save, result = term(T_JE)) ||
    (tnext = save, result = term(T_JNE)) ||
    (tnext = save, result = term(T_JL)) ||
    (tnext = save, result = term(T_JLE)) ||
    (tnext = save, result = term(T_JG)) ||
    (tnext = save, result = term(T_JGE));

    return result;
}

/*
 * code_16_inst ::= PSH.
 */
shared_ptr <token> p_code_16_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_PSH));

    return result;
}

/*
 * code_17_inst ::= PSH.
 */
shared_ptr <token> p_code_17_inst() {
    shared_ptr <token> result;
    int save = tnext;

    (tnext = save, result = term(T_PSH));

    return result;
}
