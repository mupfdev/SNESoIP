
.DEFINE Handshake_Counter $00
.DEFINE Handshake1 $01
.DEFINE Handshake2 $03
.DEFINE Handshake3 $05

.DEFINE Joypad1Data $0200
.DEFINE Joypad2Data $0202

.INCLUDE "header.inc"
.INCLUDE "InitSNES.asm"

.BANK 0 SLOT 0
.ORG $1000
.SECTION "MainCode"

VBLANK:
	PHA
	PHX
	PHY
	PHB
	
	AutoJoypadWarten:
	LDA $4212.w
	ROR
	BCS AutoJoypadWarten
	
	LDX $4218.w
	STX Joypad1Data.w
	LDX $421A.w
	STX Joypad2Data.w
	
	PLB
	PLY
	PLX
	PLA
	RTI

Start:
	InitSNES
    
	REP #$10.b
	SEP #$20.b
    
	LDA #$80.b
	STA $2100.w	; FBLANK
    
	LDA #$01.b
	STA $2105.w	; BG Mode 1

	STZ $2107.w	; BG1SC (Tilemap Address)
	STZ $210B.w	; BG12NBA (BG1 and 2 Chr Address)
	
	STZ $2121.w	; CGADD
	LDA #$7F.b
	STA $2122.w
	STA $2122.w	; ROSA

	LDA #$01.b
	STA $212C.w	; Enable BG1
	
	LDA #$81.b
	STA $4200.w	; NMI / Auto Joypad

	LDA #$0F.b
	STA $2100.w	; Screen on

; =================== Warte auf erstes Signal vom Server
InitLoop1:
	LDX Joypad2Data.w
	CPX #$0800.w	; UP
	BNE InitLoop1
	
	STZ $2121.w	; CGADD
	LDA #$3F.b
	STA $2122.w
	STA $2122.w	; BLASSORANGE

; =================== Warte auf zweites Signal vom Server
InitLoop2:
	LDX Joypad2Data.w
	CPX #$0200.w	; LEFT
	BNE InitLoop2
	
	STZ $2121.w	; CGADD
	LDA #$1F.b
	STA $2122.w
	STA $2122.w	; ORANGE

; =================== Warte auf drittes Signal vom Server
InitLoop3:
	LDX Joypad2Data.w
	CPX #$0400.w	; DOWN
	BNE InitLoop3
	
	STZ $2121.w	; CGADD
	LDA #$0F.b
	STA $2122.w
	STA $2122.w	; GR‹N

; =================== Lade Signale vom Server

	STZ Handshake_Counter
	
	Handshake_MainLoop:
	LDA Handshake_Counter
	CMP #%00000111.b		; Alle drei Flags gesetzt?
	BEQ Tastenabfrage1
	
	LDA Joypad2Data.w+1		; High Byte
	AND #$0F.b
	CMP #$0F.b			; Sind alle vier Richtungen gleichzeitig gedr¸ckt?
	BEQ ValidHandshake
	BRA Handshake_MainLoop
	
	ValidHandshake:
	LDA Joypad2Data			; Low Byte
	AND #$0F.b
	CMP #%00001001.b		; Signal 1?
	BEQ Handshake_1
	CMP #%00001011.b		; Signal 2?
	BEQ Handshake_2
	CMP #%00001101.b		; Signal 3?
	BEQ Handshake_3
	BRA Handshake_MainLoop

	Handshake_1:
	LDA Handshake_Counter		; Datensatz schon vorhanden?
	AND #%00000001.b
	BNE Handshake_MainLoop
	
	LDA Joypad2Data				; Wenn nicht, entferne aus der Tastenkombo die Signal-Zeichenkette und speicher den Wert
	AND #$F0.b
	STA Handshake1
	LDA Joypad2Data+1
	AND #$F0.b
	STA Handshake1+1
	
	LDA Handshake_Counter		; Auﬂerdem setz die Flag, dass das entsprechende Handshake-Singal da ist?
	ORA #%00000001
	STA Handshake_Counter
	
	BRA Handshake_MainLoop

	Handshake_2:
	LDA Handshake_Counter		; Datensatz schon vorhanden?
	AND #%00000010.b
	BNE Handshake_MainLoop
	
	LDA Joypad2Data				; Wenn nicht, entferne aus der Tastenkombo die Signal-Zeichenkette und speicher den Wert
	AND #$F0.b
	STA Handshake2
	LDA Joypad2Data+1
	AND #$F0.b
	STA Handshake2+1
	
	LDA Handshake_Counter		; Auﬂerdem setz die Flag, dass das entsprechende Handshake-Singal da ist?
	ORA #%00000010
	STA Handshake_Counter
	
	BRA Handshake_MainLoop

	Handshake_3:
	LDA Handshake_Counter		; Datensatz schon vorhanden?
	AND #%00000100.b
	BNE Handshake_MainLoop
	
	LDA Joypad2Data				; Wenn nicht, entferne aus der Tastenkombo die Signal-Zeichenkette und speicher den Wert
	AND #$F0.b
	STA Handshake3
	LDA Joypad2Data+1
	AND #$F0.b
	STA Handshake3+1
	
	LDA Handshake_Counter		; Auﬂerdem setz die Flag, dass das entsprechende Handshake-Singal da ist?
	ORA #%00000100
	STA Handshake_Counter
	
	BRA Handshake_MainLoop

; =================== Warte auf Taste 1
	LDA #$04.b
	STA $2122.w
	STA $2122.w	; 
Tastenabfrage1:
	LDX Joypad1Data
	CPX Handshake1
	BNE Tastenabfrage1
	LDA #$40.b
	STA $2122.w
	STA $2122.w	; 
Tastenabfrage2:
	LDX Joypad1Data
	CPX Handshake2
	BNE Tastenabfrage2
	LDA #$0F.b
	STA $2122.w
	STA $2122.w	; 
Tastenabfrage3:
	LDX Joypad1Data
	CPX Handshake3
	BNE Tastenabfrage3
	LDA #$01.b
	STA $2122.w
	STA $2122.w	; Fast schwarz
forever:
	jmp forever

.ENDS
