;crappy test program reverses array including array terminating value which is arbitrarily set to #FFFF
14 JMP print
34 Screen 32768

32 Array
   33 2 45 12 3 67 34 10 65535

32 print
    1 SET 7 z
32 loop
    2 LOD 5 7 Array
    6 STR 7 Screen 5
    5 INC 7
    3 CMP 5 65535
    14 JE main
    14 JMP loop

32 main
    1 SET 4 z ;set register 3 to the value of the zero register always zero
    1 SET 7 z ;set register 7 to the value of the zero register which is always zero
32 loopforlast
    2 LOD 5 7 Array ;load first value of the Array
    3 CMP 5 65535
    14 JE lastfound ;address of last element is in $7
    5 INC 7
    14 JMP loopforlast
32 lastfound
    2 LOD 5 4 Array
    2 LOD 6 7 Array
    6 STR 4 Array 6
    6 STR 7 Array 5
    5 INC 4
    5 DEC 7
    1 CMP 4 7
    14 JGE endofprogram
    14 JMP lastfound
32 endofprogram

32 printtwo
    1 SET 7 Z
32 looptwo
    2 LOD 5 7 Array
    6 STR 7 Screen 5
    5 INC 7
    3 CMP 5 65535
    14 JE print
    14 JMP looptwo


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
