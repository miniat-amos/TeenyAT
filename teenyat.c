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

	t->reg[TNY_REG_PC].u = 0x0;
	t->reg[TNY_REG_SP].u = 0x7FFF;
	t->reg[TNY_REG_ZERO].u = 0;
	t->reg[TNY_REG_A].u = 0;
	t->reg[TNY_REG_B].u = 0;
	t->reg[TNY_REG_C].u = 0;
	t->reg[TNY_REG_D].u = 0;
	t->reg[TNY_REG_E].u = 0;

	t->flags.carry = false;
	t->flags.equals = false;
	t->flags.less = false;
	t->flags.greater = false;

	/* "instruction" and "immediate" members do not need initialization */

	/* bus "address" and "data" members do not need initialization */
	t->bus.state = 'X';
	t->delay_cycles = 0;
	t->cycle_cnt = 0;

	return true;
}

bool tny_clock(teenyat *t) {
	t->cycle_cnt++;

	/*
	 * If there were still cycles left on the previous instruction, skip
	 * everythin else for now, and let those expire.
	 */
	if(t->delay_cycles) {
		t->delay_cycles--;
		return false;
	}

	/*
	 * All instruction fetches are limited to the range 0x0000 through 0x7FFF.
	 * Modifications to the PC are always truncated to that range.  As such,
	 * an unusual circumstance could arrive where a two-word instruction begins
	 * at 0x7FFF and has its second word retrieved from 0x0000.  This almost
	 * certainly not something anyone would want, but it's how it works :-)
	 */
	t->reg[TNY_REG_PC].u &= TNY_MAX_RAM_ADDRESS;

	tny_uword orig_PC = t->reg[TNY_REG_PC].u; /* backup for error reporting */

 	tny_word IR = t->ram[t->reg[TNY_REG_PC].u++];
	t->reg[TNY_REG_PC].u &= TNY_MAX_RAM_ADDRESS;
	tny_uword immed = t->ram[t->reg[TNY_REG_PC].u++].u;

	/*
	 * We're not masking the PC immediately here, because the instruction may
	 * bee "teeny", in which case we'll have to modify the PC again, so we'll
	 * wait until after that to mask it within range.
	 */

	tny_uword opcode = IR.instruction.opcode;
	bool teeny = IR.instruction.teeny;
	tny_uword reg1 = IR.instruction.reg1;
	tny_uword reg2 = IR.instruction.reg1;
	bool carry = IR.inst_flags.carry;
	bool equals = IR.inst_flags.equals;
	bool less = IR.inst_flags.less;
	bool greater = IR.inst_flags.greater;

	if(teeny) {
		/*
		 * This is a single word instruction encoding
		 */
		t->reg[TNY_REG_PC].u--;
		immed = IR.instruction.immed4;
	}
	/* mask the PC into range, whether teeny or not */
	t->reg[TNY_REG_PC].u &= TNY_MAX_RAM_ADDRESS;

	/*
	 * EXECUTE
	 */

	switch(opcode) {
	default:
		fprintf(stderr, "Uknown opcode (%d) encountered at 0x%04X on cycle %llu\n",
		        opcode, orig_PC, t->cycle_cnt);
		break;
	}

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
