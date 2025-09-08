#ifndef __LISTING_H__
#define __LISTING_H__

#include <iostream>
#include <vector>

#include "teenyat.h"
#include "token.h"
#include "listing.h"

struct listing_block {
    string asm_line;
    bool uses_words = false;
    int line_no;
    tny_uword address;
    vector <tny_uword> bin_uwords;
};

extern vector <listing_block> listing_blocks;

void generate_listing();

#endif  /* __LISTING_H__ */
