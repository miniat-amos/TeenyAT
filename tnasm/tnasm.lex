%option noyywrap
%option outfile="lexer.c"
%option case-insensitive
%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
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

	printf("Usage:  tnasm [-d|--dump] [-l|--listing] [-p|--parser-info] [-t|--token-info] [-v|--verbose] [(-o|--output-dir) path] input_file\n");
	printf("\n");
	printf("Options:\n");
	printf("    -d, --dump           Dump hexadecimal of generated binary to console on\n");
	printf("                         successful assembly.\n");
	printf("    -h, --help           Print this help information.\n");
	printf("    -l, --listing        Generate a listing file with the \".lst\" extension.\n");
	printf("    -p, --parser-info    Print parser information during assembly.\n");
	printf("    -t, --token-info     Print tokenizer (lexer) information during assembly.\n");
	printf("    -v, --verbose        Print verbose information during assembly, including\n");
	printf("                         \"-d -l -p -t\" and more.\n");
	printf("\n");
	printf("    -o, --output-dir     All generated files are placed in the path.  Otherwise,\n");
	printf("                         those files are generated in the same directory as the\n");
	printf("                         input_file.\n");
	printf("\n");
	printf("Example:  tnasm -l -p -o ~/bins program.asm\n");
	printf("    Prints parser information and generates the binary and listing (program.lst) in the ~/bins directory.\n");
	printf("\n");

	return;
}

/*
 * generate_binary()
 *
 * produces the Turing Machine binary once the 2nd parser stage completes successfully.
 */
void generate_binary() {

	int block_num;
	list_node *n;
	int block_cnt;

	bin_file = fopen(bin_filename, "wb");
	if(!bin_file) {
		ERROR("Could not open binary file, \"%s\", for output", bin_filename);
	}

	block_cnt = blocks->size;

	INFO_HEX("\nDumping the hex of the generated binary...\n\n");

	n = blocks->head;
	for(block_num = 0; block_num < block_cnt; block_num++) {
		block_word *word;
		int block_word_cnt;
		block *b = (block *)(n->item);
		list_node *node = b->words->head;

		INFO_HEX("+==========+\n");

		block_word_cnt = b->words->size;

		while(node) {
			word = (block_word *)(node->item);
			fwrite(&(word->data), sizeof(tm_encoding), 1, bin_file);
			INFO_HEX("|  0x%04X  |\n", word->data);
			node = node->next;
		}

		INFO_HEX("+==========+\n");
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

	char *extension;
	char *c;
	char *filename;
	char *base;
	char *outpath = NULL;

	int d_cnt = 0;
	int l_cnt = 0;
	int o_cnt = 0;
	int p_cnt = 0;
	int t_cnt = 0;
	int v_cnt = 0;

	char *optarg;
	int optind = 1;
	char option;

	if(base_filename || bin_filename) {
		ERROR("Base and/or binary file names are set inappropriately");
	}

	while(optind < argc && (optarg = argv[optind]) && optarg[0] == '-') {

//		option = optarg[1];

		if(strcmp(optarg, "-d") == 0 || strcmp(optarg, "--dump") == 0) {
			option_dump_hex = true;
			d_cnt++;
		}
		else if(strcmp(optarg, "-h") == 0 || strcmp(optarg, "--help") == 0) {
			print_program_usage();
			exit(EXIT_SUCCESS);
		}
		else if(strcmp(optarg, "-l") == 0 || strcmp(optarg, "--listing") == 0) {
			option_generate_listing = true;
			l_cnt++;
		}
		else if(strcmp(optarg, "-p") == 0 || strcmp(optarg, "--parser-info") == 0) {
			option_parser_info = true;
			p_cnt++;
		}
		else if(strcmp(optarg, "-t") == 0 || strcmp(optarg, "--token-info") == 0) {
			option_tokenizer_info = true;
			t_cnt++;
		}
		else if(strcmp(optarg, "-v") == 0 || strcmp(optarg, "--verbose") == 0) {
			option_dump_hex       = true;
			option_parser_info    = true;
			option_tokenizer_info = true;
			option_verbose_info   = true;
			option_generate_listing = true;
			v_cnt++;
		}
		else if(strcmp(optarg, "-o") == 0 || strcmp(optarg, "--output") == 0) {
			optind++;
			if(optind < argc) {
				outpath = argv[optind];
				o_cnt++;
			}
			else {
				print_program_usage();
				ERROR("Incomplete option : %s expects output filename\n", optarg);
				exit(EXIT_FAILURE);
			}
		}
		else {
			print_program_usage();
			ERROR("Unknown option : %s\n", optarg);
			exit(EXIT_FAILURE);
			break;
		}

		optind++;
	}

/*
 * Check for basic error conditions
 */

	if(optind != (argc - 1)) {
		print_program_usage();
		exit(EXIT_FAILURE);
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
	if(bin_file) {
		fclose(bin_file);
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

	return;
}

%}

%%

[ \t\b\v\r]+    { /* whitespace */
		if(((list *)tokens)->tail && ((Token *)(((list *)tokens)->tail->item))->type == T_EOL) {
			INFO_TOKEN("\t");
		}
	}

";".*    {
		/* whitespace */
		INFO_TOKEN("; Line comment ");
	}


alpha  { token_list_append(token_new(T_ALPHA)); INFO_TOKEN("T_ALPHA "); }
cmp    { token_list_append(token_new(T_CMP)); INFO_TOKEN("T_CMP "); }
or     { token_list_append(token_new(T_OR)); INFO_TOKEN("T_OR "); }
brae   { token_list_append(token_new(T_BRAE)); INFO_TOKEN("T_BRAE "); }
brane  { token_list_append(token_new(T_BRANE)); INFO_TOKEN("T_BRANE "); }
bra    { token_list_append(token_new(T_BRA)); INFO_TOKEN("T_BRA "); }
left   { token_list_append(token_new(T_LEFT)); INFO_TOKEN("T_LEFT "); }
right  { token_list_append(token_new(T_RIGHT)); INFO_TOKEN("T_RIGHT "); }
halt   { token_list_append(token_new(T_HALT)); INFO_TOKEN("T_HALT "); }
fail   { token_list_append(token_new(T_FAIL)); INFO_TOKEN("T_FAIL "); }
draw   { token_list_append(token_new(T_DRAW)); INFO_TOKEN("T_DRAW "); }

blank  { token_list_append(token_new(T_BLANK)); INFO_TOKEN("T_BLANK "); }

[a-z]+    {
	STAGE_ERROR(line_num, "Unrecognized token, \"%s\"", yytext);
}

"!"[^ \t\b\v\r\n;]+    { token_list_append(token_new(T_LABEL)); INFO_TOKEN("T_LABEL "); }

\'.\'    { token_list_append(token_new(T_CHARACTER)); INFO_TOKEN("T_CHARACTER "); }

\"[^"\n]+\"    {
		/* double-quoted string */
		Token *t;
		yytext++;
		t = token_new(T_STRING);
		t->token_str[strlen(t->token_str) - 1] = '\0';
		token_list_append(t);
		INFO_TOKEN("T_STRING ");
	}

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

#	if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
		seperator = (int)'\\';
#	else
		seperator = (int)'/';
#	endif

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
		exit(EXIT_FAILURE);
	}

	atexit(cleanup_listing);
	INFO_VERBOSE("Initializing listing structures... ");
	listing_init(in_file, base_filename);
	INFO_VERBOSE("Done\n");

	atexit(cleanup_symbols);

	INFO_VERBOSE("Initializing labels symbol table... ");
	symbol_table_init(labels);
	INFO_VERBOSE("Done\n");

	INFO_PARSER("\nBeginning syntactical analysis...\n\n");
	parser = ParseAlloc(malloc);
	atexit(cleanup_parser);

	for(pass = 1; pass <= 2; pass++) {
		address = 0x0;

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
				symbol_table_print(labels, "labels");
			}
			INFO_PARSER("--------------- Second parser pass ---------------\n\n");

			atexit(cleanup_blocks);
			INFO_VERBOSE("Initializing blocks tree... ");
			blocks_init();
			block_new(address);
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
			exit(EXIT_FAILURE);
		}
	}

	INFO_PARSER("Completed syntactical analysis\n");

	generate_binary();
	generate_listing();

	exit(EXIT_SUCCESS);
}
