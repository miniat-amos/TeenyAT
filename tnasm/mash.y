%include {
	#include <assert.h>
	#include <string.h>
	#include <stdbool.h>

	#include "token.h"
	#include "mash.h"
	#include "symbol_tables.h"
	#include "util.h"
	#include "miniat/miniat.h"
	#include "miniat_defines.h"
	#include "blocks.h"

	/*
	 * This unusual variable is used to push the line number of certain
	 * constant_immediate up the parse tree to the appropriate raw_value.
	 * Basically, it's used at a low Token level to capture the line
	 * number before the rules begin passing only "m_uword"s up.  The
	 * higher rule can then use it to determine the line number.  This
	 * data is really only utilized when generating listings.
	 */
	m_uword most_recent_const_immed_line_num;

	bool current_branch_is_flush = false;

	/* Use a bogus line_num until matching rules for a specific type of code line.
	 *
	 * E.g., alu_code_line
	 *
	 * These lines will always have an instruction Token* from which the correct
	 * line number can be determined.
	 */
	void set_instruction(instruction *inst, m_uword predicate, m_uword hint, m_uword unused,
	                     m_uword opcode, m_uword rA, m_uword rB, m_uword rC, m_uword immed,
	                     int line_num) {

		inst->low.predicate = predicate;
		inst->low.hint = hint;
		inst->low.unused = unused;
		inst->low.opcode = opcode;
		inst->low.rA = rA;
		inst->low.rB = rB;
		inst->low.rC = rC;
		inst->high.u = immed;
		inst->line_num = line_num;

		return;
	}
}

%start_symbol mash_program
%token_prefix T_

%token_type {Token *}

%syntax_error {
	Token *t = yyminor.yy0;
	switch(t->type) {
	case T_EOL:
		STAGE_ERROR(t->line_num, "Unexpected end of line");
		break;
	default:
		STAGE_ERROR(t->line_num, "Unexpected %s token, \"%s\"", yyTokenName[t->type], t->token_str);
		break;
	}
}

mash_program ::= eols lines.
mash_program ::= lines.

eols ::= eols EOL.
eols ::= EOL.
eols ::= error EOL.

lines ::= lines mode_line eols.
lines ::= lines constant_line eols.
lines ::= lines variable_line eols.
lines ::= lines address_line eols. {
		inline_variables = true;
}
lines ::= lines label_line eols. {
		inline_variables = true;
}
lines ::= lines raw_line eols. {
		inline_variables = true;
}
lines ::= lines code_line eols. {
		inline_variables = true;
		address.u += 2;
}
lines ::= .

mode_line ::= MODE_DIRECTIVE NOP_DELAY ON. {
	mode_nop_delay = true;
}
mode_line ::= MODE_DIRECTIVE NOP_DELAY OFF. {
	mode_nop_delay = false;
}

constant_line ::= CONST_DIRECTIVE IDENTIFIER(id) constant_immediate(immed). {
	if(pass == 1) {
		symbol *sym;
		if(!symbol_table_search(variables, id->token_str)) {
			sym = symbol_table_add(constants, id->token_str, immed);
			if(!sym) {
				sym = symbol_table_search(constants, id->token_str);
				if(sym) {
					STAGE_ERROR(id->line_num, "The constant, \"%s\", is already defined as 0x%08X", id->token_str, sym->value.u);
				}
				else {
					IMPOSSIBLE("Crepes be burned to William!!!");
				}
			}
		}
		else {
			STAGE_ERROR(id->line_num, "The identifier \"%s\" has already been defined as a variable", id->token_str);
		}
	}
}

variable_line ::= VAR_DIRECTIVE IDENTIFIER(id). {
	if(pass == 1) {
		m_uword *addr;
		symbol *sym;
		if(!symbol_table_search(constants, id->token_str)) {
			addr = (inline_variables ? &address.u : &variable_address.u);
			sym = symbol_table_add(variables, id->token_str, (m_word)*addr);
			if(sym != NULL) {
				(*addr)++;
			}
			else {
				sym = symbol_table_search(variables, id->token_str);
				if(sym) {
					STAGE_ERROR(id->line_num, "The variable, \"%s\", is already defined at address 0x%04X", id->token_str, sym->value.u);
				}
				else {
					IMPOSSIBLE("There really is a Wookalar!!!");
				}
			}
		}
		else {
			STAGE_ERROR(id->line_num, "The identifier \"%s\" has already been defined as a constant", id->token_str);
		}
	}
}
variable_line ::= VAR_DIRECTIVE IDENTIFIER(id) LEFT_BRACKET number(num) RIGHT_BRACKET. {
	if(pass == 1) {
		m_uword *addr;
		symbol *sym;
		if(!symbol_table_search(constants, id->token_str)) {
			addr = (inline_variables ? &address.u : &variable_address.u);
			sym = symbol_table_add(variables, id->token_str, (m_word)*addr);
			if(sym != NULL) {
				*addr += num.u;
			}
			else {
				sym = symbol_table_search(variables, id->token_str);
				if(sym) {
					STAGE_ERROR(id->line_num, "The variable, \"%s\", is already defined at address 0x%04X", id->token_str, sym->value.u);
				}
				else {
					IMPOSSIBLE("Stew kidneys with beans.");
				}
			}
		}
		else {
			STAGE_ERROR(id->line_num, "The identifier \"%s\" has already been defined as a constant", id->token_str);
		}
	}
}
variable_line ::= VAR_DIRECTIVE IDENTIFIER(id) LEFT_BRACKET number(num1) COMMA number(num2) RIGHT_BRACKET. {
	if(pass == 1) {
		if(!symbol_table_search(constants, id->token_str)) {
			if(num1.s > num2.s) {
				char s[] = "The variable array \"%s\" requires its lowest index be at least as low as it's highest";
				STAGE_ERROR(id->line_num, s, id->token_str);
			}
			else {
				m_word base;
				m_word size;
				m_uword *addr;
				symbol *sym;
				addr = (inline_variables ? &address.u : &variable_address.u);
				size.s = num2.s - num1.s + 1;
				base.s = *addr - num1.s;
				sym = symbol_table_add(variables, id->token_str, base);
				if(sym != NULL) {
					*addr += size.s;
				}
				else {
					sym = symbol_table_search(variables, id->token_str);
					if(sym) {
						STAGE_ERROR(id->line_num, "The variable, \"%s\", is already defined at address 0x%04X", id->token_str, sym->value.u);
					}
					else {
						IMPOSSIBLE("Sew buttons on my underwear.");
					}
				}
			}
		}
		else {
			STAGE_ERROR(id->line_num, "The identifier \"%s\" has already been defined as a constant", id->token_str);
		}
	}
}

address_line ::= ADDR_DIRECTIVE address_immediate(immed). {
	address = immed;
	if(pass == 2) {
		block_new(address.u);
	}
}

label_line ::= LABEL(id). {
	if(pass == 1) {
		symbol *sym = symbol_table_add(labels, id->token_str, address);
		if(sym == NULL) {
			sym = symbol_table_search(labels, id->token_str);
			if(sym) {
				STAGE_ERROR(id->line_num, "The label, \"%s\", is already defined for address 0x%04X", id->token_str, sym->value.u);
			}
			else {
				IMPOSSIBLE("Grease the pig and send it home.");
			}
		}
	}
}

raw_line ::= raw_line raw_value.
raw_line ::= raw_value.

raw_value ::= constant_immediate(immed). {
	if(pass == 2) {
		block_new_word(most_recent_const_immed_line_num, immed.u);
	}
	address.u++;
}
raw_value ::= STRING(s). {
	if(pass == 2) {
		int i;
		for(i = 0; i < strlen(s->token_str) + 1; i++) {
			block_new_word(s->line_num, s->token_str[i]);
		}
	}
	address.u += strlen(s->token_str) + 1;
}
raw_value ::= PACKED_STRING(ps). {
	if(pass == 2) {
		m_uword word;
		char * s = ps->token_str;
		int len = strlen(s) + 1;

		/*
		 * first do whole words
		 */
		while(len >= 4) {
			int i;
			word = 0;
			for(i = 3; i >=0; i--) {
				word <<= 8;
				word |= s[i] & ~(~0U << 8);
			}
			block_new_word(ps->line_num, word);
			len -= 4;
			s += 4;
		}

		/*
		 * finish with partial word
		 */
		if(len > 0) {
			int i;
			word = 0;
			for(i = len - 1; i >=0; i--) {
				word <<= 8;
				word |= s[i] & ~(~0U << 8);
			}
			block_new_word(ps->line_num, word);
		}
	}
	address.u += (strlen(ps->token_str) + 4) / 4;
}

%type code_line {instruction}
code_line ::= optional_predicate(pred) rest_of_the_code_line(rest). {
	if(pred) {
		rest.low.predicate = 1;
	}
	if(pass == 2) {
		int i;
		block_new_word(rest.line_num, rest.low.all);
		block_new_word(rest.line_num, rest.high.u);
		if(mode_nop_delay) {
			/* Add 3 NOPs (6 words of 0) to fill delay slots */
			switch(rest.low.opcode) {
			case M_BRANE:
				if(current_branch_is_flush) {
					break;
				}
			case M_BRAE:
			case M_BRAL:
			case M_BRALE:
			case M_BRAG:
			case M_BRAGE:
			case M_INT:
			case M_IRET:
				for(i = 0; i < 6; i++) {
					block_new_word(rest.line_num, 0);
				}
				break;
			default:
				break;
			}
		}
	}
	if(mode_nop_delay) {
		/* Add 3 NOPs (6 words of 0) to fill delay slots */
		switch(rest.low.opcode) {
		case M_BRANE:
			if(current_branch_is_flush) {
				break;
			}
		case M_BRAE:
		case M_BRAL:
		case M_BRALE:
		case M_BRAG:
		case M_BRAGE:
		case M_INT:
		case M_IRET:
			address.u += 6;
			break;
		default:
			break;
		}
	}
	current_branch_is_flush = false;
}

optional_predicate(val) ::= PREDICATE(pred). {
	val = pred;
}
optional_predicate(val) ::= . {
	val = NULL;
}

%type rest_of_the_code_line {instruction}
rest_of_the_code_line(val) ::= complete_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= alu_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= data_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= control_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= int_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= int_return_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= neg_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= inv_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= inc_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= dec_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= movr_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= movi_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= bra_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= nop_code_line(x). {
	val = x;
}
rest_of_the_code_line(val) ::= flush_code_line(x). {
	val = x;
}

%type complete_code_line {instruction}
complete_code_line(val) ::= alu_instruction(inst) REGISTER(regA) comma REGISTER(regB) comma REGISTER(regC) comma immediate(immed). {
	set_instruction(&val, 0, 0, 0, inst->data.u, regA->data.u, regB->data.u, regC->data.u, immed.u, inst->line_num);
}
complete_code_line(val) ::= data_load_instruction(inst) REGISTER(regA) comma REGISTER(regB) comma REGISTER(regC) comma immediate(immed). {
	set_instruction(&val, 0, 0, 0, inst->data.u, regA->data.u, regB->data.u, regC->data.u, immed.u, inst->line_num);
}
complete_code_line(val) ::= data_stor_instruction(inst) REGISTER(regA) comma REGISTER(regB) comma REGISTER(regC) comma immediate(immed). {
	set_instruction(&val, 0, 0, 0, inst->data.u, regA->data.u, regB->data.u, regC->data.u, immed.u, inst->line_num);
}
complete_code_line(val) ::= control_instruction(inst) REGISTER(regA) comma REGISTER(regB) comma REGISTER(regC) comma immediate(immed). {
	set_instruction(&val, 0, 0, 0, inst->data.u, regA->data.u, regB->data.u, regC->data.u, immed.u, inst->line_num);
}

comma ::= COMMA.
comma ::= .

%type alu_code_line {instruction}
alu_code_line(val) ::= alu_instruction(inst) REGISTER(regA) EQUALS REGISTER(regB) alu_operator(oper) paren_reg_immed_expr(expr). {
	bool good_match = false;
	switch(inst->type) {
	case T_ADD:  if(oper->type == T_ADD_OPER)  good_match = true; break;
	case T_SUB:  if(oper->type == T_SUB_OPER)  good_match = true; break;
	case T_MULT: if(oper->type == T_MULT_OPER) good_match = true; break;
	case T_DIV:  if(oper->type == T_DIV_OPER)  good_match = true; break;
	case T_MOD:  if(oper->type == T_MOD_OPER)  good_match = true; break;
	case T_AND:  if(oper->type == T_AND_OPER)  good_match = true; break;
	case T_OR:   if(oper->type == T_OR_OPER)   good_match = true; break;
	case T_EXOR: if(oper->type == T_EXOR_OPER) good_match = true; break;
	case T_SHL:  if(oper->type == T_SHL_OPER)  good_match = true; break;
	case T_SHR:  if(oper->type == T_SHR_OPER)  good_match = true; break;
	}
	if(good_match) {
		set_instruction(&val, 0, 0, 0, inst->data.u, regA->data.u, regB->data.u, expr.low.rC, expr.high.u, inst->line_num);
	}
	else {
		STAGE_ERROR(regA->line_num, "The operator, \"%s\", doesn't match the instruction", oper->token_str);
	}
}

alu_instruction(val) ::= ADD(inst). {
	val = inst;
	inst->data.u = M_ADD;
}
alu_instruction(val) ::= SUB(inst). {
	val = inst;
	inst->data.u = M_SUB;
}
alu_instruction(val) ::= MULT(inst). {
	val = inst;
	inst->data.u = M_MULT;
}
alu_instruction(val) ::= DIV(inst). {
	val = inst;
	inst->data.u = M_DIV;
}
alu_instruction(val) ::= MOD(inst). {
	val = inst;
	inst->data.u = M_MOD;
}
alu_instruction(val) ::= AND(inst). {
	val = inst;
	inst->data.u = M_AND;
}
alu_instruction(val) ::= OR(inst). {
	val = inst;
	inst->data.u = M_OR;
}
alu_instruction(val) ::= EXOR(inst). {
	val = inst;
	inst->data.u = M_EXOR;
}
alu_instruction(val) ::= SHL(inst). {
	val = inst;
	inst->data.u = M_SHL;
}
alu_instruction(val) ::= SHR(inst). {
	val = inst;
	inst->data.u = M_SHR;
}

alu_operator(val) ::= ADD_OPER(op). {
	val = op;
}
alu_operator(val) ::= SUB_OPER(op). {
	val = op;
}
alu_operator(val) ::= MULT_OPER(op). {
	val = op;
}
alu_operator(val) ::= DIV_OPER(op). {
	val = op;
}
alu_operator(val) ::= MOD_OPER(op). {
	val = op;
}
alu_operator(val) ::= AND_OPER(op). {
	val = op;
}
alu_operator(val) ::= OR_OPER(op). {
	val = op;
}
alu_operator(val) ::= EXOR_OPER(op). {
	val = op;
}
alu_operator(val) ::= SHL_OPER(op). {
	val = op;
}
alu_operator(val) ::= SHR_OPER(op). {
	val = op;
}

%type paren_reg_immed_expr {instruction}
paren_reg_immed_expr(val) ::= LEFT_PAREN reg_immediate_expr(expr) RIGHT_PAREN. {
	set_instruction(&val, 0, 0, 0, 0, 0, 0, expr.low.rC, expr.high.u, -1);
}
paren_reg_immed_expr(val) ::= REGISTER(reg). {
		set_instruction(&val, 0, 0, 0, 0, 0, 0, reg->data.u, 0, -1);
}
paren_reg_immed_expr(val) ::= immediate(immed). {
		set_instruction(&val, 0, 0, 0, 0, 0, 0, 0, immed.u, -1);
}

%type optional_paren_reg_immed_expr {instruction}
optional_paren_reg_immed_expr(val) ::= LEFT_PAREN reg_immediate_expr(expr) RIGHT_PAREN. {
	set_instruction(&val, 0, 0, 0, 0, 0, 0, expr.low.rC, expr.high.u, -1);
}
optional_paren_reg_immed_expr(val) ::= reg_immediate_expr(expr). {
	set_instruction(&val, 0, 0, 0, 0, 0, 0, expr.low.rC, expr.high.u, -1);
}

%type reg_immediate_expr {instruction}
reg_immediate_expr(val) ::= REGISTER(reg) add_or_subtract(oper) immediate(immed). {
	m_word i;
	i.s = (oper->type == T_ADD_OPER) ? immed.s : -immed.s;
	set_instruction(&val, 0, 0, 0, 0, 0, 0, reg->data.u, i.u, -1);
}
reg_immediate_expr(val) ::= REGISTER(reg). {
	set_instruction(&val, 0, 0, 0, 0, 0, 0, reg->data.u, 0, -1);
}
reg_immediate_expr(val) ::= immediate(immed). {
	set_instruction(&val, 0, 0, 0, 0, 0, 0, 0, immed.u, -1);
}

%type data_code_line {instruction}
data_code_line(val) ::= data_load_instruction(inst) REGISTER(regA) EQUALS bracket_reg_immed_expr(expr). {
	set_instruction(&val, 0, 0, 0, inst->data.u, regA->data.u, 0, expr.low.rC, expr.high.u, inst->line_num);
}
data_code_line(val) ::= data_stor_instruction(inst) bracket_reg_immed_expr(expr) EQUALS REGISTER(regA). {
	set_instruction(&val, 0, 0, 0, inst->data.u, regA->data.u, 0, expr.low.rC, expr.high.u, inst->line_num);
}

%type bracket_reg_immed_expr {instruction}
bracket_reg_immed_expr(val) ::= LEFT_BRACKET reg_immediate_expr(expr) RIGHT_BRACKET. {
	set_instruction(&val, 0, 0, 0, 0, 0, 0, expr.low.rC, expr.high.u, -1);
}
data_load_instruction(val) ::= LOAD(inst). {
	val = inst;
	inst->data.u = M_LOAD;
}
data_load_instruction(val) ::= RLOAD(inst). {
	val = inst;
	inst->data.u = M_RLOAD;
}

data_stor_instruction(val) ::= STOR(inst). {
	val = inst;
	inst->data.u = M_STORE;
}
data_stor_instruction(val) ::= RSTOR(inst). {
	val = inst;
	inst->data.u = M_RSTORE;
}

%type control_code_line {instruction}
control_code_line(val) ::= control_instruction(inst) control_address_expr(expr) COMMA REGISTER(regA) conditional_oper(oper) REGISTER(regB). {
	bool good_match = false;
	switch(inst->type) {
	case T_BRAE:  if(oper->type == T_EQUIV)            good_match = true; break;
	case T_BRANE: if(oper->type == T_NOT_EQUIV)        good_match = true; break;
	case T_BRAL:  if(oper->type == T_LESS)             good_match = true; break;
	case T_BRALE: if(oper->type == T_LESS_OR_EQUIV)    good_match = true; break;
	case T_BRAG:  if(oper->type == T_GREATER)          good_match = true; break;
	case T_BRAGE: if(oper->type == T_GREATER_OR_EQUIV) good_match = true; break;
	}
	if(good_match) {
		set_instruction(&val, 0, expr.low.hint, 0, inst->data.u, regA->data.u, regB->data.u, expr.low.rC, expr.high.u, inst->line_num);
	}
	else {
		STAGE_ERROR(regA->line_num, "The operator, \"%s\", doesn't match the instruction", oper->token_str);
	}
}

control_instruction(val) ::= BRAE(inst). {
	val = inst;
	inst->data.u = M_BRAE;
}
control_instruction(val) ::= BRANE(inst). {
	val = inst;
	inst->data.u = M_BRANE;
}
control_instruction(val) ::= BRAL(inst). {
	val = inst;
	inst->data.u = M_BRAL;
}
control_instruction(val) ::= BRALE(inst). {
	val = inst;
	inst->data.u = M_BRALE;
}
control_instruction(val) ::= BRAG(inst). {
	val = inst;
	inst->data.u = M_BRAG;
}
control_instruction(val) ::= BRAGE(inst). {
	val = inst;
	inst->data.u = M_BRAGE;
}

%type control_address_expr {instruction}
control_address_expr(val) ::= LEFT_BRACKET reg_immediate_expr(expr) hint(h) RIGHT_BRACKET. {
	m_uword x = (h && h->data.u) ? 1 : 0;
	set_instruction(&val, 0, x, 0, 0, 0, 0, expr.low.rC, expr.high.u, -1);
}

hint(val) ::= COLON TRUE(t). {
	val = t;
}
hint(val) ::= COLON FALSE(f). {
	val = f;
}
hint(val) ::= . {
	val = NULL;
}

conditional_oper(val) ::= EQUIV(op). {
	val = op;
}
conditional_oper(val) ::= NOT_EQUIV(op). {
	val = op;
}
conditional_oper(val) ::= LESS(op). {
	val = op;
}
conditional_oper(val) ::= LESS_OR_EQUIV(op). {
	val = op;
}
conditional_oper(val) ::= GREATER(op). {
	val = op;
}
conditional_oper(val) ::= GREATER_OR_EQUIV(op). {
	val = op;
}

%type int_code_line {instruction}
int_code_line(val) ::= INT(inst) optional_paren_reg_immed_expr(expr). {
	set_instruction(&val, 0, 0, 0, M_INT, 0, 0, expr.low.rC, expr.high.u, inst->line_num);
}
%type int_return_code_line {instruction}
int_return_code_line(val) ::= IRET(inst). {
	set_instruction(&val, 0, 0, 0, M_IRET, 0, 0, 0, 0, inst->line_num);
}

%type neg_code_line {instruction}
neg_code_line(val) ::= NEG(inst) REGISTER(reg). {
	set_instruction(&val, 0, 0, 0, M_SUB, reg->data.u, 0, reg->data.u, 0, inst->line_num);
}
%type inv_code_line {instruction}
inv_code_line(val) ::= INV(inst) REGISTER(reg). {
	set_instruction(&val, 0, 0, 0, M_EXOR, reg->data.u, reg->data.u, 0, ~0U, inst->line_num);
}
%type inc_code_line {instruction}
inc_code_line(val) ::= INC(inst) REGISTER(reg). {
	set_instruction(&val, 0, 0, 0, M_ADD, reg->data.u, reg->data.u, 0, 1, inst->line_num);
}
%type dec_code_line {instruction}
dec_code_line(val) ::= DEC(inst) REGISTER(reg). {
	set_instruction(&val, 0, 0, 0, M_SUB, reg->data.u, reg->data.u, 0, 1, inst->line_num);
}
%type movr_code_line {instruction}
movr_code_line(val) ::= MOVR(inst) REGISTER(regA) EQUALS REGISTER(regB). {
	set_instruction(&val, 0, 0, 0, M_ADD, regA->data.u, regB->data.u, 0, 0, inst->line_num);
}
%type movi_code_line {instruction}
movi_code_line(val) ::= MOVI(inst) REGISTER(reg) EQUALS immediate(immed). {
	set_instruction(&val, 0, 0, 0, M_ADD, reg->data.u, 0, 0, immed.u, inst->line_num);
}
%type bra_code_line {instruction}
bra_code_line(val) ::= BRA(inst) control_address_expr(expr). {
	set_instruction(&val, 0, 1, 0, M_BRAE, 0, 0, expr.low.rC, expr.high.u, inst->line_num);
}
%type nop_code_line {instruction}
nop_code_line(val) ::= NOP(inst). {
	set_instruction(&val, 0, 0, 0, M_ADD, 0, 0, 0, 0, inst->line_num);
}
%type flush_code_line {instruction}
flush_code_line(val) ::= FLUSH(inst). {
	set_instruction(&val, 0, 1, 0, M_BRANE, 0, 0, M_REGISTER_PC, 0, inst->line_num);
	current_branch_is_flush = true;
}

%type immediate {m_word}
immediate(val) ::= number(num). {
	val = num;
}
immediate(val) ::= CHARACTER(ch). {
	val.u = (m_uword)ch->token_str[1];
}
immediate(val) ::= label_immediate(id). {
	val = id;
}
immediate(val) ::= identifier_immediate(id). {
	val = id;
}

%type label_immediate {m_word}
label_immediate(val) ::= LABEL(id). {
	if(pass == 2) {
		symbol *sym = symbol_table_search(labels, id->token_str);
		if(sym) {
			val = sym->value;
		}
		else {
			STAGE_ERROR(id->line_num, "The label, %s, is undefined", id->token_str);
		}
	}
}

%type identifier_immediate {m_word}
identifier_immediate(val) ::= IDENTIFIER(id). {
	if(pass == 2) {
		symbol *sym;
		if((sym = symbol_table_search(constants, id->token_str))) {
			val = sym->value;
		}
		else if((sym = symbol_table_search(variables, id->token_str))) {
			val = sym->value;
		} else {
			STAGE_ERROR(id->line_num, "Unknown identifier \"%s\"", id->token_str);
		}
	}
}

%type constant_immediate {m_word}
constant_immediate(val) ::= number(num). {
	val = num;
}
constant_immediate(val) ::= CHARACTER(ch). {
	val.u = (m_uword)ch->token_str[1];
	most_recent_const_immed_line_num = ch->line_num;
}

%type address_immediate {m_word}
address_immediate(val) ::= number(num). {
	val = num;
}

%type number {m_word}
number(val) ::= optional_sign(sign) NUMBER(num). {
	if(sign && sign->type == T_SUB_OPER) {
		val.s = -num->data.s;
	}
	else {
		val = num->data;
	}
	most_recent_const_immed_line_num = num->line_num;
}

optional_sign(val) ::= add_or_subtract(op). {
	val = op;
}
optional_sign(val) ::= . {
	val = NULL;
}

add_or_subtract(val) ::= ADD_OPER(add). {
	val = add;
}
add_or_subtract(val) ::= SUB_OPER(sub). {
	val = sub;
}
