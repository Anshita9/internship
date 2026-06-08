; Define register aliases for clarity
.def temp   = r16
.def A      = r17
.def B      = r18
.def Cin    = r19
.def sum    = r20
.def cout   = r21
.def t1     = r22
.def t2     = r23

; Target Device Configuration for AVRA
.device ATmega328P

; Standard I/O Address Definitions for ATmega328P
.equ DDRD   = 0x0A
.equ PORTD  = 0x0B
.equ PIND   = 0x09
.equ DDRB   = 0x04
.equ PORTB  = 0x05

; --- SETUP SECTION ---
; Configure Inputs: Clear PD2, PD3, PD4 in DDRD (0 = Input)
cbi DDRD, 2
cbi DDRD, 3
cbi DDRD, 4

; Enable Pull-ups: Set PD2, PD3, PD4 in PORTD (1 = Pull-up enabled)
sbi PORTD, 2
sbi PORTD, 3
sbi PORTD, 4

; Configure Outputs: Set PB0 and PB1 in DDRB (1 = Output)
sbi DDRB, 0
sbi DDRB, 1

; --- MAIN LOOP SECTION ---
loop:
    ; Read and invert inputs (Active-LOW buttons to Active-HIGH logic)
    in   temp, PIND
    com  temp
    
    ; Isolate Input A (PD2)
    mov  A, temp
    lsr  A
    lsr  A
    andi A, 0x01
    
    ; Isolate Input B (PD3)
	    mov  B, temp
    lsr  B
    lsr  B
    lsr  B
    andi B, 0x01
    
    ; Isolate Input Cin (PD4)
    mov  Cin, temp
    lsr  Cin
    lsr  Cin
    lsr  Cin
    lsr  Cin
    andi Cin, 0x01

    ; Calculate SUM (A XOR B XOR Cin)
    mov  sum, A
    eor  sum, B
    eor  sum, Cin

    ; Calculate CARRY-OUT ((A&B) | (B&Cin) | (A&Cin))
    mov  cout, A
    and  cout, B       ; t1 = A & B
    
    mov  t1, B
    and  t1, Cin       ; t2 = B & Cin
    
    mov  t2, A
    and  t2, Cin       ; t3 = A & Cin
    
    or   cout, t1      ; cout = (A&B) | (B&Cin)
    or   cout, t2      ; cout = (A&B) | (B&Cin) | (A&Cin)

    ; Pack outputs for PORTB (Sum at bit 0, Cout at bit 1)
    lsl  cout          ; Shift Carry bit to bit position 1
    or   sum, cout     ; Combine them together
    
    ; Write outputs to physical pins
    out  PORTB, sum
    
    rjmp loop          ; Repeat indefinitely

	

