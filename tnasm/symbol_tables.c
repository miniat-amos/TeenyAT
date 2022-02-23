#include <stdlib.h>
#include <string.h>

#include "symbol_tables.h"
#include "token.h"

symbol_table labels_table;
symbol_table constants_table;
symbol_table variables_table;

symbol_table *labels = &labels_table;
symbol_table *constants = &constants_table;
symbol_table *variables = &variables_table;

void symbol_free(void *item) {

	symbol *s = (symbol *)item;
	if(s) {
		if(s->str) {
			free(s->str);
		}
		free(s);
	}

	return;
}

int symbol_compare(void *item1, void *item2) {

	int result;

	symbol *s1 = (symbol *)item1;
	symbol *s2 = (symbol *)item2;

	if(!s1 || !s2 || !s1->str || !s2->str) {
		result = 1;
	}
	else {
		result = strcmp(s1->str, s2->str);
	}

	return result;
}

void symbol_table_init(symbol_table *t) {

	list_init((list *)t, symbol_free, symbol_compare);

	return;
}

symbol *symbol_table_add(symbol_table *t, char *str, m_word value) {

	symbol *s;
	INFO_VERBOSE("adding \"%s\"=0x%08X to a symbol table... ", str, value.u);
	SAFE_MALLOC(s, symbol *, sizeof(symbol));
	SAFE_MALLOC(s->str, char *, sizeof(char) * (strlen(str) + 1));
	strcpy(s->str, str);
	s->value = value;

	if(list_search((list *)t, (void *)s)) {
		symbol_free(s);  // it's already there, so we don't want another
		INFO_VERBOSE("it's already there\n");
		return NULL;
	}

	list_append((list *)t, (void*)s);
	INFO_VERBOSE("Done\n");
	return s;
}

symbol *symbol_table_search(symbol_table *t, char *str) {

	static symbol s;
	symbol *result;

	SAFE_MALLOC(s.str, char *, sizeof(char) * (strlen(str) + 1));
    strcpy(s.str, str);
	result = list_search((list *)t, (void *)&s);
	free(s.str);

	return result;
}

void symbol_table_print(symbol_table *t, char *header) {

    printf("\nContents of symbol table, \"%s\":\n", header);

    if(t->size <= 0) {
        printf("\t<empty>\n");
    }
    else {
        symbol *s;
        list_node *current = t->head;

        while(current) {
            if(current->item) {
                s = (symbol *)(current->item);
                if(s->str) {
                    printf("\t\"%s\" : 0x%08X (hex): %d (signed) : %u (unsigned)\n", s->str, s->value.u, s->value.s, s->value.u);
                }
                else {
                    IMPOSSIBLE("A symbol in \"%s\" with a value of 0x%08X has a NULL string!", header, s->value.u);
                }

            }
            else {
                IMPOSSIBLE("A symbol in \"%s\" has no item!", header);
            }
            current = current->next;
        }
    }
    printf("\n");

    return;
}
