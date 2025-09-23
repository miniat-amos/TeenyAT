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

###### note: certain instructions do not follow this format see [`STR`](./instructions/str.md)

<a name="opcode-table"/>

### Opcodes
###### note: value could be either a register or an immediate 

| Opcode  | Description                  | Example                  |
|---------|------------------------------|--------------------------|
| [`SET`](./instructions/set.md)  | Set register to value | `set rA, 10`          |
| [`LOD`](./instructions/lod.md) | Load from memory into register  | `lod rA, [0x1000]`     |
| [`STR`](./instructions/str.md)  | Store to memory from register | `str [0x1000], rA`   |
| [`PSH`](./instructions/psh.md)   | Push value to stack | `psh rA`       |
| [`POP`](./instructions/pop.md)   | Pop value from stack | `pop rA`       |
| [`BTS`](./instructions/bts.md)   | Sets bit level in register  | `bts rA, 1`              |
| [`BTC`](./instructions/btc.md)   | Clear bit level in register | `btc rA, 15`            |
| [`BTF`](./instructions/btf.md)  | Flip bit level in register             | `btf rA, 10`             |
| [`CAL`](./instructions/cal.md)   | Call a subroutine        | `cal !print_string`                   |
| [`ADD`](./instructions/add.md)   | Add value to register        | `add rA, rB`                   |
| [`SUB`](./instructions/sub.md)   | Subtract value from register        | `sub rA, 12`      |
| [`MPY`](./instructions/mpy.md)   | Multiply register by value        | `mpy rA, rC`        |
| [`DIV`](./instructions/div.md)   | Divide register by value        | `div rA, 10`       |
| [`MOD`](./instructions/mod.md)   | Modulus register by value        | `mod rA, rD`     |
| [`AND`](./instructions/and.md)   | Bitwise AND register by value       | `and rA, rB`  |
| [`OR`](./instructions/or.md)   | Bitwise OR register by value        | `or rA, 1`    |
| [`XOR`](./instructions/xor.md)   | Bitwise XOR register by value        | `xor rA, rA`        |
| [`SHF`](./instructions/shf.md)   | Bitwise SHIFT register by value        | `shf rA, -2`     |
| [`ROT`](./instructions/rot.md)   | Bitwise ROTATE register by value    | `rot rA, 2`      |
| [`NEG`](./instructions/neg.md)   | Negate register's value       | `neg rA`                   |
| [`CMP`](./instructions/cmp.md)   | Compare register to value        | `cmp rA, 10`     |
| [`JMP`](./instructions/jmp.md)   | Set program counter to value        | `jmp !main_loop`|
| [`LUP`](./instructions/lup.md)   | Loop to label register amount     | `lup rA, !kill_loop`|
| [`DLY`](./instructions/dly.md)   | Delay value amount of cycles        | `dly rA`|
| [`INT`](./instructions/int.md)   | Generate an interrupt, 0..15      | `int 5`|
| [`RTI`](./instructions/rti.md)   | Return from an interrupt handler   | `rti`|

<a name="pseudo-table"/>

### Pseudo Instructions
- These have no physical opcode but are instead synthesized using other ones  
  ###### note: Try to figure out how if you want a practice challenge :)  

| Opcode  | Description                  | Example                  |
|---------|------------------------------|--------------------------|
| [`INC`](./instructions/inc.md)  | Increment register by 1 | `inc rA`          |
| [`DEC`](./instructions/dec.md) | Decrement register by 1  | `dec rA`     |
| [`RET`](./instructions/ret.md)  | Return from subroutine | `ret`   |
| [`SHL`](./instructions/shl.md)   | Bitwise LEFT SHIFT register by immediate  | `shl rA, 1`       |
| [`SHR`](./instructions/shr.md)   | Bitwise RIGHT SHIFT register by immediate | `shr rA, 5`       |
| [`ROL`](./instructions/rol.md)   | Bitwise LEFT ROTATE register by immediate  | `rol rA, 6`    |
| [`ROR`](./instructions/ror.md)   | Bitwise RIGHT ROTATE register by immediate | `ror rA, 15`    |
| [`JE`](./instructions/je.md)  | Set program counter if cmp was equal      | `je !here`   |
| [`JNE`](./instructions/jne.md)   | Set program counter if cmp was not equal| `jne !there`  |
| [`JL`](./instructions/jl.md)   | Set program counter if cmp was less than | `jl !where`  |
| [`JLE`](./instructions/jle.md)   | Set program counter if cmp was less than or equal| `jle !when`  |
| [`JG`](./instructions/jg.md)   | Set program counter if cmp was greater than  | `jg !who`  |
| [`JGE`](./instructions/jge.md)   | Set program counter if cmp was greater than or equal | `jge !what` |

###### note: Remember that labels represent the memory address of the line of code below them
```assembly
!my_label
    add rZ, rZ

!my_other_label
    sub rZ, rZ
```

##### The value of !my_label is equal to 0 and the value of !my_other_label is equal 1