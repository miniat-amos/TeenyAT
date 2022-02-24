#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "util.h"
#include "list.h"
#include "tnasm.h"

token_list tokens_list;
token_list *tokens = &tokens_list;

void token_free(void *item) {

	Token *t = (Token *)item;
	if(t) {
		if(t->token_str) {
			free(t->token_str);
		}
		free(t);
	}

	return;
}

int token_compare(void *item1, void *item2) {

	ERROR("There shold be no comparisons on the token list");

	return 1;
}

void token_list_init() {

	list_init(tokens, token_free, token_compare);

	return;
}

Token *token_list_append(Token *tok) {

	return (Token *)list_append((list *)tokens, (void *)tok);
}
