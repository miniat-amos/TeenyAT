/*
 * Name	   : teenyat.h
 * Author	 : William "Amos" Confer
 *
 * License	: Copyright (C) 2022 All rights reserved
 */

#ifndef __TEENYAT_H__
#define __TEENYAT_H__

#ifndef __cplusplus

#include <stdint.h>
#include <stdbool.h>

#else  // __cplusplus
extern "C" {

#include <cstdint>

#endif // __cplusplus

typedef struct teenyat teenyat;

typedef uint16_t tny_uword;
typedef int16_t tny_sword;
typedef union tny_word tny_word;

#define TNY_RAM_SIZE (1 << 15)

struct teenyat {
	bool initilized;  /* has this TeenyAT ever been initialized */
	tny_word ram[TNY_RAM_SIZE];
	tny_word bin_image[TNY_RAM_SIZE];  /* copy of original bin file for resets */
	/*
	 * Registers...
	 * reg[0]: Program Counter (PC)
	 * reg[1]: Stack Pointer (SP)... address of the next top
	 * reg[2]: Zero Register (rZ)... always contains zero and is read only
	 * reg[3]-[7]: General Purpose Registers (rA -- rE)
	 */
	tny_word reg[8];
	struct {
		/*
		 * Flag bits are set by CMP and all ALU instructions.
		 */
		bool carry;
		bool equals;
		bool less;
		bool greater;
	} flags;
	tny_word instruction;
	tny_word immed;
	unsigned int delay_cycles;  /* remaining cycles for previous instruction */
};

union tny_word {
	struct {
		int16_t immed4  : 4;
		uint16_t reg2   : 3;
		uint16_t reg1   : 3;
		uint16_t teeny  : 1;
		uint16_t opcode : 5;
	} instruction;

	struct {
		uint16_t greater  : 1;
		uint16_t less     : 1;
		uint16_t equals   : 1;
		uint16_t carry    : 1;
		uint16_t reserved : 12;
	} inst_flags;

	struct {
		uint16_t byte0 : 8;
		uint16_t byte1 : 8;
	} bytes;

	tny_uword u;
	tny_sword s;

	struct {
		uint16_t bit0  : 1;
		uint16_t bit1  : 1;
		uint16_t bit2  : 1;
		uint16_t bit3  : 1;
		uint16_t bit4  : 1;
		uint16_t bit5  : 1;
		uint16_t bit6  : 1;
		uint16_t bit7  : 1;
		uint16_t bit8  : 1;
		uint16_t bit9  : 1;
		uint16_t bit10 : 1;
		uint16_t bit11 : 1;
		uint16_t bit12 : 1;
		uint16_t bit13 : 1;
		uint16_t bit14 : 1;
		uint16_t bit15 : 1;
	} bits;
};

#define TNY_OPCODE_SET 0
#define TNY_OPCODE_LOD 1
#define TNY_OPCODE_STR 2
#define TNY_OPCODE_PSH 3
#define TNY_OPCODE_POP 4
#define TNY_OPCODE_BTS 5
#define TNY_OPCODE_BTC 6
#define TNY_OPCODE_BTF 7
#define TNY_OPCODE_CAL 8
#define TNY_OPCODE_ADD 9
#define TNY_OPCODE_SUB 10
#define TNY_OPCODE_MPY 11
#define TNY_OPCODE_DIV 12
#define TNY_OPCODE_MOD 13
#define TNY_OPCODE_AND 14
#define TNY_OPCODE_OR  15
#define TNY_OPCODE_XOR 16
#define TNY_OPCODE_SHF 17
#define TNY_OPCODE_ROT 18
#define TNY_OPCODE_NEG 19
#define TNY_OPCODE_CMP 20
#define TNY_OPCODE_JMP 21
#define TNY_OPCODE_DJZ 22

/**
 * Initialize a TeenyAT instance and buffer the file for future resets
 *
 * @param t
 *   The TeenyAT instance to initialize
 *
 * @param bin_file
 *   The pre-assembled .bin file to load and execute
 *
 * @return
 *   True on success, flase otherwise.
 */
bool tny_init_from_file(teenyat *t, FILE *bin_file);

/**
 * Restore the TeenyAT to its initialized state as if it had just been done so
 * from the original .bin file
 *
 * @param t
 *   The TeenyAT instance to reset
 *
 * @return
 *   True on success, flase otherwise.
 */
bool tny_reset(teenyat *t);

/**
 * Advance the TeenyAT instance by one clock cycle
 *
 * @param t
 *   The TeenyAT instance
 *
 * @return
 *   True if the TeenyAT instance is making a bus access, false otherwise.
 *   Attempting to reset an unitialized TeenyAT will always return false.
 *
 * @note
 *   The caller MUST check t's bus state for whether the access is a read or
 *   write.  Read/writes to/from external devices must be handled immediately
 *   (before calling tny_clock() again.  On read requests, however, the caller
 *   must satisfy the request via tny_read_result().
 */
bool tny_clock(teenyat *t);

#ifdef __cplusplus
}
#endif

#endif /* __TEENYAT_H__ */
