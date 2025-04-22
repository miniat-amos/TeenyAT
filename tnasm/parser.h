#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>

#include "token.h"


typedef struct identifier_data identifier_data;


struct identifier_data {
    tny_word addr;
    tny_word val;
    int line_no;
};


bool parse(token_lines &parse_lines, vector <string> asm_lines);


#endif /* __PARSER_H__ */
