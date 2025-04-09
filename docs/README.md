## Introduction
This document provides a complete overview of the syntax, instructions, and usage of  **TeenyAT Assembly** for our virtual embedded microcontroller.

---

## Syntax Overview
Each instruction follows a standard format:


- **OPCODE**: The operation to be performed.
- **OPERANDS**: Registers, immediate values, or memory addresses.
- Most instructions are able to be written in the style of:

    `opcode register, immediate` <br>
    `opcode register, register` <br>
    `opcode register, register + immediate`

###### note: certain instructions do not follow this format see [`STR`](#str)

<a name="opcode-table"/>

### Opcodes
###### note: value could be either a register or an immediate 


| Opcode  | Description                  | Example                  |
|---------|------------------------------|--------------------------|
| [`SET`](#set)  | Set register to value | `set rA, 10`          |
| [`LOD`](#lod) | Load from memory into register  | `lod rA, [0x1000]`     |
| [`STR`](#str)  | Store to memory from register | `str [0x1000], rA`   |
| [`PSH`](#psh)   | Push value to stack | `psh rA`       |
| [`POP`](#pop)   | Pop value from stack | `pop rA`       |
| [`BTS`](#bts)   | Sets bit level in register  | `bts rA, 1`              |
| [`BTC`](#btc)   | Clear bit level in register | `btc rA, 15`            |
| [`BTF`](#btf)  | Flip bit level in register             | `btf rA, 10`             |
| [`CAL`](#cal)   | Call a subroutine        | `cal !print_string`                   |
| [`ADD`](#add)   | Add value to register        | `add rA, rB`                   |
| [`SUB`](#sub)   | Subtract value from register        | `sub rA, 12`      |
| [`MPY`](#mpy)   | Multiply register by value        | `mpy rA, rC`        |
| [`DIV`](#div)   | Divide register by value        | `div rA, 10`       |
| [`MOD`](#mod)   | Modulus register by value        | `mod rA, rD`     |
| [`AND`](#and)   | Bitwise AND register by value       | `and rA, rB`  |
| [`OR`](#or)   | Bitwise OR register by value        | `or rA, 1`    |
| [`XOR`](#xor)   | Bitwise XOR register by value        | `xor rA, rA`        |
| [`SHF`](#shf)   | Bitwise SHIFT register by value        | `shf rA, -2`     |
| [`ROT`](#rot)   | Bitwise ROTATE register by value    | `rot rA, 2`      |
| [`NEG`](#neg)   | Negate register's value       | `neg rA`                   |
| [`CMP`](#cmp)   | Compare register to value        | `cmp rA, 10`     |
| [`JMP`](#jmp)   | Set program counter to value        | `jmp !main_loop`|
| [`LUP`](#lup)   | Loop to label register amount     | `lup rA, !kill_loop`|
| [`DLY`](#dly)   | Delay value amount of cycles        | `dly rA`|

<a name="pseudo-table"/>

### Pseudo Instructions
- These have no physical opcode but are instead synthesized using other ones
    ###### note: Try to figure out how if you want a practice challenge :)  

| Opcode  | Description                  | Example                  |
|---------|------------------------------|--------------------------|
| [`INC`](#inc)  | Increment register by 1 | `inc rA`          |
| [`DEC`](#dec) | Decrement register by 1  | `dec rA`     |
| [`RET`](#ret)  | Return from subroutine | `ret`   |
| [`SHL`](#shl)   | Bitwise LEFT SHIFT register by immediate  | `shl rA, 1`       |
| [`SHR`](#shr)   | Bitwise RIGHT SHIFT register by immediate | `shr rA, 5`       |
| [`ROL`](#rol)   | Bitwise LEFT ROTATE register by immediate  | `rol rA, 6`    |
| [`ROR`](#ror)   | Bitwise RIGHT ROTATE register by immediate | `ror rA, 15`    |
| [`JE`](#je)  | Set program counter if cmp was equal      | `je !here`   |
| [`JNE`](#jne)   | Set program counter if cmp was not equal| `jne !there`  |
| [`JL`](#jl)   | Set program counter if cmp was less than | `jl !where`  |
| [`JLE`](#jle)   | Set program counter if cmp was less than or equal| `jle !when`  |
| [`JG`](#jg)   | Set program counter if cmp was greater than  | `jg !who`  |
| [`JGE`](#jge)   | Set program counter if cmp was greater than or equal | `jge !what` |


###### note: Remember that labels are just textaual replacements for memory addresses
    !my_label
        add rZ, rZ
        
    !my_other_label
        sub rZ, rZ
The value of !my_label in this instance is zero and it points to the first line of code