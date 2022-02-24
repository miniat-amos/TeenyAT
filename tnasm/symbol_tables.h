#ifndef _SYMBOL_TABLES_H_
#define _SYMBOL_TABLES_H_

#include <stdlib.h>
#include <stdbool.h>

#include "util.h"
#include "list.h"

typedef struct symbol symbol;
typedef struct list symbol_table;

struct symbol {
	char *str;
	tny_word value;
};

extern symbol_table *labels;

/*
 * symbol_table_init()
 *
 * Initializes a symbol table to an empty state.  The argument should already
 * be allocated... perhaps the address of an static/global instance.
 */
extern void symbol_table_init(symbol_table *t);

/*
 * symbol_table_add()
 *
 * Adds the string::value pair to the symbol table if it isn't there already.
 * Returns the address of the symbol in the table if added, NULL otherwise.
 */
extern symbol *symbol_table_add(symbol_table *t, char *str, tny_uword value);

/*
 * symbol_table_search()
 *
 * searches for a symbol table entry with the same str.  Returns the address of
 * the matched symbol; NULL if no match found.
 */
extern symbol *symbol_table_search(symbol_table *t, char *str);


/*
 * symbol_table_print()
 *
 * prints the string:value pairs maintained in the symbol table.
 */
extern void symbol_table_print(symbol_table *t, char *header);

#endif /* _SYMBOL_TABLES_H_ */
