;------------------------------------------------------------------------------
; Fast and short 32 bits AVR sqrt routine, avr-gcc ABI compliant
; R25:R24 = SQRT (R25:R24:R23:R22) 
; rounded down to integer
; 
; Destroys R26,R27,R22,R23,R18,R19
; Cycles incl call & ret = 260-300
; Stack incl call = 2-3
;------------------------------------------------------------------------------

.text
.global sqrt32_floor
.type sqrt32_floor, \function

sqrt32_floor:
	ldi   R19, 0xc0
	clr   R18               ; rotation mask in R19:R18
	ldi   R27, 0x40
	sub   R26, R26          ; developing sqrt in R27:R26, C=0
1:	brcs  2f                ; C --> Bit is always 1
	cp    R24, R26
	cpc   R25, R27          ; Does test value fit?
	brcs  3f                ; C --> nope, bit is 0
2:	sub   R24, R26
	sbc   R25, R27          ; Adjust argument for next bit
	or    R26, R18
	or    R27, R19          ; Set bit to 1
3:	lsr   R19
	ror   R18               ; Shift right mask, C --> end loop
	eor   R27, R19
	eor   R26, R18          ; Shift right only test bit in result
	rol   R22               ; Bit 0 only set if end of loop
	rol   R23
	rol   R24
	rol   R25               ; Shift left remaining argument (C used at 1:)
	sbrs  R22, 0            ; Skip if 15 bits developed
	rjmp  1b                ; Develop 15 bits of the sqrt

	brcs  4f                ; C--> Last bits always 1
	lsl   R23               ; Need bit 7 in C for cpc
	cpc   R26, R24
	cpc   R27, R25          ; After this C is last bit

4:	adc   R26, R19          ; Round up if C (R19=0)
	mov   R25, R27          ; return in R25:R24 as for avr-gcc ABI
	mov   R24, R26
	ret
 
.size sqrt32_floor, .-sqrt32_floor
