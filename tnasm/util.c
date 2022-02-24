#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "util.h"

bool option_dump_hex         = false;
bool option_generate_listing = false;
bool option_parser_info      = false;
bool option_tokenizer_info   = false;
bool option_verbose_info     = false;

int stage_error_cnt = 0;

int pass;
tny_uword address;

static const char *lvl_string[] = {
		"", /* Info messages have no lead */
		"Error: ",
		"Impossible: ",
		"Error: ",
		"",
};

void util_print(util_print_level lvl, FILE *out, const char *filename, int lineno, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);
	fflush(NULL);
	fprintf(out, "%s", lvl_string[lvl]);
	if(filename) {
		fprintf(out, "%s", filename);
		if(lineno > 0) {
			fprintf(out, ":%d", lineno);
		}
		fprintf(out, " - ");
	}
	vfprintf(out, fmt, args);
	va_end(args);
	fflush(out);
}
