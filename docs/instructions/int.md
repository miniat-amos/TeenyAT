### `INT Instruction:`
###### [↩ Back to Main Table](../README.md)
### Generate an interrupt, 0..15
This instruction flags the identified interrupt as having occurred.
It does NOT guarantee the associated interrupt handler will execute.
The handler executing depends on whether interrupts are enabled in
the Constrol Status Register(CSR) and whether the identified
interrupt is individually enabled in the Interrupt Enable Register
(IER).

Interrupts identified greater than 15 will be truncated to be between
0..15
|  Opcode  | Flags Set                 | Example Usage           |
|--------- |---------------------------|-------------------------|
|   24      | N/A                       |    `int 5`<br> `int rB`<br> `int rD + 8` |
---