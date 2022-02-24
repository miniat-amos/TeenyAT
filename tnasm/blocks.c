#include "../teenyat.h"
#include "blocks.h"
#include "list.h"
#include "util.h"

list blocks_list;
list *blocks = &blocks_list;

list flat_block_words_list;
list *flat_block_words = &flat_block_words_list;

tny_uword next_word_address;

void block_destructor(void *item) {

	block *b;

	if(item) {
		b = (block *)item;
		if(b->words) {
			list_delete(b->words);
			free(b->words);
		}
		free(item);
	}

	return;
}

void word_destructor(void *item) {

	if(item) {
		free(item);
	}

	return;
}

void block_word_destructor(void *item) {

	if(item) {
		free(item);
	}

	return;
}
int no_compare(void *i1, void *i2) {

	/* DO NOTHING AT ALL */

	return 0;
}

void blocks_init() {

	list_init(blocks, block_destructor, no_compare);
	list_init(flat_block_words, block_word_destructor, no_compare);

	return;
}

void block_new(tny_uword address) {

	block *b;

	INFO_VERBOSE("New address block begining at 0x%04X\n", address);
	next_word_address = address;

	if(blocks->tail && ((block *)(blocks->tail->item))->words->size == 0) {
		((block *)(blocks->tail->item))->address = address;
	}
	else {
		SAFE_MALLOC(b, block *, sizeof(block));
		SAFE_MALLOC(b->words, list *, sizeof(list));
		b->address = address;
		list_init(b->words, block_word_destructor, no_compare);
		list_append(blocks, (void *)b);
	}

	return;
}

void block_new_word(int line_number, tny_uword data) {

	/* TODO: do we need 2 words or can we share the first instance? */
	block_word *w1;
	block_word *w2;
	block *b;

	SAFE_MALLOC(w1, block_word *, sizeof(block_word));
	w1->line_number = line_number;
	w1->data = data;
	w1->address = next_word_address;
	b = (block *)(blocks->tail->item);
	list_append(b->words, (void *)w1);

	SAFE_MALLOC(w2, block_word *, sizeof(block_word));
	w2->line_number = line_number;
	w2->data = data;
	w2->address = next_word_address;
	list_append(flat_block_words, (void *)w2);

	next_word_address++;

	return;
}

void cleanup_blocks() {

	INFO_VERBOSE("Cleaning up blocks tree... ");
	list_delete(blocks);
	list_delete(flat_block_words);
	INFO_VERBOSE("Done\n");

	return;
}
