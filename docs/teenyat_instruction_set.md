# **TeenyAT Instruction Set Architecture**

## Core Elements
    16 bit words (each memory address points to a 16 bit value)

    32K words of RAM from 0x0000 through 0x7FFF
        * Address 0x8000-0xFFFF: available for hardware access

    8 registers available to all register-use instructions
        * rZ (reg[0]) is the "Zero" register, which always contains the value 0.
            * all attempts to modify rZ are ignored 
        * PC (reg[1]) is the program counter, contains address of next instruction, initially 0x0000
        * SP (reg[2]) is the stack pointer, contains address of the next top element, initially 0x7FFF (empty)
            * stack grows down in memory
        * rA through rE (reg[3] through reg[7]) are general purpose registers

    Instructions encoded in either one or two 16 bit words
        * Fetch must set PC = PC + 2 - T, where T is the "teeny" bit  to account for this
            * if teeny == 1, the instruction is 16 bits (essentially assumes 2nd word is 0x0000)
            * if teeny == 0, the instruction is 32 bits and requires an additional cycle
        * Fetches to RAM greater than 0x7FFF will result in the PC being set to 0

## Opcode Table
 Instruction Opcodes

***Table:** Quick Referece for all the Opcodes and Flags set by Each Instruction*
|       Instructions             |   Opcode     |
|--------------------------------|:------------:|
|      SET                       |     0        |
|      LOD                       |     1        |
|      STR                       |     2        |
|      PSH                       |     3        |
|      POP                       |     4        |
|      BTS                       |     5        |
|      BTC                       |     6        |
|      BTF                       |     7        |
|      CAL                       |     8        |
|      ADD                       |     9        |
|      SUB                       |     10       |
|      MPY                       |     11       |
|      DIV                       |     12       |
|      MOD                       |     13       |
|      AND                       |     14       |
|      OR                        |     15       |
|      XOR                       |     16       |
|      SHF                       |     17       |
|      ROT                       |     18       |
|      NEG                       |     19       |
|      CMP                       |     20       |
|      JMP                       |     21       |
|      DLY                       |     22       |
|      LUP                       |     23       |

## Encodings

### First Word
```
     -------------------------------------------------------------------------------------
    | 15 | 14 | 13 | 12 | 11 |   10  | 9 | 8 | 7 | 6 | 5 | 4 |   3   |   2  |   1  |   0  |
    |-------------------------------------------------------------------------------------|
    |          opcode        | Teeny |   Reg_1   |   Reg_2   | Carry | Eqls | Less | Grtr |
    |-------------------------------------------------------------------------------------|
    |    |    |    |    |    |       |   |   |   |   |   |   |      Immed_4 / Addr_4      |
     -------------------------------------------------------------------------------------
```

### Second Word                                             
```                                                
     -------------------------------------------------------------------------------------
    | 15 | 14 | 13 | 12 | 11 |   10  | 9 | 8 | 7 | 6 | 5 | 4 |   3   |   2  |   1  |   0  |
    |-------------------------------------------------------------------------------------|
    |                                Immed_16 / Addr_16                                   |
    |-------------------------------------------------------------------------------------|
    |    |    |    |    |    |       |   |   |   |   |   |   |       |      |      |      |
     -------------------------------------------------------------------------------------
```

## Instruction Overview
Instruction Set
### 4.1 SET
- Opcode: 0
- Desc: Sets a register to a value
- Flags: N/A
- Usage: 
    - SET register1, register2 + immed
    - SET register1, register2
    - SET register1, immed
### 4.2 LOD
- Opcode: 1
- Desc: Loads a value from an address into register1
     -    If value is greater then 0x7FFF will read from bus
- Flags: N/A
- Usage: 
    - LOD register1, [register2 + immed]
    - LOD register1, [register2]
    - LOD register1, [immed]
### 4.3 STR
- Opcode: 2
- Desc: Stores a value from an address into a register2
     -    If value is greater then 0x7FFF will write to bus
- Flags: N/A
- Usage: 
    - STR [register1 + immed], register2
    - STR [register1], register2
    - STR [immed], register2
### 4.4 PSH
- Opcode: 3
- Desc: Sets SP to a value then Decrements the SP
- Flags: N/A
- Usage: 
    - PSH register2 + immed
    - PSH register2 
    - PSH immed
### 4.5 POP
- Opcode: 4
- Desc: Increments SP and puts the value into register1
- Flags: N/A
- Usage: 
    - POP register1 
### 4.6 BTS
- Opcode: 5
- Desc: Sets a bit in register1 to 1 based on the value if within range (0-15)
- Flags: Eqls,Less,Grtr
- On attempts to set an invalid bit acts as a nop.
- Usage: 
    - BTS register1, register2 + immed
    - BTS register1, register2
    - BTS register1, immed
### 4.7 BTC
- Opcode: 6
- Desc: Sets a bit in register1 to 0 based on the value if within range (0-15)
- Flags: Eqls,Less,Grtr
- On attempts to clear an invalid bit acts as a nop.
- Usage: 
    - BTC register1, register2 + immed
    - BTC register1, register2
    - BTC register1, immed
### 4.8 BTF
- Opcode: 7
- Desc: Toggles a bit in register1 based on the value if within range (0-15)
- Flags: Eqls,Less,Grtr
- On attempts to flip an invalid bit acts as a nop.
- Usage: 
    - BTF register1, register2 + immed
    - BTF register1, register2
    - BTF register1, immed
### 4.9 CAL
- Opcode: 8
- Desc: Sets SP to PC then Decrements SP and sets PC to an address
     - mainly used to set the pc to a label and then return from it later
- Flags: N/A
- Usage: 
    - CAL register2 + immed
    - CAL register2 
    - CAL immed 
### 4.10 ADD
- Opcode: 9
- Desc: Adds a value to register1
- Flags: Eqls,Less,Grtr
- Usage: 
    - ADD register1, register2 + immed
    - ADD register1, register2
    - ADD register1, immed
### 4.11 SUB
- Opcode: 10
- Desc: Subtracts a value from register1
- Flags: Eqls,Less,Grtr
- Usage: 
    - SUB register1, register2 + immed
    - SUB register1, register2
    - SUB register1, immed
### 4.12 MPY
- Opcode: 11
- Desc: Multiplies register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - MPY register1, register2 + immed
    - MPY register1, register2
    - MPY register1, immed
### 4.13 DIV
- Opcode: 12
- Desc: Divides register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - DIV register1, register2 + immed
    - DIV register1, register2
    - DIV register1, immed
### 4.14 MOD
- Opcode: 13
- Desc: Modulo register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - MOD register1, register2 + immed
    - MOD register1, register2
    - MOD register1, immed
### 4.15 AND
- Opcode: 14
- Desc: Bitwise AND register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - AND register1, register2 + immed
    - AND register1, register2
    - AND register1, immed
### 4.16 OR
- Opcode: 15
- Desc: Bitwise OR register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - OR register1, register2 + immed
    - OR register1, register2
    - OR register1, immed
### 4.17 XOR
- Opcode: 16
- Desc: Bitwise EXCLUSIVE OR register1 by a value
- Flags: Eqls,Less,Grtr
- Usage: 
    - XOR register1, register2 + immed
    - XOR register1, register2
    - XOR register1, immed
### 4.18 SHF
- Opcode: 17
- Desc: Bitwise SHIFT register1 by a value
     - If value is positive it will shift right (>>)
     - If value is negative it will shift left  (<<)
- Flags: Eqls,Less,Grtr
- Usage: 
    - SHF register1, register2 + immed
    - SHF register1, register2
    - SHF register1, immed
### 4.19 ROT
- Opcode: 18
- Desc: Bitwise ROTATE register1 by a value
     - If value is positive it will rotate right 
     - If value is negative it will rotate left  
- Flags: Eqls,Less,Grtr
- Usage: 
    - ROT register1, register2 + immed
    - ROT register1, register2
    - ROT register1, immed
### 4.20 NEG
- Opcode: 19
- Desc: Negates the value of register1 (0-register1)
- Flags: Eqls,Less,Grtr
- Usage: 
    - NEG register1
### 4.21 CMP
- Opcode: 20
- Desc: Compares register1 againts a value (does not store in register1)  
- Flags: Eqls,Less,Grtr
- Usage: 
    - CMP register1, register2 + immed
    - CMP register1, register2
    - CMP register1, immed
### 4.22 JMP
- Opcode: 21
- Desc: Sets PC to an address
     - The instance flags are set as: 
          - Equal = 0, Less = 0, Greater = 0 
- Flags: N/A
- Usage: 
    - JMP register1 + immed
    - JMP register1
    - JMP immed
### 4.23 DLY
- Opcode: 22
- Desc: Delays for a certain amount of clock cycles
- Flags: N/A
- Usage: 
    - DLY register2 + immed
    - DLY register2 
    - DLY immed
### 4.24 LUP
- Opcode: 23
- Desc: The "Loop" instruction.  Decrements register1 and jumps to the calculated target address if the register is non-zero
- Flags: N/A
- Usage: 
    - LUP register1, register2 + immed
    - LUP register1, register2 
    - LUP register1, immed

##***\*Pseudo instructions:** These instructions can be synthesized with a single different instruction. Reference the opcode for this.*
### 4.25 INC
- Opcode: 9
- Desc: Increments register1 
- Usage: 
    - INC register1
### 4.26 DEC
- Opcode: 10
- Desc: Decrements register1 
- Usage: 
    - DEC register1  
### 4.27 RET
- Opcode: 4
- Desc: Returns from a previous CAL instruction
- Usage: 
    - RET 

### 4.28 JE
- Opcode: 21
- Desc: Sets PC to address if EQUAL
     - The instance flags are set as: 
          - Equal = 1, Less = 0, Greater = 0 
- Flags: N/A
- Usage: 
    - JE register1 + immed
    - JE register1
    - JE immed   
### 4.29 JNE
- Opcode: 21
- Desc: Sets PC to address if NOT EQUAL
     - The instance flags are set as: 
          - Equal = 0, Less = 1, Greater = 1 
- Flags: N/A
- Usage: 
    - JNE register1 + immed
    - JNE register1
    - JNE immed    
### 4.30 JL
- Opcode: 21
- Desc: Sets PC to address if LESS
     - The instance flags are set as: 
          - Equal = 0, Less = 1, Greater = 0 
- Flags: N/A
- Usage: 
    - JL register1 + immed
    - JL register1
    - JL immed     
### 4.31 JLE
- Opcode: 21
- Desc: Sets PC to address if LESS OR EQUAL
     - The instance flags are set as: 
          - Equal = 1, Less = 1, Greater = 0 
- Flags: N/A
- Usage: 
    - JLE register1 + immed
    - JLE register1
    - JLE immed    
### 4.32 JG
- Opcode: 21
- Desc: Sets PC to address if GREATER
     - The instance flags are set as: 
          - Equal = 1, Less = 0, Greater = 1 
- Flags: N/A
- Usage: 
    - JG register1 + immed
    - JG register1
    - JG immed     
### 4.33 JGE
- Opcode: 21
- Desc: Sets PC to address if GREATER OR EQUAL
     - The instance flags are set as: 
          - Equal = 0, Less = 0, Greater = 1 
- Flags: N/A
- Usage: 
    - JGE register1 + immed
    - JGE register1
    - JGE immed

## Instruction Details
```
     ------------------------------------------------------------------------------------------------------------------------------------------
    |   ATRasm Instruction   |          Description           |                     Encoding                    | Teeny/Pseudo | Grammar Style |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | OPER dreg, sreg        | dreg ← dreg  oper sreg         | opcode ← OPER                                   |      T       |       1       |
    |                        |                                |     ADD:9, SUB:10, MPY:11, DIV:12, MOD:13,      |              |               |
    |                        |                                |     AND:14, OR:15, XOR:16, SHF:17, ROT:18       |              |               |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← 0                                     |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | OPER dreg, sreg + i16  | dreg ← dreg oper (sreg + i16)  | opcode ← OPER                                   |              |       2       |
    |                        |                                |     ADD:9, SUB:10, MPY:11, DIV:12, MOD:13,      |              |               |
    |                        |                                |     AND:14, OR:15, XOR:16, SHF:17, ROT:18       |              |               |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | OPER dreg, i16         | dreg ← dreg oper i16           | opcode ← OPER                                   |              |       3       |
    |                        |                                |     ADD:9, SUB:10, MPY:11, DIV:12, MOD:13,      |              |               |
    |                        |                                |     AND:14, OR:15, XOR:16, SHF:17, ROT:18       |              |               |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | OPER dreg, sreg + i4   | dreg ← dreg oper (sreg + i4)   | opcode ← OPER                                   |      T       |       2       |
    |                        |                                |     ADD:9, SUB:10, MPY:11, DIV:12, MOD:13,      |              |               |
    |                        |                                |     AND:14, OR:15, XOR:16, SHF:17, ROT:18       |              |               |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | OPER dreg, i4          | dreg ← dreg oper i4            | opcode ← OPER                                   |      T       |       3       |
    |                        |                                |     ADD:9, SUB:10, MPY:11, DIV:12, MOD:13,      |              |               |
    |                        |                                |     AND:14, OR:15, XOR:16, SHF:17, ROT:18       |              |               |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | NEG dreg               | Rdreg ← -Rdreg                 | opcode ← NEG:19                                 |      T       |       4       |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | INC dreg               | Rdreg ← Rdreg + 1              | opcode ← ADD:9                                  |     T,P      |       5       |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_16 ← 1                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | DEC dreg               | Rdreg ← -Rdreg - 1             | opcode ← SUB:10                                 |      T,P     |       5       |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_16 ← 1                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | SET dreg, sreg         | dreg ← sreg                    | opcode ← SET:0                                  |      T       |       1       |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← 0                                     |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | SET dreg, sreg + i16   | dreg ← sreg + i16              | opcode ← SET:0                                  |              |       2       |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | SET dreg, i16          | dreg ← i16                     | opcode ← SET:0                                  |              |       3       |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | SET dreg, sreg + i4    | dreg ← sreg + i4               | opcode ← SET:0                                  |      T       |       2       |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | SET dreg, i4           | dreg ← i4                      | opcode ← SET:0                                  |      T       |       3       |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | LOD dreg, [ sreg ]     | dreg ← mem[ sreg ]             | opcode ← LOD:1                                  |      T       |       1       |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← 0                                     |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | LOD dreg,[ sreg + i16 ]| dreg ← mem[ sreg + i16 ]       | opcode ← LOD:1                                  |              |       2       |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | LOD dreg, [ i16 ]      | dreg ← mem[ i16 ]              | opcode ← LOD:1                                  |              |       3       |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | LOD dreg,[ sreg + i4 ] | dreg ← mem[ sreg + i4 ]        | opcode ← LOD:1                                  |      T       |       2       |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | LOD dreg, [ i4 ]       | dreg ← mem[ i4 ]               | opcode ← LOD:1                                  |      T       |       3       |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | STR [ dreg ], sreg     | mem[ dreg ] ← sreg             | opcode ← STR:2                                  |      T       |       15      |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← 0                                     |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | STR[ dreg + i16 ], sreg| mem[ dreg + i16 ] ← sreg       | opcode ← STR:2                                  |              |       6       |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | STR [ i16 ], sreg      | mem[ i16 ] ← sreg              | opcode ← STR:2                                  |              |       7       |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← rZ                                      |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | STR[ dreg + i4 ], sreg | mem[ dreg + i4 ] ← sreg        | opcode ← STR:2                                  |      T       |       6       |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | STR [ i4 ], sreg       | mem[ i4 ] ← sreg               | opcode ← STR:2                                  |      T       |       7       |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← rZ                                      |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | PSH sreg               | Addr = SP & 0x7FFF             | opcode ← PSH:3                                  |      T       |       9       |
    |                        | mem[ Addr ] ← sreg             | Teeny ← 1                                       |              |               |
    |                        | SP ← (SP - 1) & 0x7FFF         | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← 0                                     |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | PSH sreg + i16         | Addr = SP & 0x7FFF             | opcode ← PSH:3                                  |              |       10      |
    |                        | mem[ Addr ] ← sreg + i16       | Teeny ← 0                                       |              |               |
    |                        | SP ← (SP - 1) & 0x7FFF         | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | PSH i16                | Addr = SP & 0x7FFF             | opcode ← PSH:3                                  |              |      11       |
    |                        | mem[ Addr ] ← i16              | Teeny ← 0                                       |              |               |
    |                        | SP ← (SP - 1) & 0x7FFF         | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | PSH sreg + i4          | Addr = SP & 0x7FFF             | opcode ← PSH:3                                  |      T       |       10      |
    |                        | mem[ Addr ] ← sreg + i4        | Teeny ← 1                                       |              |               |
    |                        | SP ← (SP - 1) & 0x7FFF         | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | PSH i4                 | Addr = SP & 0x7FFF             | opcode ← PSH:3                                  |      T       |      11       |
    |                        | mem[ Addr ] ← i4               | Teeny ← 0                                       |              |               |
    |                        | SP ← (SP - 1) & 0x7FFF         | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | POP dreg               | SP ← (SP + 1) & 0x7FFF         | opcode ← POP:4                                  |      T       |       4       |
    |                        | dreg ← mem[ SP ]               | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← dreg                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTS dreg, sreg         | B = sreg                       | opcode ← BTS:5                                  |      T       |       1       |
    |                        | if B in [0..15]:               | Teeny ← 1                                       |              |               |
    |                        |    dreg ← dreg | 1 << B        | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← 0                                     |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTS dreg, sreg + i16   | B = sreg + i16                 | opcode ← BTS:5                                  |              |       2       |
    |                        | if B in [0..15]:               | Teeny ← 0                                       |              |               |
    |                        |    dreg ← dreg | 1 << B        | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTS dreg, i16          | B = i16                        | opcode ← BTS:5                                  |              |       3       |
    |                        | if B in [0..15]:               | Teeny ← 0                                       |              |               |
    |                        |    dreg ← dreg | 1 << B        | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTS dreg, sreg + i4    | B = sreg + i4                  | opcode ← BTS:5                                  |      T       |       2       |
    |                        | if B in [0..15]:               | Teeny ← 1                                       |              |               |
    |                        |    dreg ← dreg | 1 << B        | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTS dreg, i4           | B = i4                         | opcode ← BTS:5                                  |      T       |       3       |
    |                        | if B in [0..15]:               | Teeny ← 0                                       |              |               |
    |                        |    dreg ← dreg | 1 << B        | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTC dreg, sreg         | B = sreg                       | opcode ← BTC:6                                  |      T       |       1       |
    |                        | if B in [0..15]:               | Teeny ← 1                                       |              |               |
    |                        |    dreg ← dreg & ~(1 << B)     | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← 0                                     |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTC dreg, sreg + i16   | B = sreg + i16                 | opcode ← BTC:6                                  |              |       2       |
    |                        | if B in [0..15]:               | Teeny ← 0                                       |              |               |
    |                        |    dreg ← dreg & ~(1 << B)     | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTC dreg, i16          | B = i16                        | opcode ← BTC:6                                  |              |       3       |
    |                        | if B in [0..15]:               | Teeny ← 0                                       |              |               |
    |                        |    dreg ← dreg & ~(1 << B)     | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTC dreg, sreg + i4    | B = sreg + i4                  | opcode ← BTS:6                                  |      T       |       2       |
    |                        | if B in [0..15]:               | Teeny ← 1                                       |              |               |
    |                        |    dreg ← dreg & ~(1 << B)     | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTC dreg, i4           | B = i4                         | opcode ← BTC:6                                  |      T       |       3       |
    |                        | if B in [0..15]:               | Teeny ← 0                                       |              |               |
    |                        |    dreg ← dreg & ~(1 << B)     | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTF dreg, sreg         | B = sreg                       | opcode ← BTF:7                                  |      T       |       1       |
    |                        | if B in [0..15]:               | Teeny ← 1                                       |              |               |
    |                        |    dreg ← dreg ^ 1 << B        | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← 0                                     |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTF dreg, sreg + i16   | B = sreg + i16                 | opcode ← BTF:7                                  |              |       2       |
    |                        | if B in [0..15]:               | Teeny ← 0                                       |              |               |
    |                        |    dreg ← dreg ^ 1 << B        | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTF dreg, i16          | B = i16                        | oopcode ← BTF:7                                 |              |       3       |
    |                        | if B in [0..15]:               | Teeny ← 0                                       |              |               |
    |                        |    dreg ← dreg ^ 1 << B        | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTF dreg, sreg + i4    | B = sreg + i4                  | opcode ← BTF:7                                  |      T       |       2       |
    |                        | if B in [0..15]:               | Teeny ← 1                                       |              |               |
    |                        |    dreg ← dreg ^ 1 << B        | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | BTF dreg, i4           | B = i4                         | oopcode ← BTF:7                                 |      T       |       3       |
    |                        | if B in [0..15]:               | Teeny ← 0                                       |              |               |
    |                        |    dreg ← dreg ^ 1 << B        | Reg_1 ← dreg                                    |              |               |
    |                        | otherwise, no action           | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | CAL sreg               | Addr = SP & 0x7FFF             | opcode ← CALL:8                                 |      T       |       9       |
    |                        | mem[ Addr ] ← PC               | Teeny ← 1                                       |              |               |
    |                        |    SP ← (SP - 1) & 0x7FFF      | Reg_2 ← sreg                                    |              |               |
    |                        | PC ← sreg                      | Immed_4 ← 0                                     |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | CAL sreg + i16         | Addr = SP & 0x7FFF             | opcode ← CALL:8                                 |              |       10      |
    |                        | mem[ Addr ] ← PC               | Teeny ← 0                                       |              |               |
    |                        |    SP ← (SP - 1) & 0x7FFF      | Reg_2 ← sreg                                    |              |               |
    |                        | PC ← sreg + i16                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | CAL i16                | Addr = SP & 0x7FFF             | opcode ← CALL:8                                 |              |       11      |
    |                        | mem[ Addr ] ← PC               | Teeny ← 0                                       |              |               |
    |                        |    SP ← (SP - 1) & 0x7FFF      | Reg_2 ← rZ                                      |              |               |
    |                        | PC ← i16                       | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | CAL sreg + i4          | Addr = SP & 0x7FFF             | opcode ← CALL:8                                 |      T       |       10      |
    |                        | mem[ Addr ] ← PC               | Teeny ← 1                                       |              |               |
    |                        |    SP ← (SP - 1) & 0x7FFF      | Reg_2 ← sreg                                    |              |               |
    |                        | PC ← sreg + i4                 | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | CAL i4                 | Addr = SP & 0x7FFF             | opcode ← CALL:8                                 |      T       |       11      |
    |                        | mem[ Addr ] ← PC               | Teeny ← 0                                       |              |               |
    |                        |    SP ← (SP - 1) & 0x7FFF      | Reg_2 ← rZ                                      |              |               |
    |                        | PC ← i4                        | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | RET                    | SP ← (SP + 1) & 0x7FFF         | opcode ← POP:4                                  |    T , P     |       12      |
    |                        | PC ← mem[ SP ]                 | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← PC                                      |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | CMP regA, regB         | Affects FLAG register by       | opcode ← CMP:20                                 |      T       |        1      |
    |                        | finding regA - regB            | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← regA                                    |              |               |
    |                        |                                | Reg_2 ← regB                                    |              |               |
    |                        |                                | Immed_4 ← 0                                     |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | CMP regA, regB + i16   | Affects FLAG register by       | opcode ← CMP:20                                 |              |       2       |
    |                        | finding regA - (regB + i16)    | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← regA                                    |              |               |
    |                        |                                | Reg_2 ← regB                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | CMP regA, i16          | Affects FLAG register by       | opcode ← CMP:20                                 |              |       3       |
    |                        | finding regA - i16             | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← regA                                    |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | CMP regA, regB + i4    | Affects FLAG register by       | opcode ← CMP:20                                 |      T       |       2       |
    |                        | finding regA - (regB + i4)     | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← regA                                    |              |               |
    |                        |                                | Reg_2 ← regB                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | CMP regA, i4           | Affects FLAG register by       | opcode ← CMP:20                                 |      T       |       3       |
    |                        | finding regA - i4              | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← regA                                    |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JMP reg                | PC ← reg                       | opcode ← JMP:21                                 |      T       |      13       |
    |                        |                                | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,0,0,0                               |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JMP reg + i16          | PC ← reg + i16                 | opcode ← JMP:21                                 |              |      14       |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,0,0,0                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JMP i16                | PC ← i16                       | opcode ← JMP:21                                 |              |      15       |
    |                        |                                | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← rZ                                      |              |               |
    |                        |                                | C,Z,L,G ← 0,0,0,0                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JE reg                 | if FLAG[Z]:                    | opcode ← JMP:21                                 |      T       |      13       |
    |                        |    PC ← reg                    | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,1,0,0                               |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JE reg + i16           | if FLAG[Z]:                    | opcode ← JMP:21                                 |              |      14       |
    |                        |    PC ← reg + i16              | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,1,0,0                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JE i16                 | if FLAG[Z]:                    | opcode ← JMP:21                                 |              |      15       |
    |                        |    PC ← i16                    | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← rZ                                      |              |               |
    |                        |                                | C,Z,L,G ← 0,1,0,0                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JNE reg                | if FLAG[L] or FLAG[G]:         | opcode ← JMP:21                                 |      T       |      13       |
    |                        |    PC ← reg                    | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,0,1,1                               |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JNE reg + i16          | if FLAG[L] or FLAG[G]:         | opcode ← JMP:21                                 |              |      14       |
    |                        |    PC ← reg + i16              | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,0,1,1                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JNE i16                | if FLAG[L] or FLAG[G]:         | opcode ← JMP:21                                 |              |      15       |
    |                        |    PC ← i16                    | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← rZ                                      |              |               |
    |                        |                                | C,Z,L,G ← 0,0,1,1                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JL reg                 | if FLAG[L]:                    | opcode ← JMP:21                                 |      T       |      13       |
    |                        |    PC ← reg                    | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,0,1,0                               |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JL reg + i16           | if FLAG[L]:                    | opcode ← JMP:21                                 |              |      14       |
    |                        |    PC ← reg + i16              | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,0,1,0                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JL i16                 | if FLAG[L]:                    | opcode ← JMP:21                                 |              |      15       |
    |                        |    PC ← i16                    | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← rZ                                      |              |               |
    |                        |                                | C,Z,L,G ← 0,0,1,0                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JLE reg                | if FLAG[L] or FLAG[Z]:         | opcode ← JMP:21                                 |      T       |      13       |
    |                        |    PC ← reg                    | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,1,1,0                               |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JLE reg + i16          | if FLAG[L] or FLAG[Z]:         | opcode ← JMP:21                                 |              |      14       |
    |                        |    PC ← reg + i16              | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,1,1,0                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JLE i16                | if FLAG[L] or FLAG[Z]:         | opcode ← JMP:21                                 |              |      15       |
    |                        |    PC ← i16                    | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← rZ                                      |              |               |
    |                        |                                | C,Z,L,G ← 0,1,1,0                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JG reg                 | if FLAG[G]:                    | opcode ← JMP:21                                 |      T       |      13       |
    |                        |    PC ← reg                    | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,0,0,1                               |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | JG reg + i16           | if FLAG[G]:                    | opcode ← JMP:21                                 |              |      14       |
    |                        |    PC ← reg + i16              | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,0,0,1                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------------------------------------------------------------------------------------------------------------------------|
    | JG i16                | if FLAG[G]:                     | opcode ← JMP:21                                 |              |      15       |
    |                        |    PC ← i16                    | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← rZ                                      |              |               |
    |                        |                                | C,Z,L,G ← 0,0,0,1                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------------------------------------------------------------------------------------------------------------------------|
    | JGE reg                | if FLAG[G] or FLAG[Z]:         | opcode ← JMP:21                                 |      T       |      13       |
    |                        |    PC ← reg                    | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,1,0,1                               |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------------------------------------------------------------------------------------------------------------------------|
    | JGE reg + i16          | if FLAG[G] or FLAG[Z]:         | opcode ← JMP:21                                 |              |      14       |
    |                        |    PC ← reg + i16              | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← reg                                     |              |               |
    |                        |                                | C,Z,L,G ← 0,1,0,1                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------------------------------------------------------------------------------------------------------------------------|
    | JGE i16                | if FLAG[G] or FLAG[Z]:         | opcode ← JMP:21                                 |              |      15       |
    |                        |    PC ← i16                    | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_1 ← rZ                                      |              |               |
    |                        |                                | C,Z,L,G ← 0,1,0,1                               |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------------------------------------------------------------------------------------------------------------------------|
    | DLY sreg               | len ← sreg                     | opcode ← DLY:22                                 |      T       |       9       |
    |                        | Waste len cycles               | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← 0                                     |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | DLY sreg + i16         | len ← sreg + i16               | opcode ← DLY:22                                 |              |       10      |
    |                        | Waste len cycles               | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | DLY i16                | len ← i16                      | opcode ← DLY:22                                 |              |      11       |
    |                        | Waste len cycles               | Teeny ← 0                                       |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | DLY sreg + i4          | len ← sreg + i4                | opcode ← DLY:22                                 |      T       |       10      |
    |                        | Waste len cycles               | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_2 ← sreg                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------|--------------------------------|-------------------------------------------------|--------------|---------------|
    | DLY i4                 | len ← i4                       | opcode ← DLY:22                                 |      T       |      11       |
    |                        | Waste len cycles               | Teeny ← 1                                       |              |               |
    |                        |                                | Reg_2 ← rZ                                      |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------------------------------------------------------------------------------------------------------------------------|
    | LUP regA, regB         | regA ← regA - 1                | opcode ← LUP:23                                 |      T       |       1       |
    |                        | if regA == 0:                  | Teeny ← 1                                       |              |               |
    |                        |    PC ← regB                   | Reg_1 ← regA                                    |              |               |
    |                        |                                | Reg_2 ← regB                                    |              |               |
    |                        |                                | Immed_4 ← 0                                     |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------------------------------------------------------------------------------------------------------------------------|
    | LUP regA, regB + i16   | regA ← regA - 1                | opcode ← LUP:23                                 |              |       2       |
    |                        | if regA == 0:                  | Teeny ← 0                                       |              |               |
    |                        |    PC ← regB + i16             | Reg_1 ← regA                                    |              |               |
    |                        |                                | Reg_2 ← regB                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------------------------------------------------------------------------------------------------------------------------|
    | LUP regA, i16          | regA ← regA - 1                | opcode ← LUP:23                                 |              |       3       |
    |                        | if regA == 0:                  | Teeny ← 0                                       |              |               |
    |                        |    PC ← i16                    | Reg_1 ← regA                                    |              |               |
    |                        |                                | Reg_2 ← regB                                    |              |               |
    |                        |                                | Immed_16 ← i16                                  |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------------------------------------------------------------------------------------------------------------------------|
    | LUP regA, regB + i4    | regA ← regA - 1                | opcode ← LUP:23                                 |      T       |       2       |
    |                        | if regA == 0:                  | Teeny ← 0                                       |              |               |
    |                        |    PC ← regB + i4              | Reg_1 ← regA                                    |              |               |
    |                        |                                | Reg_2 ← regB                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
    |------------------------------------------------------------------------------------------------------------------------------------------|
    | LUP regA, i4           | regA ← regA - 1                | opcode ← LUP:23                                 |     T        |       3       |
    |                        | if regA == 0:                  | Teeny ← 0                                       |              |               |
    |                        |    PC ← i4                     | Reg_1 ← regA                                    |              |               |
    |                        |                                | Reg_2 ← regB                                    |              |               |
    |                        |                                | Immed_4 ← i4                                    |              |               |
    |                        |                                |                                                 |              |               |
    |                        |                                |                                                 |              |               |
     ------------------------------------------------------------------------------------------------------------------------------------------ 
```
