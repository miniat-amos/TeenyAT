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
