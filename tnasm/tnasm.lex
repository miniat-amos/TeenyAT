%option noyywrap
%option case-insensitive
%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>

#include "../teenyat.h"

#include "token.h"
#include "tnasm.h"
#include "symbol_tables.h"
#include "util.h"
#include "list.h"
#include "listing.h"
#include "blocks.h"

extern void *ParseAlloc(void *(*mallocProc)(size_t));
extern void Parse(void *yyp, int yymajor, Token *yyminor);
extern void ParseTrace(FILE *TraceFILE, char *zTracePrompt);
extern void ParseFree(void *p, void (*freeProc)(void*));

void *parser;

int line_num;

FILE *in_file = NULL;
FILE *bin_file = NULL;
char *base_filename = NULL;
char *bin_filename = NULL;
int seperator;

/*
 * ascii_to_value()
 *
 * converts from string to number given a base for the original string
 */
m_sword ascii_to_value(char *s, char base) {

	const char table[] = "0123456789abcdef";
	m_sword result = 0;

	while(*s != '\0') {
		if(*s != '_') {
			result *= base;
			result += (strchr(table, tolower(*s)) - table) / sizeof(char);
		}
		s++;
	}

	return result;
}

/*
 * get_yystring()
 *
 * Returns a pointer to a newly allocated copy of the current lexical token, yytext.
 */
char *get_yystring() {

	char *s;

	SAFE_MALLOC(s, char *, sizeof(char) * (yyleng + 1));
	memcpy(s, yytext, yyleng);
	s[yyleng] = '\0';

	return s;
}

/*
 * token_new()
 *
 * Returns a pointer to a new allocated Token of the given type based on the current
 * lexical token, yytext.
 */
Token *token_new(int type) {

	Token *t;

	SAFE_MALLOC(t, Token *, sizeof(Token));
	t->type = type;
	t->token_str = get_yystring();
	t->line_num = line_num;

	return t;
}

/*
 * print_program_usage()
 *
 * Prints tnasm usage and command argument details.
 */
void print_program_usage() {

	printf("Usage:  tnasm [-dlptv] [-o output_dir] input_file\n");
	printf("\n");
	printf("Options:\n");
	printf("    -d     Dump hexadecimal of generated binary to console on successful assembly\n");
	printf("    -l     Generate a listing file... file.lst\n");
	printf("    -p     Print parser information during assembly\n");
	printf("    -t     Print tokenizer (lexer) information during assembly\n");
	printf("    -v     Print verbose information during assembly, including -dlpt and more.\n");
	printf("\n");
	printf("    -o     When output_dir is provided, all generated files are placed in that directory.\n");
	printf("           Otherwise, those files are generated in the same directory as the input_file.\n");
	printf("\n");
	printf("Example:  tnasm -l -p -o ~/bins program.asm\n");
	printf("    Prints parser information and generates the binary and listing (program.lst) in the ~/bins directory.\n");
	printf("\n");

	return;
}

/*
 * generate_binary()
 *
 * produces the TeenyAT binary once the 2nd parser stage completes successfully.
 */
void generate_binary() {

	int error;
	m_uword block_num;
	list_node *n;
	m_ubyte value = 0;

	bin_file = fopen(bin_filename, "wb");
	if(!bin_file) {
		ERROR("Could not open binary file, \"%s\", for output", bin_filename);
	}

	/* seek and the write a byte of size 1 to grow the file */
	error = fseek(bin_file, M_FLASH_SIZE_IN_BYTES -1, SEEK_SET);
	if(error < 0) {
		ERROR("Could not seek to end of file!");
	}

	error = fwrite(&value, sizeof(value), 1, bin_file);
	if(error < 0) {
		ERROR("Could not write to end of file!");
	}

	INFO_HEX("\nDumping the hex of the generated binary...\n\n");

	n = blocks->head;
	for(block_num = 0; block_num < blocks->size; block_num++) {

		block *b = (block *)(n->item);
		list_node *node = b->words->head;

		m_uword start = b->address;
		m_uword len = (m_uword)b->words->size;
		m_uword end = start + len - 1;

		/*
		 * Sanity check ranges. Must be in range of flash, ensure no roll-over on
		 * calculating end.
		 */
		if(start < M_FLASH_START || start > M_FLASH_END) {
			ERROR("Starting address out of flash range: 0x%X\n", start);
		}

		if(end < M_FLASH_START || end > M_FLASH_END) {
			ERROR("Ending address out of flash range: 0x%X\n", end);
		}

		/*
		 * Catches roll-over and empty blocks, since len == 0 causes end to
		 * be smaller than start. A block of length one has the same
		 * start and end address since these are inclusive ranges.
		 * Thus, no greater than or equal here.
		 */
		if(start > end) {
			const char *msg = len ? "Internal Rollover" : "Empty Block";
			ERROR("%s at start: 0x%X, length: %u\n", msg, start, len);
		}

		/* Translate address into file offset and set current file pointer to it */
		start -= M_FLASH_START;

		error = fseek(bin_file, start, SEEK_SET);
		if(error < 0) {
			ERROR("Could not set file pointer to flash offset: %u", start);
		}

		while(node) {
			block_word *word = (block_word *)(node->item);
			fwrite(&(word->data), sizeof(m_uword), 1, bin_file);
			INFO_HEX("| 0x%08X |\n", word->data);
			node = node->next;
		}

		n = n->next;
	}

	return;
}

/*
 * parse_options()
 *
 * Processes command arguments and opens assembly file.
 */
void parse_options(int argc, char *argv[]) {

	extern char *optarg;
	extern int optind;

	char *extension;
	char *c;
	char *filename;
	char *base;
	char *outpath = NULL;

	int option;
	int d_cnt = 0;
	int l_cnt = 0;
	int o_cnt = 0;
	int p_cnt = 0;
	int t_cnt = 0;
	int v_cnt = 0;

	if(base_filename || bin_filename) {
		ERROR("Base and/or binary file names are set inappropriately");
	}

	while((option = getopt(argc, argv, "dlptvo:")) != -1) {
		switch((char)option) {
		case 'd':
			option_dump_hex = true;
			d_cnt++;
			break;
		case 'l':
			option_generate_listing = true;
			l_cnt++;
			break;
		case 'p':
			option_parser_info = true;
			p_cnt++;
			break;
		case 't':
			option_tokenizer_info = true;
			t_cnt++;
			break;
		case 'v':
			option_dump_hex       = true;
			option_parser_info    = true;
			option_tokenizer_info = true;
			option_verbose_info   = true;
			option_generate_listing = true;
			v_cnt++;
			break;
		case 'o':
			outpath = optarg;
			o_cnt++;
			break;
		default:
			print_program_usage();
			ERROR("Unknown option : %c\n", (char)option);
			exit(1);
			break;
		}
	}

/*
 * Check for basic error conditions
 */

	if(optind != (argc - 1)) {
		print_program_usage();
		exit(1);
	}

	if(d_cnt > 1 || l_cnt > 1 || p_cnt > 1 || t_cnt > 1 || v_cnt > 1 || o_cnt > 1) {
		print_program_usage();
		ERROR("Not sure what is intended by repeat flag usage...\n");
	}

/*
 * Open ASM and derive base file name
 */

	in_file = fopen(argv[optind], "rb");
	if(!in_file) {
		ERROR("Could not open file :\"%s\"", argv[optind]);
	}

	extension = argv[optind] + strlen(argv[optind]) - 4;
	c = extension;
	while(*c) {
		*c = tolower((int)*c);
		c++;
	}
	if(strcmp(extension, ".asm")) {
		ERROR("\".asm\" file extension expected");
	}

	if(outpath == NULL) {
		filename = argv[optind];
		SAFE_MALLOC(base_filename, char *, sizeof(char) * (strlen(filename) - 4 + 1));
		SAFE_MALLOC(bin_filename, char *, sizeof(char) * (strlen(filename) + 1));
		memcpy(base_filename, filename, strlen(filename) - 4);
		memcpy(bin_filename, filename, strlen(filename));
		strcpy(strrchr(bin_filename, '.'), ".bin");
	}
	else {
		/* remove path from filename */
		filename = strrchr(argv[optind], seperator);
		if(filename) {
			filename++; /* skip the leading seperator */
		}
		else {
			filename = argv[optind];
		}

		if(strrchr(outpath, seperator) == (outpath + strlen(outpath) - 1)) {
			/* output_dir ends with path seperator */
			SAFE_MALLOC(base_filename, char *, sizeof(char) * (strlen(outpath) + strlen(filename) - 4 + 1));
			base = base_filename;
			memcpy(base, outpath, strlen(outpath));
			base += strlen(outpath);
			memcpy(base, filename, strlen(filename) - 4);
		}
		else {
			/* output_dir does NOT end with path seperator */
			SAFE_MALLOC(base_filename, char *, sizeof(char) * (strlen(outpath) + 1 + strlen(filename) - 4 + 1)); /* middle +1 for seperator */
			base = base_filename;
			memcpy(base, outpath, strlen(outpath));
			base += strlen(outpath);
			*base = (char)seperator;
			base++;
			memcpy(base, filename, strlen(filename) - 4);
		}
		SAFE_MALLOC(bin_filename, char *, sizeof(char) * (strlen(base_filename) + 4 + 1));
		base = bin_filename;
		memcpy(base, base_filename, strlen(base_filename));
		base += strlen(base_filename);
		strcpy(base, ".bin");
	}

	return;
}

void cleanup_files() {

	INFO_VERBOSE("Cleaning up files... ");

	if(base_filename) {
		free(base_filename);
	}
	if(in_file) {
		fclose(in_file);
	}

	if(bin_filename) {
		free(bin_filename);
	}

	INFO_VERBOSE("Done\n");

	return;
}

void cleanup_parser() {

	INFO_VERBOSE("Deallocating parser... ");
	ParseFree(parser, free);
	INFO_VERBOSE("Done\n");

	return;
}

void cleanup_tokens() {

	INFO_VERBOSE("Cleaning up token list... ");
	list_delete(tokens);
	INFO_VERBOSE("Done\n");

	return;
}

void cleanup_symbols() {

	INFO_VERBOSE("Cleaning up labels symbol table... ");
	list_delete(labels);
	INFO_VERBOSE("Done\n");

	INFO_VERBOSE("Cleaning up varibales symbol table... ");
	list_delete(variables);
	INFO_VERBOSE("Done\n");

	INFO_VERBOSE("Cleaning up constants symbol table... ");
	list_delete(constants);
	INFO_VERBOSE("Done\n");

	return;
}

%}

HEX_DIGIT [a-f0-9]
DEC_DIGIT [0-9]
OCT_DIGIT [0-7]
BIN_DIGIT [01]

ALPHA    [a-z]
ALPHA_NUM [a-z0-9]
ALPHA_NUM_UNDER [a-z0-9_]
%%

[ \t\b\v\r]+    { /* whitespace */
		if(((list *)tokens)->tail && ((Token *)(((list *)tokens)->tail->item))->type == T_EOL) {
			INFO_TOKEN("\t");
		}
	}

"#".*    {
		/* whitespace */
		INFO_TOKEN("# Line comment ");
	}

"<<<"((">"|">>")?[^>]+)*">>>"   {
		/* whitespace */
		char *c;
		INFO_TOKEN("<<< Block comment ");
		c = yytext;
		while(*c) {
			if(*c == '\n') {
				line_num++;
				INFO_TOKEN("\n%5d:\t    Block comment ", line_num);
			}
			c++;
		}
		INFO_TOKEN(">>> ");
	}

add    { token_list_append(token_new(T_ADD)); INFO_TOKEN("T_ADD "); }
sub    { token_list_append(token_new(T_SUB)); INFO_TOKEN("T_SUB "); }
mult    { token_list_append(token_new(T_MULT)); INFO_TOKEN("T_MULT "); }
div    { token_list_append(token_new(T_DIV)); INFO_TOKEN("T_DIV "); }
mod    { token_list_append(token_new(T_MOD)); INFO_TOKEN("T_MOD "); }

and    { token_list_append(token_new(T_AND)); INFO_TOKEN("T_AND "); }
or    { token_list_append(token_new(T_OR)); INFO_TOKEN("T_OR "); }
exor    { token_list_append(token_new(T_EXOR)); INFO_TOKEN("T_EXOR "); }
shl    { token_list_append(token_new(T_SHL)); INFO_TOKEN("T_SHL "); }
shr    { token_list_append(token_new(T_SHR)); INFO_TOKEN("T_SHR "); }

load    { token_list_append(token_new(T_LOAD)); INFO_TOKEN("T_LOAD "); }
stor    { token_list_append(token_new(T_STOR)); INFO_TOKEN("T_STOR "); }
rload    { token_list_append(token_new(T_RLOAD)); INFO_TOKEN("T_RLOAD "); }
rstor    { token_list_append(token_new(T_RSTOR)); INFO_TOKEN("T_RSTOR "); }

brae    { token_list_append(token_new(T_BRAE)); INFO_TOKEN("T_BRAE "); }
brane    { token_list_append(token_new(T_BRANE)); INFO_TOKEN("T_BRANE "); }
bral    { token_list_append(token_new(T_BRAL)); INFO_TOKEN("T_BRAL "); }
brale    { token_list_append(token_new(T_BRALE)); INFO_TOKEN("T_BRALE "); }
brag    { token_list_append(token_new(T_BRAG)); INFO_TOKEN("T_BRAG "); }
brage    { token_list_append(token_new(T_BRAGE)); INFO_TOKEN("T_BRAGE "); }

int    { token_list_append(token_new(T_INT)); INFO_TOKEN("T_INT "); }
iret    { token_list_append(token_new(T_IRET)); INFO_TOKEN("T_IRET "); }

neg    { token_list_append(token_new(T_NEG)); INFO_TOKEN("T_NEG "); }
inv    { token_list_append(token_new(T_INV)); INFO_TOKEN("T_INV "); }
inc    { token_list_append(token_new(T_INC)); INFO_TOKEN("T_INC "); }
dec    { token_list_append(token_new(T_DEC)); INFO_TOKEN("T_DEC "); }
movr    { token_list_append(token_new(T_MOVR)); INFO_TOKEN("T_MOVR "); }
movi    { token_list_append(token_new(T_MOVI)); INFO_TOKEN("T_MOVI "); }
bra    { token_list_append(token_new(T_BRA)); INFO_TOKEN("T_BRA "); }
nop    { token_list_append(token_new(T_NOP)); INFO_TOKEN("T_NOP "); }
flush    { token_list_append(token_new(T_FLUSH)); INFO_TOKEN("T_FLUSH "); }

\.mode    { token_list_append(token_new(T_MODE_DIRECTIVE)); INFO_TOKEN("T_MODE_DIRECTIVE "); }
nop_delay    { token_list_append(token_new(T_NOP_DELAY)); INFO_TOKEN("T_NOP_DELAY "); }
on    { token_list_append(token_new(T_ON)); INFO_TOKEN("T_ON "); }
off    { token_list_append(token_new(T_OFF)); INFO_TOKEN("T_OFF "); }

\.const(ant)?    { token_list_append(token_new(T_CONST_DIRECTIVE)); INFO_TOKEN("T_CONST_DIRECTIVE "); }
\.var(iable)?    { token_list_append(token_new(T_VAR_DIRECTIVE)); INFO_TOKEN("T_VAR_DIRECTIVE "); }
\.addr(ess)?    { token_list_append(token_new(T_ADDR_DIRECTIVE)); INFO_TOKEN("T_ADDR_DIRECTIVE "); }

t|"sad_but_true"    { /* true hint */
		Token *t = token_new(T_TRUE);
		t->data.u = 1;
		token_list_append(t);
		INFO_TOKEN("T_TRUE ");
	}
f    { /* false hint */
		Token *t = token_new(T_FALSE);
		t->data.u = 0;
		token_list_append(t);
		INFO_TOKEN("T_FALSE ");
	}

r{DEC_DIGIT}+ |
rsp |
sp |
pc    {
		/* Register */
		Token *t = token_new(T_REGISTER);
		switch(yytext[1]) {
		case 's':
		case 'S':
			/* rsp */
			t->data.u = 253;
			break;
		case 'p':
		case 'P':
			/* sp */
			t->data.u = 254;
			break;
		case 'c':
		case 'C':
			/* pc */
			t->data.u = 255;
			break;
		default:
			t->data.u = ascii_to_value(yytext + 1, 10);
			if(t->data.u > 255) {
				STAGE_ERROR(line_num, "No such register, \"%s\"", yytext);
			}
			break;
		}
		token_list_append(t);
		INFO_TOKEN("T_REGISTER ");
	}

{ALPHA}{ALPHA_NUM_UNDER}*    { token_list_append(token_new(T_IDENTIFIER)); INFO_TOKEN("T_IDENTIFIER "); }

"!"{ALPHA}{ALPHA_NUM_UNDER}*    { token_list_append(token_new(T_LABEL)); INFO_TOKEN("T_LABEL "); }

"__"    { token_list_append(token_new(T_PREDICATE)); INFO_TOKEN("T_PREDICATE "); }

"="    { token_list_append(token_new(T_EQUALS)); INFO_TOKEN("T_EQUALS "); }
"=="    { token_list_append(token_new(T_EQUIV)); INFO_TOKEN("T_EQUIV "); }
"!="    { token_list_append(token_new(T_NOT_EQUIV)); INFO_TOKEN("T_NOT_EQUIV "); }
"<"    { token_list_append(token_new(T_LESS)); INFO_TOKEN("T_LESS "); }
"<="    { token_list_append(token_new(T_LESS_OR_EQUIV)); INFO_TOKEN("T_LESS_OR_EQUIV "); }
">"    { token_list_append(token_new(T_GREATER)); INFO_TOKEN("T_GREATER "); }
">="    { token_list_append(token_new(T_GREATER_OR_EQUIV)); INFO_TOKEN("T_GREATER_OR_EQUIV "); }

"("    { token_list_append(token_new(T_LEFT_PAREN)); INFO_TOKEN("T_LEFT_PAREN "); }
")"    { token_list_append(token_new(T_RIGHT_PAREN)); INFO_TOKEN("T_RIGHT_PAREN "); }
\'.\'    { token_list_append(token_new(T_CHARACTER)); INFO_TOKEN("T_CHARACTER "); }

"-"    { token_list_append(token_new(T_SUB_OPER)); INFO_TOKEN("T_SUB_OPER "); }
"+"    { token_list_append(token_new(T_ADD_OPER)); INFO_TOKEN("T_ADD_OPER "); }
"*"    { token_list_append(token_new(T_MULT_OPER)); INFO_TOKEN("T_MULT_OPER "); }
"/"    { token_list_append(token_new(T_DIV_OPER)); INFO_TOKEN("T_DIV_OPER "); }
"%"    { token_list_append(token_new(T_MOD_OPER)); INFO_TOKEN("T_MOD_OPER "); }
"&"    { token_list_append(token_new(T_AND_OPER)); INFO_TOKEN("T_AND_OPER "); }
"|"    { token_list_append(token_new(T_OR_OPER)); INFO_TOKEN("T_OR_OPER "); }
"^"    { token_list_append(token_new(T_EXOR_OPER)); INFO_TOKEN("T_EXOR_OPER "); }
"<<"    { token_list_append(token_new(T_SHL_OPER)); INFO_TOKEN("T_SHL_OPER "); }
">>"    { token_list_append(token_new(T_SHR_OPER)); INFO_TOKEN("T_SHR_OPER "); }

\"[^"\n]+\"    {
		/* double-quoted string */
		Token *t;
		yytext++;
		t = token_new(T_STRING);
		t->token_str[strlen(t->token_str) - 1] = '\0';
		token_list_append(t);
		INFO_TOKEN("T_STRING ");
	}

\'[^'\n]+\'    {
		/* single-quoted "packed" string */
		Token *t;
		yytext++;
		t = token_new(T_PACKED_STRING);
		t->token_str[strlen(t->token_str) - 1] = '\0';
		token_list_append(t);
		INFO_TOKEN("T_PACKED_STRING ");
	}

{DEC_DIGIT}+(_*{DEC_DIGIT})*    {
		/* signed decimal number */
		Token *t = token_new(T_NUMBER);
		t->data.s = ascii_to_value(t->token_str, 10);
		token_list_append(t);
		INFO_TOKEN("T_NUMBER ");
	}

0x(_*{HEX_DIGIT})+ |
0d(_*{DEC_DIGIT})+ |
0c(_*{OCT_DIGIT})+ |
0b(_*{BIN_DIGIT})+    {
		/* unsigned number */
		Token *t;
		int base = -1;
		switch(yytext[1]) {
		case 'x':
		case 'X':
			base = 16;
			break;
		case 'd':
		case 'D':
			base = 10;
			break;
		case 'c':
		case 'C':
			base = 8;
			break;
		case 'b':
		case 'B':
			base = 2;
			break;
		default:
			STAGE_ERROR(line_num, "Unknown numeric-base identifier '%c'", yytext[1]);
			break;
		}
		t = token_new(T_NUMBER);
		yytext += 2;
		t->data.s = ascii_to_value(yytext, base);
		token_list_append(t);
		INFO_TOKEN("T_NUMBER ");
	}

":"    { token_list_append(token_new(T_COLON)); INFO_TOKEN("T_COLON "); }
"["    { token_list_append(token_new(T_LEFT_BRACKET)); INFO_TOKEN("T_LEFT_BRACKET "); }
"]"    { token_list_append(token_new(T_RIGHT_BRACKET)); INFO_TOKEN("T_RIGHT_BRACKET "); }
","    { token_list_append(token_new(T_COMMA)); INFO_TOKEN("T_COMMA "); }

\n    {
		/* New line */
		token_list_append(token_new(T_EOL));
		line_num++;
		INFO_TOKEN(" \n%5d:\t", line_num);
	}

.    {
		/* BAD TOKEN */
		STAGE_ERROR(line_num, "Unrecognized character : %s", yytext);
	}
%%

int main(int argc, char **argv) {

	list_node *node;
	list_node *prev_node;
	Token *eol;

#		if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
			seperator = (int)'\\';
#		else
			seperator = (int)'/';
#		endif

	atexit(cleanup_files);
	parse_options(argc, argv);

	INFO_VERBOSE("***********************************************\n");
	INFO_VERBOSE("*  Verbose output enabled.  Get ready... :-)  *\n");
	INFO_VERBOSE("***********************************************\n");

	INFO_VERBOSE("Initializing tokens list... ");
	token_list_init();
	atexit(cleanup_tokens);
	INFO_VERBOSE("Done\n");

	INFO_TOKEN("\nBeginning lexical analysis...\n");
	line_num = 1;
	INFO_TOKEN("\n%5d:\t", line_num);
	yyin = in_file;
	yylex();
	yylex_destroy();
	INFO_TOKEN("\n");
	INFO_TOKEN("\nCompleted lexical analysis\n");

	/* Add an extra EOL in case the source is lacking one */
	if(tokens->tail && ((Token *)(tokens->tail->item))->type != T_EOL) {
		SAFE_MALLOC(eol, Token *, sizeof(Token));
		eol->type = T_EOL;
		SAFE_MALLOC(eol->token_str, char *, sizeof(char) * 2);
		eol->token_str[0] = '\n';
		eol->token_str[1] = '\0';
		eol->line_num = line_num;
		token_list_append(eol);
	}

	if(stage_error_cnt > 0) {
		exit(1);
	}

	atexit(cleanup_listing);
	INFO_VERBOSE("Initializing listing structures... ");
	listing_init(in_file, base_filename);
	INFO_VERBOSE("Done\n");

	atexit(cleanup_symbols);

	INFO_VERBOSE("Initializing labels symbol table... ");
	symbol_table_init(labels);
	INFO_VERBOSE("Done\n");

	INFO_VERBOSE("Initializing variables symbol table... ");
	symbol_table_init(variables);
	INFO_VERBOSE("Done\n");

	INFO_VERBOSE("Initializing constants symbol table... ");
	symbol_table_init(constants);
	INFO_VERBOSE("Done\n");

	INFO_PARSER("\nBeginning syntactical analysis...\n\n");
	parser = ParseAlloc(malloc);
	atexit(cleanup_parser);

	for(pass = 1; pass <= 2; pass++) {
		address = (m_word)M_FLASH_START;
		inline_variables = false;
		variable_address = (m_word)M_RAM_START;

		switch(pass) {
		case 1:
			INFO_PARSER("--------------- First parser pass ---------------\n");
			if(option_verbose_info) {
				ParseTrace(stdout, "");
			}
			break;
		case 2:
			if(option_verbose_info) {
				ParseTrace(NULL, NULL);
			}
			if(option_parser_info) {
				symbol_table_print(constants, "constants");
				symbol_table_print(variables, "variables");
				symbol_table_print(labels, "labels");
			}
			INFO_PARSER("--------------- Second parser pass ---------------\n\n");

			atexit(cleanup_blocks);
			INFO_VERBOSE("Initializing blocks tree... ");
			blocks_init();
			block_new(address.u);
			INFO_VERBOSE("Done\n");

			break;
		default:
			ERROR("Unknown parser pass, %d\n", pass);
			break;
		}

		node = tokens->head;
		prev_node = tokens->tail;

		while(node) {
			if(pass == 1 && ((Token *)(prev_node->item))->type == T_EOL && node != tokens->tail) {
				INFO_VERBOSE("==== Line %d ====\n", ((Token *)(node->item))->line_num);
			}
			Parse(parser, ((Token *)(node->item))->type, (Token *)(node->item));
			prev_node = node;
			node = node->next;
		}
		Parse(parser, 0, NULL);

		if(stage_error_cnt > 0) {
			exit(1);
		}
	}

	INFO_PARSER("Completed syntactical analysis\n");

	generate_binary();
	generate_listing();

	exit(0);
}
