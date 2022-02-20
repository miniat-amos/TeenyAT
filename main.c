#include <stdio.h>
#include <stdlib.h>

#include "teenyat.h"

int main(int argc, char *argv[]) {
	FILE *bin_file = fopen(argv[1], "rb");
	teenyat t;
	tny_init_from_file(&t, bin_file);

	for(int i = 0; i < 123456; i++) {
		tny_clock(&t);
	}

	return EXIT_SUCCESS;
}
