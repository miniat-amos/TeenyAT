;crappy test program reverses array including array terminating value which is arbitrarily set to #FFFF
13 JMP print
34 Screen 32768

32 Array
   33 2 45 12 3 67 34 10 65535

32 print
    0 SET 7 z
32 loop
    1 LOD 5 7 Array
    5 STR 7 Screen 5
    4 INC 7
    2 CMP 5 65535
    13 JE main
    13 JMP loop

32 main
    0 SET 4 z ;set register 3 to the value of the zero register always zero
    0 SET 7 z ;set register 7 to the value of the zero register which is always zero
32 loopforlast
    1 LOD 5 7 Array ;load first value of the Array
    2 CMP 5 65535
    13 JE lastfound ;address of last element is in $7
    4 INC 7
    13 JMP loopforlast
32 lastfound
    1 LOD 5 4 Array
    1 LOD 6 7 Array
    5 STR 4 Array 6
    5 STR 7 Array 5
    4 INC 4
    4 DEC 7
    0 CMP 4 7
    13 JGE endofprogram
    13 JMP lastfound
32 endofprogram

32 printtwo
    0 SET 7 Z
32 looptwo
    1 LOD 5 7 Array
    5 STR 7 Screen 5
    4 INC 7
    2 CMP 5 65535
    13 JE print
    13 JMP looptwo


;first pass
;first thing on each line can be a instruction or a label or a variable
;instructions start with a ascii letter and have no colon
;labels start with ascii and do have a colon
;variables start with a .
;data section starts with @

;if a label add to map with mapping the string to the current address remove the label line
;if @data ignore it for now
;if instruction figure out if it is teeny and increase the current address appropriately
;if variable add to map mapping the string to the value remove the variable assignment line

;second pass
;replace all instances of variables and labels in asm code with the value they represent

;third pass
;go through each instruction make the encoding and place it in memory in the correct location
;once we hit @data just place the whitespace seperated values in memory as half words or 16 bit values
