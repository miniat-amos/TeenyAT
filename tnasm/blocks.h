#ifndef _BLOCKS_H_
#define _BLOCKS_H_

#include "../teenyat.h"
#include "list.h"

typedef struct block block;
typedef struct block_word block_word;

struct block {
	tny_uword address;
	list *words;
};

struct block_word {
	int line_number;
	tny_uword data;
	tny_uword address;
};

extern list *blocks;
extern list *flat_block_words;

extern void blocks_init();
extern void block_new(tny_uword address);
extern void block_new_word(int line_number, tny_uword data);
extern void cleanup_blocks();

extern void blocks_print();

#endif /* _BLOCKS_H_ */
