#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "list.h"
#include "../teenyat.h"


typedef struct Token Token;
typedef list token_list;

struct Token {
	char *token_str;
	int line_num;
	int type;
	m_word data;
};

extern token_list *tokens;
//extern token_list *extra_tokens;

/*
 * token_list_init()
 *
 * Initializes the "tokens" list to an empty state.
 */
extern void token_list_init();

/*
 * token_list_add()
 *
 * Adds the token to the tail of the "tokens" list .
 * Returns the address of the token in the table if added, NULL otherwise.
 */
extern Token *token_list_append(Token *tok);

#endif /* _TOKEN_H_ */
