#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "teenyat.h"

static void set_elg_flags(teenyat *t, tny_sword alu_result) {
	t->flags.equals = (alu_result == 0);
	t->flags.less =  alu_result & (1 << 15);
	t->flags.greater = (alu_result > 0);

	return;
}

static inline void trunc_pc(teenyat *t) {
	assert(t != NULL);
	t->reg[TNY_REG_PC].u &= TNY_MAX_RAM_ADDRESS;

	return;
}

static inline void set_pc(teenyat *t, tny_uword addr) {
	assert(t != NULL);
	t->reg[TNY_REG_PC].u = addr;
	trunc_pc(t);

	return;
}

static inline void inc_pc(teenyat *t) {
	set_pc(t, t->reg[TNY_REG_PC].u + 1);

	return;
}

static inline void dec_pc(teenyat *t) {
	set_pc(t, t->reg[TNY_REG_PC].u - 1);

	return;
}

bool tny_init_from_file(teenyat *t, FILE *bin_file,
                        TNY_READ_FROM_BUS_FNPTR bus_read,
                        TNY_WRITE_TO_BUS_FNPTR bus_write) {
	if(!t) return false;
	t->initialized = false;
	if(!bin_file) return false;
	if(!bus_read || !bus_write) return false;

	/* Clear the entire instance */
	memset(t, 0, sizeof(teenyat));

	/* backup .bin file */
	fread(t->bin_image, sizeof(tny_word), TNY_RAM_SIZE, bin_file);
	if(ferror(bin_file)) return false;

	/* store bus callbacks */
	t->bus_read = bus_read;
	t->bus_write = bus_write;

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

	t->delay_cycles = 0;
	t->cycle_cnt = 0;

	return true;
}

void tny_clock(teenyat *t) {
	t->cycle_cnt++;

	/*
	 * If there were still cycles left on the previous instruction, skip
	 * everythin else for now, and let those expire.
	 */
	if(t->delay_cycles) {
		t->delay_cycles--;
		return;
	}

	/*
	 * All instruction fetches are limited to the range 0x0000 through 0x7FFF.
	 * Modifications to the PC are always truncated to that range.  As such,
	 * an unusual circumstance could arrive where a two-word instruction begins
	 * at 0x7FFF and has its second word retrieved from 0x0000.  This almost
	 * certainly not something anyone would want, but it's how it works :-)
	 */
	trunc_pc(t);

	tny_uword orig_PC = t->reg[TNY_REG_PC].u; /* backup for error reporting */

 	tny_word IR = t->ram[t->reg[TNY_REG_PC].u];
	inc_pc(t);
	tny_sword immed = t->ram[t->reg[TNY_REG_PC].u].s;
	inc_pc(t);

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
		dec_pc(t);
		immed = IR.instruction.immed4;
	}
	else {
		/* double word instructions cost one extra cycle */
		t->delay_cycles++;
	}

	/*
	 * EXECUTE
	 */

	uint32_t tmp;  /* for quick use to determine carry */

	switch(opcode) {
	case TNY_OPCODE_SET:
		t->reg[reg1].s = t->reg[reg2].s + immed;
		break;
	case TNY_OPCODE_LOD:
		{
			tny_uword addr = t->reg[reg2].s + immed;
			if(addr > TNY_MAX_RAM_ADDRESS) {
				/* read from peripheral address */
				tny_word data = {.u = 0};
				uint16_t delay = 0;
				t->bus_read(t, addr, &data, &delay);
				t->reg[reg1] = data;
				t->delay_cycles += delay;
			}
			else {
				/* read from RAM */
				t->reg[reg1] = t->ram[addr];
			}

			/*
			 * To promote student use of registers, all bus operations,
			 * including RAM access comes with an extra penalty.
			 */
			t->delay_cycles += TNY_BUS_DELAY;
		}
		break;
	case TNY_OPCODE_STR:
		{
			tny_uword addr = t->reg[reg1].s + immed;
			if(addr > TNY_MAX_RAM_ADDRESS) {
				/* write to peripheral address */
				uint16_t delay = 0;
				t->bus_write(t, addr, t->reg[reg2], &delay);
				t->delay_cycles += delay;
			}
			else {
				/* write to RAM */
				t->ram[addr] = t->reg[reg2];
			}
			/*
			 * To promote student use of registers, all bus operations,
			 * including RAM access comes with an extra penalty.
			 */
			t->delay_cycles += TNY_BUS_DELAY;
		}
		break;
	case TNY_OPCODE_PSH:
		t->reg[TNY_REG_SP].u &= TNY_MAX_RAM_ADDRESS;
		t->ram[t->reg[TNY_REG_SP].u].u = t->reg[reg2].s + immed;
		t->reg[TNY_REG_SP].u--;
		t->reg[TNY_REG_SP].u &= TNY_MAX_RAM_ADDRESS;
		/*
		 * To promote student use of registers, all bus operations,
		 * including RAM access comes with an extra penalty.
		 */
		t->delay_cycles += TNY_BUS_DELAY;
		break;
	case TNY_OPCODE_POP:
		t->reg[TNY_REG_SP].u++;
		t->reg[TNY_REG_SP].u &= TNY_MAX_RAM_ADDRESS;
		t->reg[reg1] = t->ram[t->reg[TNY_REG_SP].u];
		/*
		 * To promote student use of registers, all bus operations,
		 * including RAM access comes with an extra penalty.
		 */
		t->delay_cycles += TNY_BUS_DELAY;
		break;
	case TNY_OPCODE_BTS:
		{
			tny_sword bit = t->reg[reg2].s + immed;
			if(bit >= 0 && bit <= 15) {
				t->reg[reg2].s |= (1 << bit);
				set_elg_flags(t, t->reg[reg2].s);
			}
		}
		break;
	case TNY_OPCODE_BTC:
		{
			tny_sword bit = t->reg[reg2].s + immed;
			if(bit >= 0 && bit <= 15) {
				t->reg[reg2].s &= ~(1 << bit);
				set_elg_flags(t, t->reg[reg2].s);
			}
		}
		break;
	case TNY_OPCODE_BTF:
		{
			tny_sword bit = t->reg[reg2].s + immed;
			if(bit >= 0 && bit <= 15) {
				t->reg[reg2].s ^= (1 << bit);
				set_elg_flags(t, t->reg[reg2].s);
			}
		}
		break;
	case TNY_OPCODE_CAL:
		t->reg[TNY_REG_SP].u &= TNY_MAX_RAM_ADDRESS;
		t->ram[t->reg[TNY_REG_SP].u] = t->reg[TNY_REG_PC];
		t->reg[TNY_REG_SP].u--;
		t->reg[TNY_REG_SP].u &= TNY_MAX_RAM_ADDRESS;
		set_pc(t, t->reg[reg2].s + immed);
		/*
		 * To promote student use of registers, all bus operations,
		 * including RAM access comes with an extra penalty.
		 */
		t->delay_cycles += TNY_BUS_DELAY;
		break;
	case TNY_OPCODE_ADD:
		tmp = (uint32_t)(t->reg[reg1].s) + (uint32_t)((uint32_t)(t->reg[reg2].s) + (uint32_t)immed);
		t->flags.carry = tmp & (1 << 16);
		t->reg[reg1].s = tmp;
		set_elg_flags(t, t->reg[reg1].s);
		break;
	case TNY_OPCODE_SUB:
		tmp = (uint32_t)(t->reg[reg1].s) - (uint32_t)((uint32_t)(t->reg[reg2].s) + (uint32_t)immed);
		t->flags.carry = tmp & (1 << 16);
		t->reg[reg1].s = tmp;
		set_elg_flags(t, t->reg[reg1].s);
		break;
	case TNY_OPCODE_MPY:
		tmp = (uint32_t)(t->reg[reg1].s) * (uint32_t)((uint32_t)(t->reg[reg2].s) + (uint32_t)immed);
		t->flags.carry = tmp & (1 << 16);
		t->reg[reg1].s = tmp;
		set_elg_flags(t, t->reg[reg1].s);
		break;
	case TNY_OPCODE_DIV:
		t->reg[reg1].s /= t->reg[reg2].s + immed;
		set_elg_flags(t, t->reg[reg1].s);
		break;
	case TNY_OPCODE_MOD:
		t->reg[reg1].s %= t->reg[reg2].s + immed;
		set_elg_flags(t, t->reg[reg1].s);
		break;
	case TNY_OPCODE_AND:
		t->reg[reg1].s &= t->reg[reg2].s + immed;
		set_elg_flags(t, t->reg[reg1].s);
		break;
	case TNY_OPCODE_OR:
		t->reg[reg1].s |= t->reg[reg2].s + immed;
		set_elg_flags(t, t->reg[reg1].s);
		break;
	case TNY_OPCODE_XOR:
		t->reg[reg1].s ^= t->reg[reg2].s + immed;
		set_elg_flags(t, t->reg[reg1].s);
		break;
	case TNY_OPCODE_SHF:
		{
			tny_sword bits_to_shift = t->reg[reg2].s + immed;
			if(bits_to_shift < 0) {
				/* shift left */
				bits_to_shift *= -1;
				if(bits_to_shift <= 15) {
					t->reg[reg1].u <<= bits_to_shift - 1;
					t->flags.carry = t->reg[reg1].u & (1 << 15);
					t->reg[reg1].u <<= 1;
				}
				else {
					if(bits_to_shift == 16) {
						t->flags.carry = t->reg[reg1].u & (1 << 0);
					}
					else {
						t->flags.carry = 0;
					}
					t->reg[reg1].u = 0;
				}
			}
			else if(bits_to_shift > 0) {
				/* shift right */
				if(bits_to_shift <= 15) {
					t->reg[reg1].u >>= bits_to_shift - 1;
					t->flags.carry = t->reg[reg1].u & (1 << 0);
					t->reg[reg1].u >>= 1;
				}
				else {
					if(bits_to_shift == 16) {
						t->flags.carry = t->reg[reg1].u & (1 << 15);
					}
					else {
						t->flags.carry = 0;
					}
					t->reg[reg1].u = 0;
				}
			}
			set_elg_flags(t, t->reg[reg1].s);
		}
		break;
	case TNY_OPCODE_ROT:
		{
			/* calculate remainder as rotate could go around many times */
			tny_sword bits_to_rotate = (t->reg[reg2].s + immed) % 16;
			if(bits_to_rotate < 0) {
				/* rotate left */
				bits_to_rotate *= -1;
				tny_uword main_part = t->reg[reg1].u << bits_to_rotate;
				tny_uword wrap_part = t->reg[reg1].u >> (16 - bits_to_rotate);
				t->reg[reg1].u = main_part | wrap_part;
				t->flags.carry = t->reg[reg1].u & (1 << 0);
			}
			else if(bits_to_rotate > 0) {
				/* rotate right */
				tny_uword main_part = t->reg[reg1].u >> bits_to_rotate;
				tny_uword wrap_part = t->reg[reg1].u << (16 - bits_to_rotate);
				t->reg[reg1].u = main_part | wrap_part;
				t->flags.carry = t->reg[reg1].u & (1 << 15);
			}
			set_elg_flags(t, t->reg[reg1].s);
		}
		break;
	case TNY_OPCODE_NEG:
		tmp = (uint32_t)0 - (uint32_t)(t->reg[reg1].s);
		t->flags.carry = tmp & (1 << 16);
		t->reg[reg1].s = tmp;
		set_elg_flags(t, t->reg[reg1].s);
		break;
	case TNY_OPCODE_CMP:
		tmp = (uint32_t)(t->reg[reg1].s) - (uint32_t)((uint32_t)(t->reg[reg2].s) + (uint32_t)immed);
		t->flags.carry = tmp & (1 << 16);
		set_elg_flags(t, (tny_sword)tmp);
		break;
	case TNY_OPCODE_JMP:
		{
			bool flags_checked = false;
			bool condition_satisfied = false;
			if(carry) {
				flags_checked = true;
				condition_satisfied |= t->flags.carry;
			}
			if(equals) {
				flags_checked = true;
				condition_satisfied |= t->flags.equals;
			}
			if(less) {
				flags_checked = true;
				condition_satisfied |= t->flags.less;
			}
			if(greater) {
				flags_checked = true;
				condition_satisfied |= t->flags.greater;
			}
			if(!flags_checked || condition_satisfied) {
				set_pc(t, t->reg[reg2].s + immed);
			}
		}
		break;
	case TNY_OPCODE_DJZ:
		tmp = (uint32_t)(t->reg[reg1].s) - 1;
		t->flags.carry = tmp & (1 << 16);
		t->reg[reg1].s = tmp;
		set_elg_flags(t, (tny_sword)tmp);
		if(tmp == 0) {
			set_pc(t, t->reg[reg2].s + immed);
		}
		break;
	default:
		fprintf(stderr, "Uknown opcode (%d) encountered at 0x%04X on cycle %" PRIu64 "\n",
		        opcode, orig_PC, t->cycle_cnt);
		break;
	}

	/* Ensure the zero register still has a zero in it */
	t->reg[TNY_REG_ZERO].u = 0;

	return;
}
