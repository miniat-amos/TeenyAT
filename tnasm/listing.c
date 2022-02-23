#include <stdio.h>
#include <string.h>

#include "listing.h"
#include "util.h"
#include "blocks.h"

FILE *listing_file = NULL;
char *listing_filename = NULL;

char **lines = NULL;
int line_cnt = 0;
int line_digits; // # of digits in largest line number

void fprint_line_number(FILE *f, int num);
int calculate_digits(int num);

void listing_init(FILE *in, char *base_filename) {

	long in_file_size;
	char *buffer;
	char *pos;
	size_t bytes_read;
	size_t bytes_left;
	char c;
	char *begin;
	char *end;
	int current_line;

	if(!option_generate_listing) {
		return;
	}

	if(!in) {
		IMPOSSIBLE("Bad input file stream given for listing");
	}

	if(!base_filename || strlen(base_filename) <= 0) {
		IMPOSSIBLE("Bad base filename given for listing");
	}

	/* +5 is for ".lst" and terminating NULL */
	SAFE_MALLOC(listing_filename, char *, sizeof(char) * (strlen(base_filename) + 5));
	sprintf(listing_filename, "%s.lst", base_filename);
	listing_file = fopen(listing_filename, "wb");
	if(!listing_file) {
		ERROR("Could not open listing file, \"%s\", for output", listing_filename);
	}

	if(fseek(in, 0, SEEK_END) != 0) {
		ERROR("Couldn't seek to end of input file for some reason");
	}

	/*
	 * Read in the file to a temporary buffer
	 */
	in_file_size = ftell(in);
	if(in_file_size <= 0) {
		ERROR("The input file is empty");
	}
	rewind(in);
	SAFE_MALLOC(buffer, char *, in_file_size + 1);
	bytes_left = in_file_size;
	pos = buffer;
	while((bytes_read = fread(pos, 1, bytes_left, in)) < bytes_left) {
		pos += bytes_read;
		bytes_left -= bytes_read;
	}

	line_cnt = 0;
	end = buffer;
	while((c = *end) != '\0') {
		if(c == '\n') {
			line_cnt++;
		}
		end++;
	}
	if(*(end - 1) != '\n') {
		line_cnt++;
	}
	SAFE_MALLOC(lines, char **, sizeof(char *) * line_cnt);

	line_digits = calculate_digits(line_cnt + 1);

	begin = buffer;
	end = buffer;
	current_line = 0;
	while((c = *end) != '\0') {
		if(c == '\n') {
			int line_length = (int)(end - begin);
			SAFE_MALLOC(lines[current_line], char *, sizeof(char) * (line_length + 1));
			memcpy(lines[current_line], begin, line_length);
			current_line++;
			end++;
			begin = end;
		}
		else {
			end++;
		}
	}
	if(*(end - 1) != '\n') {
		/* +1 this time because we're not excluding the last char... previously \n */
		int line_length = (int)(end - begin + 1);
		SAFE_MALLOC(lines[current_line], char *, sizeof(char) * (line_length + 1));
		memcpy(lines[current_line], begin, line_length);
	}

	if(buffer) {
		free(buffer);
	}

	return;
}

void cleanup_listing() {

	if(!option_generate_listing) {
		return;
	}

	INFO_VERBOSE("Cleaning up listing structures... ");

	if(listing_file) {
		fclose(listing_file);
	}

	if(listing_filename) {
		free(listing_filename);
	}

	if(lines) {
		int i;
		for(i = 0; i < line_cnt; i++) {
			if(lines[i]) {
				free(lines[i]);
			}
		}
		free(lines);
	}

	INFO_VERBOSE("Done\n");

	return;
}

void fprint_line_number(FILE *f, int num) {

	int i;
	int spaces;

	spaces = line_digits - calculate_digits(num);
	for(i = 0; i < spaces; i++) {
		fprintf(f, " ");
		INFO_VERBOSE(" ");
	}
	fprintf(f, "%d", num);
	INFO_VERBOSE("%d", num);

	return;
}

int calculate_digits(int num) {

	int i;
	int ten_power;
	int spaces;

	ten_power = 1;
	spaces = 0;
	while(ten_power <= num) {
		ten_power *= 10;
		spaces++;
	}

	return spaces;
}

void generate_listing() {

	/*
	 * NOTE
	 *
	 * Because the outer loop of this method is based on the line_cnt variable
	 * which is only set in listing_init, NOTHING happens when listing generation
	 * was NOT given as a command argument because the variable will be 0.
	 */

	int current_line_number;
	list_node *n;
	block_word *w;

	n = flat_block_words->head;
	/* don't bother with word-less files */
	if(!n) {
		return;
	}

	INFO_VERBOSE("\nGenerating assembly listing...\n\n");

	w = (block_word *)n->item;

	for(current_line_number = 1; current_line_number <= line_cnt; current_line_number++) {
		int line_word_cnt;
		fprint_line_number(listing_file, current_line_number);
		fprintf(listing_file, ":  ");
		INFO_VERBOSE(":  ");

		if(!n || !w) {
			/* no words for this line */
			if(current_line_number % 5 == 0) {
				fprintf(listing_file, " . . . . . . . . . . . . . . . .");
				INFO_VERBOSE(" . . . . . . . . . . . . . . . .");
			}
			else {
				fprintf(listing_file, "                                ");
				INFO_VERBOSE("                                ");
			}
		}
		else {
			if(current_line_number < w->line_number) {
				/* no words for this line */
				if(current_line_number % 5 == 0) {
					fprintf(listing_file, " . . . . . . . . . . . . . . . .");
					INFO_VERBOSE(" . . . . . . . . . . . . . . . .");
				}
				else {
					fprintf(listing_file, "                                ");
					INFO_VERBOSE("                                ");
				}
			}
			else if(current_line_number > w->line_number) {
				/* the first word for a line should always be >= actual line */
				IMPOSSIBLE("Tip the barrel to see if it spills!");
			}
			else {
				/* this word is for this line */
				fprintf(listing_file, "[0x%04X]  0x%08X", w->address, w->data);
				INFO_VERBOSE("[0x%04X]  0x%08X", w->address, w->data);

				/* now print the second word if applicable */
				if((n = n->next) && (w = (block_word *)n->item)) {
					if(current_line_number == w->line_number) {
						fprintf(listing_file, "  0x%08X", w->data);
						INFO_VERBOSE("  0x%08X", w->data);
						n && (n = n->next) && (w = (block_word *)n->item);
					}
					else {
						fprintf(listing_file, "            ");
						INFO_VERBOSE("            ");
					}
				}
				else {
					w = NULL;
					fprintf(listing_file, "            ");
					INFO_VERBOSE("            ");
				}
			}
		}

		fprintf(listing_file, "\t%s", lines[current_line_number - 1]);
		INFO_VERBOSE("\t%s", lines[current_line_number - 1]);

		line_word_cnt = 0;
		while(n && w && current_line_number == w->line_number) {
			/* every new pair goes on a new output line */
			if(line_word_cnt % 2 == 0) {
				int i;
				fprintf(listing_file, "\n");
				INFO_VERBOSE("\n");
				for(i = 0; i < line_digits; i++) {
					fprintf(listing_file, " ");
					INFO_VERBOSE(" ");
				}
				fprintf(listing_file, "   ");
				INFO_VERBOSE("   ");
				fprintf(listing_file, "[0x%04X]", w->address);
				INFO_VERBOSE("[0x%04X]", w->address);
			}

			fprintf(listing_file, "  0x%08X", w->data);
			INFO_VERBOSE("  0x%08X", w->data);

			n && (n = n->next) && (w = (block_word *)n->item);
			line_word_cnt++;
		}

		fprintf(listing_file, "\n");
		INFO_VERBOSE("\n");
	}

	return;
}
