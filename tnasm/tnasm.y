%include {
	#include <assert.h>
	#include <string.h>
	#include <stdbool.h>

	#include "token.h"
	#include "tnasm.h"
	#include "symbol_tables.h"
	#include "util.h"
	#include "../teenyat.h"
	#include "blocks.h"

	/*
	 * This unusual variable is used to push the line number of certain
	 * constant_immediate up the parse tree to the appropriate raw_value.
	 * Basically, it's used at a low Token level to capture the line
	 * number before the rules begin passing only "tny_uword"s up.  The
	 * higher rule can then use it to determine the line number.  This
	 * data is really only utilized when generating listings.
	 */
	tny_uword most_recent_const_immed_line_num;

	/* Use a bogus line_num until matching rule for a code line.
	 * These lines will always have an instruction Token* from which the correct
	 * line number can be determined.
	 */
}

%start_symbol tnasm_program
%token_prefix T_

%token_type {Token *}

%syntax_error {
	Token *t = yyminor;//.yy0;
	switch(t->type) {
	case T_EOL:
		STAGE_ERROR(t->line_num, "Unexpected end of line");
		break;
	default:
		STAGE_ERROR(t->line_num, "Unexpected %s token, \"%s\"", yyTokenName[t->type], t->token_str);
		break;
	}
}

tnasm_program ::= eols lines.
tnasm_program ::= lines.

eols ::= eols EOL.
eols ::= EOL.
eols ::= error EOL.

lines ::= lines label_line eols.
lines ::= lines code_line eols. {
		address++;
	}
lines ::= .

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


%type code_line {tm_instruction}
code_line(val) ::= ALPHA char_or_string(A). {
		val.encoding.alpha.opcode = TM_OPCODE_ALPHA;
		val.encoding.alpha.rsvd = 0;
		val.line_num = A->line_num;

		if(A->type == T_CHARACTER) {
			val.encoding.alpha.letter = A->token_str[1];
			if(pass == 2) {
				block_new_word(val.line_num, val.encoding.word.u);
			}
		}
		else if(A->type == T_STRING) {
			int i;
			int len = strlen(A->token_str);
			for(i = 0; i < len; i++) {
				val.encoding.alpha.letter = A->token_str[i];
				if(pass == 2) {
					block_new_word(val.line_num, val.encoding.word.u);
				}
			}
			address += len - 1;
		}
	}


char_or_string(val) ::= STRING(A). {
		val = A;
	}
char_or_string(val) ::= CHARACTER(A). {
		val = A;
	}


code_line(val) ::= cmp_or_or(A) blank_or_letter(B). {
		val.line_num = A->line_num;
		val.encoding.cmp.opcode = TM_OPCODE_CMP;
		val.encoding.cmp.or = (A->type == T_OR);
		val.encoding.cmp.rsvd = 0;
		if(B->type == T_BLANK) {
			val.encoding.cmp.blank = 1;
			val.encoding.cmp.letter = 0;
		}
		else {
		val.encoding.cmp.blank = 0;
		val.encoding.cmp.letter = B->token_str[1];
		}

		if(pass == 2) {
			block_new_word(val.line_num, val.encoding.word.u);
		}
	}


cmp_or_or(val) ::= CMP(A). {
		val = A;
	}
cmp_or_or(val) ::= OR(A). {
		val = A;
	}


blank_or_letter(val) ::= BLANK(A). {
		val = A;
	}
blank_or_letter(val) ::= CHARACTER(A). {
		val = A;
	}


code_line(val) ::= branch_instruction(A) LABEL(id). {
		val.line_num = A->line_num;
		if(pass == 2) {
			symbol *sym = symbol_table_search(labels, id->token_str);
			if(sym) {
				val.encoding.bra.addr = sym->value.u;
			}
			else {
				STAGE_ERROR(id->line_num, "The label, %s, is undefined", id->token_str);
			}
		}

		if(pass == 2) {
			switch(A->type) {
			case T_BRAE:
				val.encoding.generic.opcode = TM_OPCODE_BRAE;
				break;
			case T_BRANE:
				val.encoding.generic.opcode = TM_OPCODE_BRANE;
				break;
			case T_BRA:
				val.encoding.generic.opcode = TM_OPCODE_BRA;
				break;
			}
			val.line_num = id->line_num;

			block_new_word(val.line_num, val.encoding.word.u);
		}
	}


branch_instruction(val) ::= BRAE(A). {
		val = A;
	}
branch_instruction(val) ::= BRANE(A). {
		val = A;
	}
branch_instruction(val) ::= BRA(A). {
		val = A;
	}


code_line(val) ::= direction_instruction(A). {
		val.encoding.right.opcode = TM_OPCODE_RIGHT;
		val.line_num = A->line_num;
		val.encoding.right.right = (A->type == T_RIGHT);
		val.encoding.right.rsvd = 0;

		if(pass == 2) {
			block_new_word(val.line_num, val.encoding.word.u);
		}
	}


direction_instruction(val) ::= LEFT(A). {
		val = A;
	}
direction_instruction(val) ::= RIGHT(A). {
		val = A;
	}


code_line(val) ::= stop_instruction(A). {
		val.encoding.halt.opcode = TM_OPCODE_HALT;
		val.line_num = A->line_num;
		val.encoding.halt.halt = (A->type == T_HALT);
		val.encoding.halt.rsvd = 0;

		if(pass == 2) {
			block_new_word(val.line_num, val.encoding.word.u);
		}
	}


stop_instruction(val) ::= HALT(A). {
		val = A;
	}
stop_instruction(val) ::= FAIL(A). {
		val = A;
	}


code_line(val) ::= DRAW blank_or_letter(A). {
		val.encoding.draw.opcode = TM_OPCODE_DRAW;
		val.line_num = A->line_num;
		val.encoding.draw.blank = (A->type == T_BLANK);
		val.encoding.draw.rsvd = 0;

		if(A->type == T_BLANK) {
			val.encoding.draw.letter = 0;
		}
		else {
			val.encoding.draw.letter = A->token_str[1];
		}

		if(pass == 2) {
			block_new_word(val.line_num, val.encoding.word.u);
		}
	}
