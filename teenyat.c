/*
 * Name	   : teenyat.c
 * Author	 : William "Amos" Confer
 *
 * License	: Copyright (C) 2023 All rights reserved
 */

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "teenyat.h"

tny_uword tny_random(teenyat *t);

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
	size_t words_read = fread(t->bin_image, sizeof(tny_word), TNY_RAM_SIZE, bin_file);
	if((words_read <= 0) || ferror(bin_file)) return false;

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

	/* Initialize ports to output w/ all 0s */
	t->port_a.u = 0;
	t->port_a_directions.u = 0;
	t->port_b.u = 0;
	t->port_b_directions.u = 0;

	t->port_change = NULL;

	/*
	 * Initialize each teenyat with a unique random number stream
	 */
	static uint64_t stream = 1;  // start at >=1 for increment constant to be unique

	/*
	 * find a random seed
	 */
	uint64_t seed = (intptr_t)&time << 32;
	seed ^= (intptr_t)&printf;
	seed ^= time(NULL);
	/* make seed "more" random w/ /dev/urandom if there */
	FILE *f = fopen("/dev/urandom", "rb");
	if(f != NULL) {
		uint64_t entropy_bits;
		if(fread(&entropy_bits, sizeof(entropy_bits), 1, f) == sizeof(entropy_bits)) {
			seed ^= entropy_bits;
		}
		fclose(f);
	}

	/* Set increment to arbitrary odd constant that goes up by stream */
	t->random.increment = ((intptr_t)&tny_reset + stream) | 1ULL;
	stream++;

	/*
	 * Set initial state and "put it in the past"
	 */
	t->random.state = seed + t->random.increment;
	(void)tny_random(t);

	t->delay_cycles = 0;
	t->cycle_cnt = 0;

	return true;
}

void tny_modify_port_levels(teenyat *t, bool is_system_request, tny_word data, bool is_port_a) {
	tny_word *port = (is_port_a ? &(t->port_a) : &(t->port_b));
	tny_word dir = (is_port_a ? t->port_a_directions : t->port_b_directions);

	/* 
	 * For the masking below, we need a bit pattern of all 1s or 0s depending
	 * on whether this is a system request (1s) or not (0s).
	 */
	tny_word src;
	src.u = ~0 * is_system_request;

	/* We must know for which bits the source of the request and direction
	 * bits match.  Eg, if the system wants to modify the port levels,
	 * it can only do that for port bits with their direction set for input.
	 */
	tny_word src_dir_matches;
	src_dir_matches.u = src.u ^ dir.u;

	/* Save a copy of the port bits to determine whether any change */
	tny_word old_port = *port;

	/* Modify only those bits which should change */
	port->u = (port->u & src_dir_matches.u) + (data.u & ~src_dir_matches.u);

	/* Launch the port change callback if any port bits were modify */
	if((t->port_change != NULL) && (~src.u & ~dir.u & (old_port.u ^ port->u))) {
		t->port_change(t, is_port_a, *port);
	}

	return;
}

void tny_port_change(teenyat *t, TNY_PORT_CHANGE_FNPTR port_change) {
	t->port_change = port_change;

	return;
}

void tny_get_ports(teenyat *t, tny_word *a, tny_word *b) {
	if(a != NULL) {
		*a = t->port_a;
	}

	if(b != NULL) {
		*b = t->port_b;
	}

	return;
}

void tny_set_ports(teenyat *t, tny_word *a, tny_word *b) {
	if(a != NULL) {
		tny_modify_port_levels(t, true, *a, true);
	}

	if(b != NULL) {
		tny_modify_port_levels(t, true, *b, false);
	}

	return;
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
	tny_uword reg2 = IR.instruction.reg2;
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
			switch(addr) {
			case TNY_PORTA_ADDRESS:
				t->reg[reg1] = t->port_a;
				break;
			case TNY_PORTB_ADDRESS:
				t->reg[reg1] = t->port_b;
				break;
			case TNY_PORTA_DIR_ADDRESS:
				t->reg[reg1] = t->port_a_directions;
				break;
			case TNY_PORTB_DIR_ADDRESS:
				t->reg[reg1] = t->port_b_directions;
				break;
			case TNY_RANDOM_ADDRESS:
				t->reg[reg1].u = tny_random(t);
				break;
			default:
				if(addr >= TNY_PERIPHERAL_BASE_ADDRESS) {
					/* read from peripheral address */
					tny_word data = {.u = 0};
					uint16_t delay = 0;
					t->bus_read(t, addr, &data, &delay);
					t->reg[reg1] = data;
					t->delay_cycles += delay;
				}
				else if(addr <= TNY_MAX_RAM_ADDRESS) {
					/* read from RAM */
					t->reg[reg1] = t->ram[addr];
				}
				else {
					/* 
					 * This is an attempt to access an unaccounted for
					 * address in the "Microcontroller Device Space".
					 */
				}
				break;
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
			switch(addr) {
			case TNY_PORTA_ADDRESS:
				tny_modify_port_levels(t, false, t->reg[reg2], true);
				break;
			case TNY_PORTB_ADDRESS:
				tny_modify_port_levels(t, false, t->reg[reg2], false);
				break;
			case TNY_PORTA_DIR_ADDRESS:
				t->port_a_directions = t->reg[reg2];
				break;
			case TNY_PORTB_DIR_ADDRESS:
				t->port_b_directions = t->reg[reg2];
				break;
			case TNY_RANDOM_ADDRESS:
				/* Do nothing */
				break;
			default:
				if(addr >= TNY_PERIPHERAL_BASE_ADDRESS) {
					/* write to peripheral address */
					uint16_t delay = 0;
					t->bus_write(t, addr, t->reg[reg2], &delay);
					t->delay_cycles += delay;
				}
				else if(addr <= TNY_MAX_RAM_ADDRESS) {
					/* write to RAM */
					t->ram[addr] = t->reg[reg2];
				}
				else {
					/* 
					 * This is an attempt to access an unaccounted for
					 * address in the "Microcontroller Device Space".
					 */
				}
				break;
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
				t->reg[reg1].s |= (1 << bit);
				set_elg_flags(t, t->reg[reg1].s);
			}
		}
		break;
	case TNY_OPCODE_BTC:
		{
			tny_sword bit = t->reg[reg2].s + immed;
			if(bit >= 0 && bit <= 15) {
				t->reg[reg1].s &= ~(1 << bit);
				set_elg_flags(t, t->reg[reg1].s);
			}
		}
		break;
	case TNY_OPCODE_BTF:
		{
			tny_sword bit = t->reg[reg2].s + immed;
			if(bit >= 0 && bit <= 15) {
				t->reg[reg1].s ^= (1 << bit);
				set_elg_flags(t, t->reg[reg1].s);
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
		if(t->reg[reg2].s + immed != 0) {
			t->reg[reg1].s /= t->reg[reg2].s + immed;
			set_elg_flags(t, t->reg[reg1].s);
		}
		else {
			/* No behavior defined on divide-by-zero */
		}
		break;
	case TNY_OPCODE_MOD:
		if(t->reg[reg2].s + immed != 0) {
			t->reg[reg1].s %= t->reg[reg2].s + immed;
			set_elg_flags(t, t->reg[reg1].s);
		}
		else {
			/* No behavior defined on divide-by-zero */
		}
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
				set_pc(t, t->reg[reg1].s + immed);
			}
		}
		break;
	case TNY_OPCODE_LUP:
		tmp = (uint32_t)(t->reg[reg1].s) - 1;
		t->flags.carry = tmp & (1 << 16);
		t->reg[reg1].s = tmp;
		set_elg_flags(t, (tny_sword)tmp);
		if(tmp != 0) {
			set_pc(t, t->reg[reg2].s + immed);
		}
		break;
	case TNY_OPCODE_DLY:
		{
			tny_sword delay_cnt = t->reg[reg2].s + immed;
			if(delay_cnt >= 1) {
				t->delay_cycles = delay_cnt - 1; // current instruction already 1 cycle
			}
		}
		break;
	default:
		fprintf(stderr, "Unknown opcode (%d) encountered at 0x%04X on cycle %" PRIu64 "\n",
		        opcode, orig_PC, t->cycle_cnt);
		break;
	}

	/* Ensure the zero register still has a zero in it */
	t->reg[TNY_REG_ZERO].u = 0;

	return;
}

tny_uword tny_random(teenyat *t) {
    uint64_t tmp = t->random.state;

    /*
	 * Find the next state in the sequence.  The weird large immediate value
	 * is a special constant chosen by the world at large to do great
	 * things for the random number. ... We don't really know about it,
	 * but it seems to work ;-)
	 */
    t->random.state = tmp * 6364136223846793005ULL + t->random.increment;

	/*
	 * The code below involves some shifts of seemingly random amounts.
	 * They determine the amounts of manioulation of 64 and 16 bit values.
	 * 27 = 32 - 5
	 * 18 = floor((64 - 27) / 2)
	 * 59 = 64 - 5
	 */

    /* use top 5 bits of previous state to "randomly" rotate */
    unsigned bitcnt_to_rotate = tmp >> 59;

    /* scramble the previous state and truncate to 16 bits */
    uint32_t to_rotate = (tmp >> 18 ^ tmp) >> 27;

    /* return the right-rotated the scrambled previous state */
	uint32_t result = to_rotate >> bitcnt_to_rotate;
	/* the bitmask below ensures the left shift is fewer than 32 bits */
	result |= to_rotate << (-bitcnt_to_rotate & ~(~0U << 5));

	/* truncate result and return as 16-bit tny_uword */
    return (tny_uword)result;
}
