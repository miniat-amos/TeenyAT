/*
 * Name	   : main.c
 * Author	 : William "Amos" Confer
 *
 * License	: Copyright (C) 2023 All rights reserved
 *
 * This brief program is meant to serve as an ultra simplistic example of
 * how the TeenyAT header and source can be quickly used to create a
 * simulated system.  In this case, the system loads whatever binary image
 * is provided and runs it for up 123456 cycles.  Any attempt to read from
 * the bus will result in a 9-cycle access to the number 42, and writes to
 * the bus will have no side effect other than wasting 7 cycles.
 */

#include <stdio.h>
#include <stdlib.h>

#include "teenyat.h"

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay);
void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay);

int main(int argc, char *argv[]) {
	if(argc != 2) {
		fprintf(stderr, "usage:  %s <bin_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	FILE *bin_file = fopen(argv[1], "rb");
	teenyat t;
	tny_init_from_file(&t, bin_file, bus_read, bus_write);

	for(int i = 0; i < 123456; i++) {
		tny_clock(&t);
	}

	return EXIT_SUCCESS;
}

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay) {
	data->s = +42;  /* That's what the answer has always been */
	*delay = 9;  /* add a delay overhead of 9 cycles */

	return;
}

void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay) {
	*delay = 7;  /* add a delay overhead of 7 cycles */

	return;
}
