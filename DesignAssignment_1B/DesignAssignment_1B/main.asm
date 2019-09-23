; DesignAssignment_1B.asm
;
; Created: 9/20/2019 7:06:30 PM
; Author : BRYSEN KOKUBUN
;		

.ORG 0X0000
	
.EQU STARTADDS = 0X0200     ;MEMORY LOCATION IN SRAM FOR POPULATION #'S

LDI XL,LOW(STARTADDS)		;LOADING X LOW POINTER TO POINT TO STARTADDS
LDI XH,HIGH(STARTADDS)		;LOADING X HIGH POINTER TO POINT TO STARTADDS
LDI YL,LOW(0X0300)			;LOADING Y LOW POINTER TO POINT TO  0X0300
LDI YH,HIGH(0X0300)			;LOADING Y HIGH POINTER TO POINT TO 0X0300
LDI ZL,LOW(0X0500)			;LOADING Z LOW POINTER TO POINT TO  0X0500
LDI ZH,HIGH(0X0500)			;LOADING Z HIGH POINTER TO POINT TO 0X0500

LDI R22, 0					;INITIALIZING R22 = 0 FOR LOADING FROM MEMORY
LDI R21, 250				;REGISTER FOR LOOP COUNTER SIZE == 250
LDI R20, 0					;LOADING REGISTER R20 WITH VALUE OF 0

CLR R25						;CLEARING R25 FOR ADD CARRY
CLR R19						;CLEARING SUM REG = 0 (!DIVISIBLE BY 5)
CLR R18						;CLEARING SUM REG = 0 (!DIVISIBLE BY 5)
CLR R17						;CLEARING SUM REG = 0 (DIVISIBLE BY 5)
CLR R16						;CLEARING SUM REG = 0 (DIVISIBLE BY 5)
	
STORE: 
	INC R20					;INCREMENTS VALUE IN REGISTER R20
	ST X+, R20				;STORES R20 VALUE INTO MEMORY LOCATION OF X	
							;THEN INCREMENTS THE MEMORY LOCATION FOR X
	MOV R22, R20			;MOVE VALUE IN REGISTER R20 INTO REGISTER R22
	JMP CHECK_DIV			;CONTROL JUMPS TO CHECK_DIV LABEL

LOOP_COUNTER:
	DEC R21					;DECREMENT LOOP COUNTER 
	BRNE STORE				;IF R21 != 0 BRANCH TO STORE LABEL
	JMP DONE				;CONTROL JUMPS TO DONE LABEL
	
CHECK_DIV:
	CPI R22, 5				;WILL COMPARE VALUES IN R22 AND 5
	BRLO NDIV_5				;IF R22 < 5 THEN BRANCH TO NDIV_5 LABEL
	SUBI R22, 5				;SUBTRACTING VALUE IN R22 WITH 5

	CPI R22, 0				;COMPARING VALUE IN R22 == 0
	BREQ DIV_5				;IF VALUE IN R22 == 0
							;BRANCH DIV_5, ELSE CONTINUE	
								
	JMP CHECK_DIV 			;CONTROL JUMPS TO CHECK_DIV LABEL 		
		
DIV_5:

	ST Y+, R20				;STORES VALUE IN REGISTER R20 INTO MEMORY 
							;Y AND INCREMENTS LOCATION

	ADD R16, R20			;ADDS VALUE IN REGISTER R20 INTO REGISTER R16
	ADC R17, R25			;ADDS VALUE INTO REGISTER R17	
							;INCASE CARRY IS OBTAINED

	JMP LOOP_COUNTER		;CONTROL JUMPS TO LOOP_COUNTER LABEL
		
NDIV_5: 
	ST Z+, R20              ;STORES VALUE IN REGISTER R20 INTO MEMORY
							;Z AND INCREMENTS LOCATION

	ADD R18, R20			;ADDS VALUE IN REGISTER R20 INTO REGISTER R18
	ADC R19, R25			;ADDS VALUE INTO REGISTER R19
							;INCASE CARRY IS OBTAINED

	JMP LOOP_COUNTER		;CONTROL JUMPS TO LOOP_COUNTER LABEL
	
DONE: JMP DONE				;END PROGRAM
