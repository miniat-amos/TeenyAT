#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../teenyat.h"

typedef struct instruction instruction;

struct instruction {
	m_low_word_layout low;
	m_word high;
	int line_num;
};

#define STAGE_ERROR_MAX 16

extern bool option_dump_hex;
extern bool option_generate_listing;
extern bool option_parser_info;
extern bool option_tokenizer_info;
extern bool option_verbose_info;

extern bool mode_nop_delay;

extern int stage_error_cnt;

extern int pass;
extern m_word address;
extern bool inline_variables;
extern m_word variable_address;

typedef enum util_print_level util_print_level;
enum util_print_level {
	util_print_level_info = 0,
	util_print_level_error,
	util_print_level_impossible
};

#define __FILENAME__ (strrchr(__FILE__, (int)'/') ? (strrchr(__FILE__, (int)'/') + 1) : \
	(strrchr(__FILE__, (int)'\\') ? (strrchr(__FILE__, (int)'\\') + 1) : \
	__FILE__) \
)

/*
 * ERROR macro
 *
 * prints error message and exits.
 */
#define ERROR(fmt, ...) \
	do { \
		util_print(util_print_level_error, stderr, NULL, 0, fmt, ##__VA_ARGS__); \
		exit(1); \
	} while(0)

/*
 * IMPOSSIBLE macro
 *
 * Prints the given error message and its source location before exiting.
 */
#define IMPOSSIBLE(fmt, ...) \
	do { \
		util_print(util_print_level_impossible, stderr, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__); \
		fprintf(stderr, "\n"); \
		exit(1); \
	} while(0)

/*
 * INFO macro
 *
 * Prints the string when the condition is true.  This macro is intended to be used
 * by other macros/methods for command argument output filtering.
 */
#define INFO(condition, fmt, ...) \
	do { \
		if (condition) { \
			util_print(util_print_level_info, stdout, NULL, 0, fmt, ##__VA_ARGS__); \
		} \
	} while(0)


/*
 * INFO_... macros
 *
 * Display command argument filtered output.
 */
#define INFO_HEX(fmt, ...) INFO(option_dump_hex, fmt, ##__VA_ARGS__)
#define INFO_TOKEN(fmt, ...) INFO(option_tokenizer_info, fmt, ##__VA_ARGS__)
#define INFO_LISTING(fmt, ...) INFO(option_generate_listing, fmt, ##__VA_ARGS__)
#define INFO_PARSER(fmt, ...) INFO(option_parser_info, fmt, ##__VA_ARGS__)
#define INFO_VERBOSE(fmt, ...) INFO(option_verbose_info, fmt, ##__VA_ARGS__)

/*
 * SAFE_MALLOC macro
 *
 * Allocate, cast, and clear memory request or exit.
 */
#define SAFE_MALLOC(ptr, ptr_type, size) \
	do { \
		\
		ptr = (ptr_type)calloc(1, size); \
		if(ptr == NULL) { \
			ERROR("Unable to allocate memory"); \
		} \
	} while(0)

/*
 * SAFE_REALLOC macro
 *
 * Reallocate and cast, or exit.
 */
#define SAFE_REALLOC(ptr, ptr_type, size) \
	do { \
		\
		void *tmp = (ptr_type)realloc(ptr, size); \
		if(tmp == NULL) { \
			ERROR("Unable to allocate memory"); \
		} \
		else { \
			ptr = tmp; \
		} \
	} while(0)

/*
 * STAGE_ERROR macro
 *
 * Prints the given error message and its user's source location.  Unlike the "ERROR" macros,
 * this macro is for printing errors about the user's input file (etc), not for printing
 * about tnasm internal errors.
 */
#define STAGE_ERROR(line_num, fmt, ...) \
	do { \
		util_print(util_print_level_error, stderr, NULL, 0, "line %d - ", line_num); \
		util_print(util_print_level_info, stderr, NULL, 0, fmt, ##__VA_ARGS__); \
		fprintf(stderr, "\n"); \
		stage_error_cnt++; \
		if(stage_error_cnt >= STAGE_ERROR_MAX) { \
			ERROR("%d errors already!  Giving up.\n", stage_error_cnt); \
		} \
	} while(0)

/**
 * prints an error message to a specified stream
 * @param lvl
 *  the error level
 * @param out
 *  file to output the message too
 * @param filename
 *  The filename of the current file, a NULL filename causes it to not print file and line
 * @param lineno
 *  The line number the error occurred on
 * @param fmt
 *  The format specifier consistent with printf
 */
extern void util_print(util_print_level lvl, FILE *out, const char *filename, int lineno, const char *fmt, ...)
		__attribute__ ((format (printf, 5, 6)));

#endif /* _UTIL_H_ */
