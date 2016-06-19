; Stack area
				AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   0x00000400
__init_sp

; Init vectors
                AREA    RESET, DATA, READONLY
__vectors       DCD     __init_sp                    ; Top of Stack
                DCD     RstHandler                   ; Reset Handler



                AREA    |.text|, CODE, READONLY

; Reset handler
RstHandler	PROC
				ENTRY
                LDR     R0, =__main
                BX      R0
            ENDP

			ALIGN
				
; Main procedure
__main		PROC
				LDR 	R1, =0x20000500
				MOV		R0, #0x5
				MOV		R4, #0x0
				MOV		R5, #0x1
				MOV		R6, #0x0
FIBLOP			CMP 	R6, #0x0
				BNE		FIBNZ
				MOV		R7, #0x0
				B		NXTLOP
FIBNZ			CMP 	R6, #0x1
				BNE		FIBCON
				MOV		R7, #0x1
				B		NXTLOP
FIBCON			ADD		R7, R4, R5
				MOV		R4, R5
				MOV		R5, R7
NXTLOP			STMIA.W	R1!, {R7}
				ADD		R6, R6, #0x1
				CMP		R6, R0
				BLE		FIBLOP
				B		.
			ENDP
		END