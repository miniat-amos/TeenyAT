/*
 * Name	   : teenyat.h
 * Author	 : William "Amos" Confer
 *
 * License	: Copyright (C) 2023 All rights reserved
 */

#ifndef __TEENYAT_H__
#define __TEENYAT_H__

#ifdef __cplusplus

extern "C" {

#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

typedef struct teenyat teenyat;

typedef uint16_t tny_uword;
typedef int16_t tny_sword;
typedef union tny_word tny_word;

/**
 * @brief
 *   System calllback function to handle TeenyAT read requests
 *
 * @param t
 *   The TeenyAT instance making the request
 *
 * @param addr
 *   Address of the read request
 *
 * @param[out] data
 *   Output the data result of the request.  If the requested address is not
 *   simulated or otherwise illegal (eg, not meant as an input address) it is
 *   up to the callback function what value to return here.
 *
 * @param delay
 *   Use this to tell the TeenyAT how many additional cycles of cost you want
 *   associated with this read request.  For example, if you want the device
 *   being read from to have a penalty of 9 clock cycles on top of the particular
 *   instruction used, pass 9 as the delay.  The TeenyAT will ensure this
 *   many cycles pass before the next instruction is executed.
 */
typedef void(*TNY_READ_FROM_BUS_FNPTR)(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay);

/**
 * @brief
 *   System calllback function to handle TeenyAT write requests
 *
 * @param t
 *   The TeenyAT instance making the request
 *
 * @param addr
 *   Address of the write request
 *
 * @param data
 *   The data to be sent to the address.  If the requested address is not
 *   simulated or otherwise illegal (eg, not meant as an input address) it is
 *   up to the callback function whether to ignore the request or do something
 * else.
 *
 * @param delay
 *   Use this to tell the TeenyAT how many additional cycles of cost you want
 *   associated with this write request.  For example, if you want the device
 *   being written to to have a penalty of 7 clock cycles on top of the particular
 *   instruction used, pass 7 as the delay.  The TeenyAT will ensure this
 *   many cycles pass before the next instruction is executed.
 */
typedef void(*TNY_WRITE_TO_BUS_FNPTR)(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay);

/**
 * @brief
 *   System calllback function to handle TeenyAT output port pin changes
 *
 * Whenever a store instruction (STR) writes to one of the two GPIO port addresses,
 * values in bits identified for output may change.  If any are actually modified
 * as a result of the store, this registered callback would be run.  It is the
 * responsibility of the callback author to determine which bits have changed
 * (and to what).
 * 
 * @param t
 *   The TeenyAT instance making the request
 *
 * @param is_port_a
 *   Used to determine whether the levels changed because of a modificatio
 *   to port A (true) or port B (false).
 *
 * @param port
 *   The updated externally held pin levels for the modified port
 */
typedef void(*TNY_PORT_CHANGE_FNPTR)(teenyat *t, bool is_port_a, tny_word port);

/* While the TeenyAT has a 16 bit address space, RAM is only 32K words */
#define TNY_RAM_SIZE 0x8000
#define TNY_MAX_RAM_ADDRESS 0x7FFF

#define TNY_PORTA_DIR_ADDRESS 0x8000
#define TNY_PORTB_DIR_ADDRESS 0x8001
#define TNY_PORTA_ADDRESS 0x8002
#define TNY_PORTB_ADDRESS 0x8003

#define TNY_RANDOM_ADDRESS 0x8010  /* positive random values */
#define TNY_RANDOM_BITS_ADDRESS 0x8011  /* random 16-bit pattern */

#define TNY_CYCLE_COUNT 0x8090
#define TNY_CYCLE_COUNT_RESET 0x8091

#define TNY_WALL_TIME 0x8094
#define TNY_WALL_TIME_RESET 0x8095

#define TNY_CONTROL_STATUS_REGISTER 0x8EFF

#define TNY_INTERRUPT_VECTOR_TABLE_START 0x8E00
#define TNY_INTERRUPT_VECTOR_TABLE_END 0x8E0F
#define TNY_INTERRUPT_CNT (TNY_INTERRUPT_VECTOR_TABLE_END - TNY_INTERRUPT_VECTOR_TABLE_START + 1)
#define TNY_INTERRUPT_ENABLE_REGISTER 0x8E10
#define TNY_INTERRUPT_QUEUE_REGISTER 0x8E11


#define TNY_PERIPHERAL_BASE_ADDRESS 0x9000

/*
* To promote student use of registers, all bus operations,
* including RAM access comes with an extra penalty.
*/
#define TNY_BUS_DELAY 1
#define TNY_BUS_EXTERNAL_DELAY_ADJUST 2

#define TNY_DEFAULT_CALIBRATE_CYCLES 500

typedef struct alu_flags {
	bool greater : 1;
	bool less    : 1;
	bool equals  : 1;
	bool carry   : 1;
	int reserved: 12;
} alu_flags;

typedef enum tny_xint {
    TNY_XINT0,
    TNY_XINT1,
    TNY_XINT2,
    TNY_XINT3,
    TNY_XINT4,
    TNY_XINT5,
    TNY_XINT6,
    TNY_XINT7
} tny_xint;

union tny_word {
	struct {
		tny_sword immed4  : 4;
		tny_uword reg2   : 3;
		tny_uword reg1   : 3;
		tny_uword teeny  : 1;
		tny_uword opcode : 5;
	} instruction;

	struct {
		tny_uword greater  : 1;
		tny_uword less     : 1;
		tny_uword equals   : 1;
		tny_uword carry    : 1;
		tny_uword reserved : 12;
	} inst_flags;

	/* the Control Status Register (CSR) */
	struct {
		tny_uword interrupt_enable  : 1;
		tny_uword interrupt_clearing  : 1;
		tny_uword clock_divisor_scale : 4;
		tny_uword unclocked : 1;
		tny_uword reserved : 9;
	} csr;

	struct {
		tny_uword byte0 : 8;
		tny_uword byte1 : 8;
	} bytes;

	tny_uword u;
	tny_sword s;

	struct {
		tny_uword bit0  : 1;
		tny_uword bit1  : 1;
		tny_uword bit2  : 1;
		tny_uword bit3  : 1;
		tny_uword bit4  : 1;
		tny_uword bit5  : 1;
		tny_uword bit6  : 1;
		tny_uword bit7  : 1;
		tny_uword bit8  : 1;
		tny_uword bit9  : 1;
		tny_uword bit10 : 1;
		tny_uword bit11 : 1;
		tny_uword bit12 : 1;
		tny_uword bit13 : 1;
		tny_uword bit14 : 1;
		tny_uword bit15 : 1;
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
#define TNY_OPCODE_LUP 22
#define TNY_OPCODE_DLY 23
#define TNY_OPCODE_INT 24
#define TNY_OPCODE_RTI 25

#define TNY_REG_ZERO 0
#define TNY_REG_PC   1
#define TNY_REG_SP   2
#define TNY_REG_A    3
#define TNY_REG_B    4
#define TNY_REG_C    5
#define TNY_REG_D    6
#define TNY_REG_E    7

/**
 * @brief
 *   Initialize a 1 MHz instance of the TeenyAT and 
 *   buffer the file for future resets.
 *
 *
 * @param bin_file
 *   The pre-assembled .bin file to load and execute
 *
 * @param bus_read
 *   Callback function for handling read requests
 *
 * @param bus_write
 *   Callback function for handling write requests
 *
 * @return
 *   The initialized instance of the TeenyAT
 *
 * @note
 *   Upon failed initialization, the t->initialized member can be assumed false,
 *   but the state of all other members is undefined.
 */
teenyat* tny_init_from_file(FILE *bin_file,
                        TNY_READ_FROM_BUS_FNPTR bus_read,
                        TNY_WRITE_TO_BUS_FNPTR bus_write);

/**
 * @brief
 *   Initialize an unclocked instance of TeenyAT.
 *
 * @param bin_file
 *   The pre-assembled .bin file to load and execute
 *
 * @param bus_read
 *   Callback function for handling read requests
 *
 * @param bus_write
 *   Callback function for handling write requests
 *
 * @return
 *   The initialized instance of the TeenyAT
 *
 * @note
 *   Upon failed initialization, the t->initialized member can be assumed false,
 *   but the state of all other members is undefined.
 */
teenyat* tny_init_unclocked(FILE *bin_file,
						TNY_READ_FROM_BUS_FNPTR bus_read,
						TNY_WRITE_TO_BUS_FNPTR bus_write);

/**
 * @brief
 *   Reinitialize the TeenyAT
 *
 * Restore the TeenyAT to its initialized state as if it had just been done so
 * from the original .bin file
 *
 * @param t
 *   The TeenyAT instance to reset
 *
 * @return
 *   True on success, false otherwise.
 *   Attempting to reset an uninitialized TeenyAT will always return false.
 */
bool tny_reset(teenyat *t);

/**
 * @brief
 *   Advance the TeenyAT instance by one clock cycle
 *
 * This function provides the TeenyAT with something similar to a CPU clock
 * pulse.  The TeenyAT will handle all the internals of the traditional Fetch,
 * Decode, and Execute processes, but it is the caller's responsibility to
 * check whether an any bus requests were made after EVERY cycle.
 *
 * The caller MUST check t's bus state for whether the access is a read or
 * write.  Read/writes from/to external devices must be handled immediately...
 * before calling tny_clock() again.  Check the character in t->bus.state to
 * determine the type of bus access request.  'R' indicates t was trying to read
 * from the device at the address t->bus.address.  'W' indicates t was trying to
 * write the data in t->bus.data to the device at the address t->bus.address.
 * the state member may also contain 'X', indicating no request at all.
 *
 * On read requests, the caller MUST satisfy the request via tny_lod_result().
 *
 * On write requests, the caller MUST satisfy the request via tny_str_result().
 *
 * @param t
 *   The TeenyAT instance
 */
void tny_clock(teenyat *t);

/**
 * @brief
 *   Get the current bit levels on ports A and B.
 *
 * @param t
 *   The TeenyAT instance
 * 
 * @param[out] a
 *   Returns the current bit levels held on port A
 * 
 * @param[out] b
 *   Returns the current bit levels held on port B
 * 
 * @note
 *   A NULL tny_word pointer argument identifies that port is to be ignored.
 */
void tny_get_ports(teenyat *t, tny_word *a, tny_word *b);

/**
 * @brief
 *   Set the current bit levels on ports A and B for those pins operating
 *   as input pins.
 *
 * @param t
 *   The TeenyAT instance
 *
 * @param a
 *   New potential bits levels for port A
 *
 * @param b
 *   Returns the current bit levels held on port B
 *
 * @note
 *   A NULL tny_word pointer argument identifies that port is to be ignored.
 */
void tny_set_ports(teenyat *t, tny_word *a, tny_word *b);

/**
 * @brief
 *   Register a callback for external port level changes
 *
 * @param t
 *   The TeenyAT instance
 *
 * @param port_change
 *   Callback for handling external port level changes
 */
void tny_port_change(teenyat *t, TNY_PORT_CHANGE_FNPTR port_change);

/**
 * @brief
 *   Trigger an external interrupt
 *
 * @param t
 *   The TeenyAT instance
 *
 * @param external_interrupt
 *   A number from 0-7 denoting which external interrupt to queue
 */
void tny_external_interrupt(teenyat* t, tny_xint external_interrupt);

#ifdef __cplusplus
}
#endif

#endif /* __TEENYAT_H__ */
