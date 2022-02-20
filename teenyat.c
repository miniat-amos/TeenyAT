#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "teenyat.h"

bool tny_init_from_file(teenyat *t, FILE *bin_file) {
	if(!t) return false;
	t->initialized = false;
	if(!bin_file) return false;

	/* Clear the entire instance */
	memset(t, 0, sizeof(teenyat));

	/* backup .bin file */
	fread(t->bin_image, sizeof(tny_word), TNY_RAM_SIZE, bin_file);
	if(ferror(bin_file)) return false;

	if(!tny_reset(t)) return false;

	t->initialized = true;
	return true;
}

bool tny_reset(teenyat *t) {
	if(!t) return false;

	/* restore ram to it's initial post-bin-load state */
	memcpy(t->ram, t->bin_image, TNY_RAM_SIZE);

	t->reg[TNY_REG_PC] = 0x0;
	t->reg[TNY_REG_SP] = 0x7FFF;
	t->reg[TNY_REG_ZERO] = 0;
	t->reg[TNY_REG_A] = 0;
	t->reg[TNY_REG_B] = 0;
	t->reg[TNY_REG_C] = 0;
	t->reg[TNY_REG_D] = 0;
	t->reg[TNY_REG_E] = 0;

	t->flags.carry = false;
	t->flags.equals = false;
	t->flags.less = false;
	t->flags.greater = false;

	/* "instruction" and "immediate" members do not need initialization */

	/* bus "address" and "data" members do not need initialization */
	t->bus.state = 'X';
	t->delay_cycles = 0;

	return true;
}

bool tny_clock(teenyat *t) {
	/* stubbed clock cycle does nothing */
	return false;
}

void tny_lod_result(teenyat *t, tny_word data, uint16_t delay) {
	assert(t);
	assert(t->bus.state == 'R');

	t->bus.data = data;
	t->bus.state = 'X';
	t->delay_cycles += delay;

	return;
}

void tny_str_result(teenyat *t, uint16_t delay) {
	assert(t);
	assert(t->bus.state == 'W');

	t->bus.state = 'X';
	t->delay_cycles += delay;

	return;
}
