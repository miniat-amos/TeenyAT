### `LUP Instruction:`
###### [↩ Back to Main Table](../README.md)
### Loop based on a counting register
### This decrements the identified loop register and branches to the target address if the loop register is not zero
| Opcode | Flags Set    | Example Usage |
|--------|-------------|---------------|
| 22     | Eq/Less/Grt | `lup rA, !label` <br> `lup rA, rB` <br> `lup rA, rB + !label` |
---
