#ifndef _LISTING_H_
#define _LISTING_H_

#include <stdio.h>

#include "list.h"

/*
 * listing_init()
 *
 * Reads in all the lines from the already open "in" file and generates
 * the listing filename.
 */
extern void listing_init(FILE *in, char *base_filename);

/*
 * cleanup_listing()
 *
 * Cleanup on exit
 */
extern void cleanup_listing();

extern void generate_listing();

#endif /* _LISTING_H_ */
