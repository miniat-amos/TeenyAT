### `LUP Instruction:`
###### [↩ Back to Main Table](../README.md)
### Loop to a label register amount of times
#### This is a loop down counter meaning the value of the register decrements by 1 until its equal to zero
| Opcode | Flags Set    | Example Usage |
|--------|-------------|---------------|
| 22     | Eq/Less/Grt | `lup rA, !label` <br> `lup rA, rB` <br> `lup rA, rB + !label` |
---