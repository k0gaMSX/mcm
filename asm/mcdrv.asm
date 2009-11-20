;;; THIS VERSION DISABLE SEARCH OF TURBOR AND FM
;;;Commands of driver:


;;; MCDRV  -> Execute a step of playing
;;; MCDRVC -> Accepts commands for the driver.

	
;;; a=1 Init variables of driver
;;; a=2 Set sfx table -> hl address of new table
;;; a=3 Init chips and variables
;;; a=4 Fadeout -> c Number of interrupts between each step.
;;; a=5 Fadein -> c0-c5 Number of interrupts between each step.
;;;               c7    Enable silence decodification
;;; a=6 Test Music -> FZ=0 if it play a song
;;; a=7 Play sfx -> c Number of sfx
;;; a=8 Test sfx -> FZ=0 if it play a sfx.
;;; a=9 Play song -> hl Address of song
;;; a=10 Play internal song -> Number of internal song
;;; a=11 Quit music.
;;; a=12 Set internal music table -> hl address of new table

	
	
;;; Thanks to MicroCabbin for making the best music driver that I have never 
;;; seen. Thanks to Manuel Pazos and Patrieck Lesparre for helping me to 
;;; dissasemble this code to.

	

	
	
	
	
		
;;; ************************************************************************
;;; Name:	MCDRV
;;; Function:	Do a step of playing.
;;; Input:	Nothing
;;; Output:	Nothing
;;; Modify:	All
;;; ************************************************************************
	
		
MCDRV:				
		jp	IntFunct

			
		
;;;************************************************************************* 
;;; Name:	MCDRVC
;;; Function:	Trap for calling all functions of the driver
;;; Input:	A -> Number of function
;;;                1 Init variables of driver
;;;		   2 Set sfx table -> hl address of new table
;;;		   3 Init chips and variables
;;;		   4 Fadeout -> c Number of interrupts between each step.
;;;		   5 Fadein -> c Number of interrupts between each step.
;;;		   6 Test Music -> FZ=0 if it play a song
;;;		   7 Play sfx -> c Number of sfx
;;;		   8 Test sfx -> FZ=0 if it play a sfx.
;;;		   9 Play song -> hl Address of song
;;;		  10 Play internal song -> Number of internal song
;;;		  11 Quit music.
;;;		  12 Set internal music table -> hl address of new table
;;;		C -> Parameter of called function
;;;	        HL -> Parameter of called function
;;; Output:	Nothing
;;; Modify:	A
;;; *************************************************************************
	
	
		
MCDRVC:	
Commands:				
	di			; Input	point for driver commands
	push	ix		; Push all registers 
	push	hl
	push	de
	push	bc
	cp	13		; Accept only 13 functions 
	jr	nc, EndCommand
	
	add	a, a		; Calculate offset into Table_CMD
	ld 	d,0	
	ld 	e,a
	push 	de
	pop 	ix
	ld	de, Table_CMD	; add offset to base address
	add	ix, de		 
	ld	e, (ix+0)	; and take the routine address
	ld	d, (ix+1)
	push 	de
	pop 	ix
	call	JumpCommand	; and jump to it

EndCommand:				
	pop	bc		; pop all registers
	pop	de
	pop	hl
	pop	ix		
	ret


JumpCommand:			
	jp	(ix)




	
EmptySong:			; Song used for resting the player
	db 0Ch, 0, 9, 0, 0A1h, 62h, 25h, 72h, 0     
	db 10h, 0, 0A1h, 62h, 25h, 72h, 0, 17h    
	db 0, 0A1h, 62h, 25h, 72h, 0, 1Eh, 0, 0A1h  
	db 62h, 25h, 72h, 0, 25h, 0, 0A1h, 62h	
	db 25h, 72h, 0, 2Ch, 0, 0A1h, 62h, 25h     	
	db 72h, 0, 33h, 0, 0A1h, 62h, 25h, 72h	
	db 0, 3Ah, 0, 0A1h, 62h, 25h, 72h, 0, 41h   		
	db 0, 0A1h, 62h, 25h, 72h, 0, 48h, 0, 0A1h
	db 62h, 25h, 72h, 0, 4Fh, 0, 0A1h, 62h   
	db 25h, 72h, 0, 56h, 0, 0A1h, 62h, 25h
	db 72h, 0, 0, 0
	

	
	
	
Table_CMD:			; Table used for store the address of
				; routines used for the player functions 
	dw CMD_initvar		
	dw CMD_initvar		
	dw CMD_putSFX
	dw CMD_initchips
	dw CMD_fadeout
	dw CMD_fadein
	dw CMD_chkmus
	dw CMD_sfx
	dw CMD_chksfx
	dw CMD_musicon
	dw CMD_incmus
	dw CMD_musicoff
	dw CMD_putMinc





	
	
;;; **********************************************************************
;;; Name:	CMD_initvar
;;; Function:	Initialize all drivers variable
;;; Input:	Nothing
;;; Output:	Nothing
;;; Modify:	af,hl,de,bc
;;; **********************************************************************
		

CMD_initvar:				
	jp	initvars





	
;;; **********************************************************************
;;; Name:	CMD_putSFX
;;; Function:	Store a new sfx table.
;;; Input:	HL -> Address of new sfx table
;;; Output:	Nothing
;;; Modify:	Nothing
;;; **********************************************************************
	
	
CMD_putSFX:	
	ld (Table_SFX),hl
	ret	





	
;;; **********************************************************************
;;; Name:	CMD_putMinc
;;; Function:	Store a new internal music table.
;;; Input:	HL -> Address of new internal music table.
;;; Output:	Nothing
;;; Modify:	Nothing
;;; **********************************************************************	
	

CMD_putMinc:
	ld (TableMinc),hl
	ret
	
			


;;; **********************************************************************
;;; Name:	CMD_musicoff
;;; Function:	Rest the player
;;; Input:	Nothing
;;; Output:	Nothing
;;; Modify:	af,hl,de,bc,ix
;;; **********************************************************************

	
CMD_musicoff:				
	ld	hl,EmptySong	; Put a empy song for resting the driver
	

	
;;; **********************************************************************
;;; Name:	CMD_musicon
;;; Function:	Put a song for playing
;;; Input:	hl -> Address of song
;;; Output:	Nothing
;;; Modify:	af,hl,de,bc,ix
;;; **********************************************************************

CMD_musicon:				
	in	a, (0E6h)	; Init time of last fm access
	ld	(Tlast_FM), a

	push	hl		; Init all variables for playing a song
	call	initvars
	pop	hl		; and init all data tables from
	call	InitTblSng	; the song
	ret


	
;;; **********************************************************************
;;; Name:	CMD_initchips
;;; Function:	Init variables and chips
;;; Input:	Nothing
;;; Output:	Nothing
;;; Modify:	af,hl,de,bc
;;; **********************************************************************

CMD_initchips:		
	call	initvars	; Init driver variables
	call	initchips	; Init sound chips
	ret	


	

;;; **********************************************************************
;;; Name:	CMD_fadeout
;;; Function:	Enable fadeout
;;; Input:	c0-c5 -> Number of playing steps between fade steps.
;;;		c7    -> Enable decoding in silence	
;;; Output:	Nothing
;;; Modify:	af
;;; **********************************************************************	

	
CMD_fadeout:				
	ld	a,80h		; Set upper bit of FadeCtl
	or	c		; and get enable fade out
	ld	(FadeCtl),a
	
	ld	a,c		; and now test if is enabled
	and	80h		; silence decodification option
	ld	(FadeOutRest),a 
	ret



;;; **********************************************************************
;;; Name:	CMD_fadein
;;; Function:	Enable fadein
;;; Input:	c0-c5 -> Number of playing steps between fade steps.	
;;; Output:	Nothing
;;; Modify:	af
;;; **********************************************************************	

	

CMD_fadein:				
	ld	a, (SilenceFlag) ; If we are not in a silence then return
	or	a
	ret	z
	
	ld	a, 0C0h		 ; Enable fadein setting b6 of FadeCtl 
	or	c
	ld	(FadeCtl), a
	
	ld	a,15		; Set initial number of pass to 15
	ld	(FadeNumPass), a
	xor	a		; and disable silence mode
	ld	(SilenceFlag), a
	ret
	


	
;;; **********************************************************************
;;; Name:	CMD_chkmus
;;; Function:	Test if there is music playing
;;; Input:	Nothing	
;;; Output:	Z -> 1 Music is playing
;;;		     0 Music is not playing
;;; Modify:	af
;;; **********************************************************************	
	

CMD_chkmus:				
	ld	a, (ChannelOFF)	; If ChannelOff
	ld	b, a
	ld	a, (FadeCtl)	; and FadeCtl are both 0
	or	b		; then there isn't music playing
	ret

	


	

;;; ****** VOY POR AQUI COMENTANDO CODIGO!!!!!!!!!!



	
CMD_sfx:				; ...
		ld	hl, (Table_SFX)	; Definicion de	los SFX
		ld	a,h
		or	l
		ret	z	
		call	SearchSong
		ret	nc
		call	PutSFX
		xor	a
		ret


	
;****************************************************************************

CMD_incmus:				; ...
		ld	hl, (TableMinc)
		ld	a,h
		or	l
		ret	z
		call	SearchSong
		ret	nc
		call	PutMusInc
		xor	a
		ret	
;****************************************************************************
;Search	for a song in a	song list
;hl -> Address of song list

SearchSong:				; ...
		ld	d, h		; Input: HL -> SFX Table base
		ld	e, l
		ld	a, c
		dec	a
		cp	(hl)
		ld	a, 0FFh
		ret	nc
		ld	b, 0
		add	hl, bc
		add	hl, bc
		ld	c, (hl)
		inc	hl
		ld	b, (hl)
		ex	de, hl
		add	hl, bc
		scf	
		ret	
;****************************************************************************

CMD_chksfx:				; ...
		ld	a, (SFX_ON)
		or	a
		ret	
;****************************************************************************

WritePSG:				; ...
		push	af
;b -> PSG channel
		ld	a, b
		out	(0A0h),	a
		pop	af
		out	(0A1h),	a
		ret	
;****************************************************************************

ReadPSG:				; ...
		ld	a, b
		out	(0A0h),	a
		in	a, (0A2h)
		ret	
;****************************************************************************

		 
WriteFM:	push bc
		ld c,a
;;; 	ld a,(Tmsx)
;;;     or a
;;; 	jr z,WriteFMt
		ld a,b
		out (7ch),a
		ld a,c
		out (7dh),a
		pop bc		
		ret		

	
WriteFMt:	ld	a,c
		push	af
		ld	a, (Tlast_FM)
		ld	c, a

WriteFM1:				; ...
		in	a, (0E6h)
		sub	c
		cp	6
		jr	c, WriteFM1
		ld	a, b
		out	(7Ch), a
		in	a, (0E6h)
		ld	c, a

WriteFM2:				; ...
		in	a, (0E6h)
		sub	c
		cp	1
		jr	c, WriteFM2
		in	a, (0E6h)
		ld	(Tlast_FM), a
		pop	af
		out	(7Dh), a
		pop	bc
		ret
	



					




	
;;; *************************************************************************
;;; Name:	initvars
;;; Function:	Init all songs relative variables
;;; Input:	Nothing
;;; Output:	Nothing
;;; Modify:	hl,de,bc,af
;;; *************************************************************************

	
initvars:				; ...
		ld	hl, ChannelOFF
		ld	de, TimeLastChn
		ld	bc, 298h
		ld	(hl), 0
		ldir	
		ld	a, 4Bh ; 'K'
		ld	(NumDecInt), a
		ld	a, 20h ; ' '
		ld	(ShdFM_E), a
		ld	a, 9
		ld	(Rhythm_Mode), a
		ld	a, 1
		ld	(ChannelOFF), a


;;; ************************************************************************
;;; Name:	initPSG
;;; Function:	Disable tone and noise for all channels
;;; Input:	Nothing
;;; Output:	Nothing
;;; Modify:	A,B
;;; ************************************************************************
	
initPSG:				; ...
		ld	b,7
		call	ReadPSG
		or	3Fh
		ld	(ShdPSG_7),a
		call	WritePSG
		ret



	
;****************************************************************************

InitTblSng:				; ...
		ld	ix, ChnTblSong
		ld	a, (hl)
		cp	0FEh		; Bload	header?
		jr	nz, InitTblSng1
		ld	de, 7
		add	hl, de

InitTblSng1:				; ...
		ld	e, l
		ld	d, h
		inc	hl
		inc	hl

InitTblSng2:				; ...
		push	hl
		ld	(ix+0),	1
		inc	hl
		inc	hl
		ld	(ix+1),	l
		ld	(ix+2),	h
		pop	hl
		ld	bc, 2Eh	
		add	ix, bc
		ld	a, (hl)
		inc	hl
		ld	h, (hl)
		ld	l, a
		add	hl, de
		push	hl
		ld	a, (hl)
		inc	hl
		or	(hl)
		pop	hl
		jr	nz, InitTblSng2
		ret	
;****************************************************************************

PutSFX:					; ...
		push	hl
		ld	hl, ChnSFXPSG1
		ld	de, ChnSFXPSG1+1
		ld	bc, 45
		ld	(hl), 0
		ldir	
		pop	hl
		ld	ix, ChnSFXPSG1
		ld	a, (hl)

PutSFX1:				; ...
		ld	(ix+0),	1
		inc	hl
		ld	(ix+1),	l
		ld	(ix+2),	h
		ld	de, 46
		add	ix, de
		call	ADD_HL
		ld	a, (hl)
		or	a
		jr	nz, PutSFX1
		ld	a, (ShdPSG_7)
		or	36h ; '6'
		ld	(ShdPSG_7), a
		ld	a, 3
		ld	(SFX_ON), a
		ret	
;****************************************************************************

PutMusInc:				; ...
		push	hl
		ld	a, (SONG_ON)
		or	a
		jr	nz, PutMusInc1
		ld	hl, ChannelAct
		ld	de, ShadowVars
		ld	bc, 28Fh
		ldir	

PutMusInc1:				; ...
		call	initvars
		ld	a, 1
		ld	(SONG_ON), a
		pop	hl
		call	InitTblSng
		ret	
;****************************************************************************

QuitSong:				; ...
		ld	hl, ShadowVars
		ld	de, ChannelAct
		ld	bc, 28Fh
		ldir	
		xor	a
		ld	(SONG_ON), a
		ld	a, (FM_InstSoft)
		call	WriteFMInst
		xor	a
		ld	b, 8
		call	WritePSG
		inc	b
		call	WritePSG
		inc	b
		call	WritePSG
		ret	
;****************************************************************************

initchips:		

;;; 	call    MCSearchFM
;;; 	call    MCTestTR
		
		ld	c, 9
		ld	b, 30h

initchip1:				; ...
		ld	a, 0Fh
		call	WriteFM
		inc	b
		dec	c
		jr	nz, initchip1
		jp	initPSG

;***************************************************************************

	
MCTestTR:	ld a,(0fcc1h)
		ld hl,2dh
		call 0ch
		cp 3
		ld a,1
		jr nz,MCTestTR1
		xor a
		

MCTestTR1:	ld (Tmsx),a
		ret        				
	

Tmsx:		db 0	
	

;;; ****************************************************************
; Search and init FM



	
MCSearchFM:	in a,(0a8h)
		push af
		ld a,(0ffffh)
		cpl
		push af
			
		ld hl,0
		add hl,sp			
		dec hl
		ld de,(MCEND-MCfindfm1)+20	
		or a
		sbc hl,de
		push hl
		push hl
		push hl
	
	
		ld de,(MCFMText-MCfindfm1)+1
		add hl,de
		ex de,hl
		ld hl,MCFMP1+1
		ld (hl),e
		inc hl
		ld (hl),d

		pop hl
		ld de,(MCFM_Japones-MCfindfm1)+1
		add hl,de
		ex de,hl
		ld hl,MCFMP1+1
		ld (hl),e
		inc hl
		ld (hl),d
		
				
		pop hl		
		ld de,MCfindfm1
		ex de,hl
		ld bc,MCEND-MCfindfm1
		ldir
		pop hl
		jp (hl)
			
	
MCfindfm1	ld	hl,0FCCAh
		xor	a
		ld	b,4
MCFMLP2:	push	bc
		ld	b,4
MCFMLP1:	push	bc
		push	af
		push	hl
		set	7,a		
		ld	h,040h
		call	024h
		pop	hl
		push	hl
		ld	a,(hl)
		cp	020h
		jr	z,MCFMTest
MCFMt		jr	z,MCFMFnd
		pop	hl
		pop	af
		add	a,4
		and	0Fh
		inc	hl
		inc	hl
		inc	hl
		inc	hl
		pop	bc
		djnz	MCFMLP1
		add	a,1
		and	03h
		pop	bc
		djnz	MCFMLP2
			

MCRecover:	pop af
		ld b,a
		pop af
		out (0a8h),a
		ld a,b
		ld (0ffffh),a
		ret
				
	
MCFMTest:	ld	hl,0401Ch
MCFMP1		ld	de,MCFMText ; Esto por supesto debria cambiarlo
		ld	b,4
MCFMLP:		ld	a,(de)
		cp	(hl)
		jr	nz,MCFMt
		inc	hl
		inc	de
		djnz	MCFMLP
		cp	a
		jr	MCFMt
	
MCFMFnd:	pop	hl
		pop	af
		pop	bc
		pop	bc
	
MCFM_Check:	ld	hl,04018h
MCFMP2		ld	de,MCFM_Japones	; al igual que esto
		ld	b,4
MCJapo:		ld	a,(de)
		cp	(hl)
		jr	nz,MCEs_Japones
		inc	hl
		inc	de
		djnz	MCJapo	
		jr	MCRecover
MCEs_Japones:		
		ld	a,(07FF6h)
		or	1
		ld	(07FF6h),a
		ei
		jr	MCRecover
				
MCFMText:	db	"OPLL"
MCFM_Japones:	db	"APRL"	
MCEND:		db	0	
	
				

	
			
		
;****************************************************************************
;Funcion execute on each interrupt
;;; ESTAN MAL LOS COMENTARIOS DE LOS FADES!!!!!!


	
IntFunct:			
		ld	hl,FadeContAct
		ld	a,(FadeCtl)
		bit	6,a
		jp	z,FadeOutR	; Fade in?
		and	3Fh
		cp	(hl)
		jp	z, FadeInP	; do it	reach cont of ints of fade?
	
		inc	(hl)
		jp	BeginDeco
;****************************************************************************

FadeInP:	
		xor	a		; It is	necesary does a	fade out pass
		ld	(hl), a
		inc	hl
		dec	(hl)
		jp	nz, BeginDeco	; Is it	fade finish?
		xor	a
		ld	(FadeCtl), a
		jp	BeginDeco
;****************************************************************************
;This code is which deal with fade out command

FadeOutR:	
		cp	80h
		jp	c, BeginDeco	; is fade out command active?
		and	3Fh
		cp	(hl)		; Is necesary decrement	volumen?
		jp	z, FadeOutAct
		inc	(hl)		; Not yet
		jp	BeginDeco
;****************************************************************************
;This code decrement volumen for fade out and actualize	all variables implicates on fade out

FadeOutAct:	
		xor	a
		ld	(hl), a
		inc	hl
		inc	(hl)
		ld	a, (hl)
		cp	10h
		jp	c, BeginDeco	; did it finished fade out?
		ld	a, (FadeOutRest); Is active option of decodification on	silence?
		or	a
		jp	z, FadeORest
		ld	(SilenceFlag), a; yes, it is
		xor	a
		ld	(FadeCtl), a
		call	initchips
		jp	BeginDeco
;****************************************************************************
;This code clean song at all

FadeORest:	
		xor	a
		ld	(ChannelOFF), a
		call	initvars
		call	initchips
  
BeginDeco:
		ld	a, 80h
		ld	(TimeLastChn), a
		ld	a, (SFX_ON)
		ld	(MaskSFX), a
		ld	a, (NumDecLeft)
		ld	b, a
		ld	a, (NumDecInt)
		add	a, b

DecoLoop:				; ...
		cp	4Bh
		jp	c, EndLoopInt
		sub	4Bh
		push	af
		ld	ix, ChnTblSong-46
		xor	a
		ld	(TimeLastChn), a
		ld	a, 80h
		call	DecoChannel
		ld	a, 81h
		call	DecoChannel
		ld	a, 82h
		call	DecoChannel
		xor	a
		call	DecoChannel
		ld	a, 1
		call	DecoChannel
		ld	a, 2
		call	DecoChannel
		ld	a, 3
		call	DecoChannel
		ld	a, 4
		call	DecoChannel
		ld	a, 5
		call	DecoChannel
		ld	a, 6
		call	DecoChannel
		ld	a, 7
		call	DecoChannel
		ld	a, 8
		call	DecoChannel 
		pop	af 
		jp	DecoLoop
;****************************************************************************

EndLoopInt:				; ...
		ld	(NumDecLeft), a
		ld	a, (SFX_ON)
		or	a
		jr	nz, DecoSFX
		ld	a, (TimeLastChn)
		or	a
		ret	nz
		ld	a, (SONG_ON)
		or	a
		jp	nz, QuitSong
		ld	(ChannelOFF), a
		ret	
;****************************************************************************

DecoSFX:				; ...
		ld	ix, ChnSFXPSG1-46
		xor	a
		ld	(TimeLastChn), a
		ld	(MaskSFX), a
		ld	a, 82h
		call	DecoChannel
		ld	a, 81h
		call	DecoChannel
		ld	a, (TimeLastChn)
		or	a
		ret	nz
		ld	(SFX_ON), a
		ret	
;****************************************************************************
;Function which	decode a channel

DecoChannel:				; ...
		ld	de, 46
		add	ix, de
		ld	(ChannelAct), a
		and	80h
		ld	b, a
		ld	a, (ChannelAct)
		cp	6
		jr	c, DecoChnMel
		ld	a, (ShdFM_E)	; Channell can be a FM Rhythm channel
		and	20h		; Save FM Rhythm status
		or	b
		ld	b, a

DecoChnMel:				; ...
		ld	a, (ix+0Ch)
		and	0Fh
		or	b
		ld	(ix+0Ch), a
		ld	a, (ix+0)
		or	a
		jp	z, ChangeModeChn; if time decoding is 0	it means that channell is inactive thus	it is only necessary actualize channel mode

		ld	a, 1		; Time of Channel is not finished
		ld	(TimeLastChn), a
		call	PlayEffect	; Make actions for efecct command
		dec	(ix+0)		; Decrement channel count
		bit	7, (ix+0Bh)	; is channel executing a rest?
		jr	nz, DecoChnADSR

		ld	a, (ix+0Ah)	; If timbre time is defined
		cp	(ix+0)
		jr	nz, DecoChnADSR

		or	a		; If time is diferent of 0 mean	that is	reached	maximun	time of	active note
		jr	nz, DecoChnTimeOff; and	is necessary put off channel sound

		ld	a, (ChannelAct)
		add	a, a
		jr	c, DecoChnADSR	; on FM	channels is necessary desactive	note on	final count

DecoChnTimeOff:				; ...
		call	ChangeModeChn	; It is	necesary put on	silence	this channel

DecoChnADSR:				; ...
		ld	a, (ix+0)	; Actions necesary after deal adsr duration
		or	a
		jr	z, DecoNext

		ld	a, (ix+0Dh)	; Is there a adsr definition?
		or	a
		jr	z, WriteAdsrVol

		dec	(ix+0Dh)	; and is it necessary change volumen of	channel	for adsr definition?
		jr	nz, WriteAdsrVol

		ld	a, (ix+10h)	; Change Volumen value
		add	a, (ix+9)
		jr	nc, adsrDef

		cp	(ix+0Fh)	; is sustain level reach?
		jr	c, WriteAdsrVol

		jr	adsrSustain
;****************************************************************************

adsrDef:				; ...
		cp	(ix+0Fh)	; Does adsr rect reach its sustain level?
		jr	z, adsrSustain
		jr	nc, WriteAdsrVol

adsrSustain:				; ...
		ld	(ix+9),	a
		call	WriteVol
		ld	a, (ix+0Eh)
		ld	(ix+0Dh), a
		jr	DecoChnVib
;****************************************************************************

WriteAdsrVol:				; ...
		ld	a, (FadeCtl)
		bit	7, a
		ld	a, (ix+9)
		call	nz, WriteVol	; Write	new volumen if fade is inactive

DecoChnVib:				; ...
		ld	a, (ix+12h)	; Is vib command define?
		or	a
		ret	z

		dec	(ix+12h)
		ret	nz
		ld	a, (ix+13h)	; If time of vib is finished
		ld	(ix+12h), a
		ld	a, (ix+5)	; add lsb frecuency adder
		add	a, (ix+14h)
		ld	(ix+5),	a
		ld	a, (ix+6)
		adc	a, (ix+15h)	; and MSB frecuency adder
		ld	(ix+6),	a
		jp	WriteFrec
;****************************************************************************


			
DecoNext:				; ...
		xor	a
		ld	(ix+12h), a
		ld	a, (ix+0Eh)
		or	a
		jr	z, DecopOP1
		ld	(ix+0Dh), a
		ld	a, (ix+11h)
		ld	(ix+9),	a

DecopOP1:				; ...
		ld	l, (ix+1)
		ld	h, (ix+2)
		ld	a, (hl)
		or	a
		jp	z, ChangeModeChn
		inc	hl
		ld	(ix+1),	l
		ld	(ix+2),	h

	
		dec a
DecoOP2:	ld l,a
		ld e,a
		ld h,0
		ld d,h
		add hl,hl
		add hl,de
		ld de, TableOP
		add hl,de

DecoOP3:				; ...
		ld	e, (hl)
		inc	hl		; de ->	Address	of opcode routine
		ld	d, (hl)		; a -> parameter of routine
		inc	hl
		ld	a,(hl)
		ld	hl, DecopOP1	; put return address
		push	hl
		push	de
		ret







	

TableOp:	dw OP_play
		db 0	; 1
		dw OP_play
		db 1	; 2
		dw OP_play
		db 2	; 3
		dw OP_play
		db 3	; 4
		dw OP_play
		db 4	; 5
		dw OP_play
		db 5	; 6
		dw OP_play
		db 6	; 7
		dw OP_play
		db 7	; 8
		dw OP_play
		db 8	; 9
		dw OP_play
		db 9	; a
		dw OP_play
		db 10	; b
		dw OP_play
		db 11	; c
		dw OP_play
		db 12	; d
		dw OP_play
		db 13	; e
		dw OP_play
		db 14	; f
		dw OP_play
		db 15	; 10
		dw OP_play
		db 16	; 11
		dw OP_play
		db 17	; 12
		dw OP_play
		db 18	; 13
		dw OP_play
		db 19	; 14
		dw OP_play
		db 20   ; 15
		dw OP_play
		db 21   ; 16
		dw OP_play
		db 22	; 17
		dw OP_play
		db 23	; 18
		dw OP_play
		db 24	; 19
		dw OP_play
		db 25	; 1a
		dw OP_play
		db 26	; 1b
		dw OP_play
		db 27	; 1c
		dw OP_play
		db 28	; 1d
		dw OP_play
		db 29	; 1e
		dw OP_play
		db 30	; 1f
		dw OP_play
		db 31	; 20
		dw OP_play
		db 32	; 21
		dw OP_play
		db 33	; 22
		dw OP_play
		db 34	; 23
		dw OP_play
		db 35	; 24
	
		dw OP_rest
		db 0    ; 25
	
		dw OP_moct
		db 0	; 26
		dw OP_moct
		db 1	; 27
	
		dw OP_oct
		db 0	; 28
		dw OP_oct
		db 1	; 29
		dw OP_oct
		db 2	; 2a
		dw OP_oct
		db 3	; 2b
		dw OP_oct
		db 4	; 2c
		dw OP_oct
		db 5	; 2d	
		dw OP_oct
		db 6	; 2e	
		dw OP_oct
		db 7	; 2f	
						
		dw OP_mvol
		db 0	; 30
		dw OP_mvol
		db 1	; 31

		dw OP_vol
		db 0	; 32
		dw OP_vol
		db 1	; 33
		dw OP_vol
		db 2	; 34
		dw OP_vol
		db 3	; 35
		dw OP_vol
		db 4	; 36
		dw OP_vol
		db 5	; 37
		dw OP_vol
		db 6	; 38
		dw OP_vol
		db 7	; 39
		dw OP_vol
		db 8	; 3a
		dw OP_vol
		db 9	; 3b
		dw OP_vol
		db 10	; 3c
		dw OP_vol
		db 11	; 3d
		dw OP_vol
		db 12	; 3e
		dw OP_vol
		db 13	; 3f
		dw OP_vol
		db 14	; 40
		dw OP_vol
		db 15	; 41
	
		dw OP_FMinst
		db 0	; 42
		dw OP_FMinst
		db 1	; 43
		dw OP_FMinst
		db 2	; 44
		dw OP_FMinst
		db 3	; 45
		dw OP_FMinst
		db 4  ; 46
		dw OP_FMinst
		db 5  ; 47
		dw OP_FMinst
		db 6  ; 48
		dw OP_FMinst
		db 7	; 49
		dw OP_FMinst
		db 8  ; 4a
		dw OP_FMinst
		db 9	; 4b
		dw OP_FMinst
		db 10	; 4c
		dw OP_FMinst
		db 11	; 4d
		dw OP_FMinst
		db 12	; 4e
		dw OP_FMinst
		db 13	; 4f
		dw OP_FMinst
		db 14	; 50
		dw OP_FMinst
		db 15	; 51
		dw OP_FMinst
		db 16	; 52
		dw OP_FMinst
		db 17 ; 53
		dw OP_FMinst
		db 18 ; 54
		dw OP_FMinst
		db 19	; 55
		dw OP_FMinst
		db 20	; 56
		dw OP_FMinst
		db 21	; 57
		dw OP_FMinst
		db 22	; 58
		dw OP_FMinst
		db 23	; 59
		dw OP_FMinst
		db 24	; 5a
		dw OP_FMinst
		db 25	; 5b
		dw OP_FMinst
		db 26	; 5c
		dw OP_FMinst
		db 27	; 5d
		dw OP_FMinst
		db 28	; 5e
		dw OP_FMinst
		db 29	; 5f
		dw OP_FMinst
		db 30	; 60
		dw OP_FMinst
		db 31	; 61
	
		dw OP_loop
		db 0	; 62
		dw OP_loop
		db 1	; 63
		dw OP_loop
		db 2	; 64
		dw OP_loop
		db 3	; 65
		dw OP_loop
		db 4	; 66
		dw OP_loop
		db 5	; 67
		dw OP_loop
		db 6	; 68
		dw OP_loop
		db 7	; 69
		dw OP_loop
		db 8	; 6a
		dw OP_loop
		db 9	; 6b
		dw OP_loop
		db 10	; 6c
		dw OP_loop
		db 11	; 6d
		dw OP_loop
		db 12	; 6e
		dw OP_loop
		db 13	; 6f
		dw OP_loop
		db 14	; 70
		dw OP_loop
		db 15	; 71
	
		dw OP_endl
		db 0	; 72
		dw OP_adsr
		db 0	; 73
		dw OP_adsr
		db 1	; 74
		dw OP_adsr
		db 2	; 75
		dw OP_adsr
		db 3	; 76
		dw OP_adsr
		db 4	; 77
		dw OP_adsr
		db 5	; 78
		dw OP_adsr
		db 6	; 79
		dw OP_adsr
		db 7	; 7a
		dw OP_adsr
		db 8	; 7b
		dw OP_adsr
		db 9	; 7c
		dw OP_adsr
		db 10	; 7d
		dw OP_adsr
		db 11	; 7e
		dw OP_adsr
		db 12	; 7f
		dw OP_adsr
		db 13	; 80
		dw OP_adsr
		db 14	; 81
		dw OP_adsr
		db 15	; 82
	
		dw OP_vib
		db 0	; 83
		dw OP_vib
		db 1	; 84
		dw OP_vib
		db 2	; 85
		dw OP_vib
		db 3	; 86
		dw OP_vib
		db 4	; 87
		dw OP_vib
		db 5	; 88
		dw OP_vib
		db 6	; 89
		dw OP_vib
		db 7	; 8a
		dw OP_vib
		db 8	; 8b
		dw OP_vib
		db 9	; 8c
		dw OP_vib
		db 10	; 8d
		dw OP_vib
		db 11	; 8e
		dw OP_vib
		db 12	; 8f
		dw OP_vib
		db 13	; 90
		dw OP_vib
		db 14	; 91
		dw OP_vib
		db 15	; 92
	
		dw OP_XXX
		db 0	; 93
		dw OP_XXX
		db 1	; 94
		dw OP_XXX
		db 2	; 95
		dw OP_XXX
		db 3	; 96
	
		dw OP_time
		db 0	; 97
		dw OP_time
		db 1	; 98
		dw OP_time
		db 2	; 99
		dw OP_time
		db 3	; 9a
		dw OP_time
		db 4	; 9b
		dw OP_time
		db 5	; 9c
		dw OP_time
		db 6	; 9d
		dw OP_time
		db 7	; 9e
		dw OP_time
		db 8	; 9f
		dw OP_time
		db 9	; a0
		dw OP_time
		db 10	; a1
		dw OP_time
		db 11	; a2
		dw OP_time
		db 12	; a3
		dw OP_time
		db 13	; a4
		dw OP_time
		db 14	; a5
		dw OP_time
		db 15	; a6
		dw OP_time
		db 16	; a7
		dw OP_time
		db 17	; a8
		dw OP_time
		db 18	; a9
		dw OP_time
		db 19	; aa
		dw OP_time
		db 20	; ab
		dw OP_time
		db 21	; ac
		dw OP_time
		db 22	; ad
		dw OP_time
		db 23	; ae
		dw OP_time
		db 24	; af
		dw OP_time
		db 25	; b0
		dw OP_time
		db 26	; b1
		dw OP_time
		db 27	; b2
		dw OP_time
		db 28	; b3
		dw OP_time
		db 29	; b4
		dw OP_time
		db 30	; b5
		dw OP_time
		db 31	; b6
		dw OP_time
		db 32	; b7
		dw OP_time
		db 33	; b8
		dw OP_time
		db 34	; b9
		dw OP_time
		db 35	; ba
		dw OP_time
		db 36	; bb
		dw OP_time
		db 37	; bc
		dw OP_time
		db 38	; bd
		dw OP_time
		db 39	; be
		dw OP_time
		db 40	; bf
		dw OP_time
		db 41	; c0
		dw OP_time
		db 42	; c1
		dw OP_time
		db 43	; c2
		dw OP_time
		db 44	; c3
		dw OP_time
		db 45	; c4
		dw OP_time
		db 46	; c5
		dw OP_time
		db 47	; c6
		dw OP_time
		db 48	; c7
		dw OP_time
		db 49	; c8
		dw OP_time
		db 50	; c9
		dw OP_time
		db 51	; ca
		dw OP_time
		db 52	; cb
		dw OP_time
		db 53	; cc
		dw OP_time
		db 54	; cd
		dw OP_time
		db 55	; ce
		dw OP_time
		db 56	; cf
		dw OP_time
		db 57	; d0
		dw OP_time
		db 58	; d1
		dw OP_time
		db 59	; d2
		dw OP_time
		db 60	; d3
		dw OP_time
		db 61	; d4
		dw OP_time
		db 62	; d5
		dw OP_time
		db 63	; d6
		dw OP_time
		db 64	; d7
		dw OP_time
		db 65	; d8
		dw OP_time
		db 66	; d9
		dw OP_time
		db 67	; da
		dw OP_time
		db 68	; db
		dw OP_time
		db 69	; dc
		dw OP_time
		db 70	; dd
		dw OP_time
		db 71	; de
		dw OP_time
		db 72	; df
		dw OP_time
		db 73	; e0
		dw OP_time
		db 74	; e1
		dw OP_time
		db 75	; e2
		dw OP_time
		db 76	; e3
		dw OP_time
		db 77	; e4
		dw OP_time
		db 78	; e5
		dw OP_time
		db 79	; e6
		dw OP_time
		db 80	; e7
		dw OP_time
		db 81	; e8
		dw OP_time
		db 82	; e9
		dw OP_time
		db 83	; ea
		dw OP_time
		db 84	; eb
		dw OP_time
		db 85	; ec
		dw OP_time
		db 86	; ed
		dw OP_time
		db 87	; ee
		dw OP_time
		db 88	; ef
		dw OP_time
		db 89	; f0
		dw OP_time
		db 90	; f1

		dw OP_divsus
		db 0	; f2
		dw OP_divsus
		db 1	; f3
		dw OP_divsus
		db 2	; f4
		dw OP_divsus
		db 3	; f5
		dw OP_divsus
		db 4	; f6
		dw OP_divsus
		db 5	; f7
		dw OP_divsus
		db 6	; f8
		dw OP_divsus
		db 7	; f9
		dw OP_timel
		db 0	; fa
		dw OP_loopl
		db 0	; fb
		dw OP_FMinstl
		db 0	; fc
		dw OP_WriteFM
		db 0	; fd
		dw OP_effect
		db 0	; fe
		dw OP_par
		db 0	; ff
	
	

;;;*********************************************************************************

OP_play:				; ...
		ld	c, (ix+4)
		ld	(ix+4),	a
		ld	hl, table_note
		call	ADD_HL_A4
		ld	a, (ChannelAct)
		cp	80h
		jr	c, OP_playFM

		inc	hl		; On PSG channels is necessary calculate absolute frecuency
		inc	hl
		ld	e, (hl)
		inc	hl
		ld	d, (hl)
		ld	b, (ix+7)
		inc	b

OP_playPSG1:				; ...
		srl	d
		rr	e
		djnz	OP_playPSG1
		jr	nc, OP_playPSG2
		inc	de

OP_playPSG2:				; ...
		ld	(ix+6),	d
		ld	(ix+5),	e
		jr	OP_PlayW
;****************************************************************************

OP_playFM:				; ...
		ld	a, (hl)
		ld	(ix+5),	a
		inc	hl
		ld	a, (hl)
		ld	(ix+6),	a

OP_PlayW:				; ...
		call	WriteFrec
		ld	a, (ix+9)
		call	WriteVol
		ld	a, (ix+0Bh)	; this channel play a rest just	before this play?
		cp	80h
		jr	nc, OP_playEnd

		add	a, 40h		; Put flag of active channel
		jp	p, OP_PlayConf	; It is	necesary reconfigure channel?
		ld	(ix+0Bh), a

OP_PlayConf:				; ...
		call	ChangeModeChn
		call	ActChannel

OP_playEnd:				; ...
		ld	a, (ix+3)	; Put duration of note
		ld	(ix+0),	a
		pop	hl		; Ending decodification	of channel
		ret	
;****************************************************************************

OP_rest:				; ...
		ld	(ix+0Ch), 0
		call	ChangeModeChn
		ld	a, (ix+3)
		ld	(ix+0),	a
		pop	hl
		ld	a, (ix+0Bh)	; Channel is active
		add	a, 40h
		ret	p
		and	7Fh		; and play a rest
		ld	(ix+0Bh), a
		ret	
;****************************************************************************

OP_moct:				; ...
		or	a
		jr	nz, OP_octADD
		dec	a

OP_octADD:				; ...
		add	a, (ix+7)
		and	7

OP_oct:					; ...
		ld	(ix+7),	a
		ret	
;****************************************************************************

OP_mvol:				; ...
		or	a
		jr	nz, OP_volADD
		dec	a

OP_volADD:				; ...
		add	a, (ix+8)
		and	0Fh

OP_vol:					; ...
		ld	(ix+8),	a
		ld	(ix+0Eh), 0	; Clean	adsr data
		ld	(ix+0Dh), 0
		bit	7, (ix+0Ch)
		jr	nz, OP_volEnd	; is a PSG channel?
		ld	b, a
		ld	a, 0Fh
		sub	b

OP_volEnd:				; ...
		ld	(ix+9),	a
		ret	
;****************************************************************************

OP_FMinstl:				; ...
		call	GetNextOP

OP_FMinst:				; ...
		bit	7, (ix+0Ch)
		jr	z, OP_FM_INST
		ld	(ix+6),	80h	; Put PSG channel as noise
		ld	(ix+5),	a
		jp	OP_PlayW
;****************************************************************************

OP_FM_INST:				; ...
		cp	10h
		jr	nc, OP_FMintSoft
		ld	(ix+16h), a
		jr	WFMinst2
;****************************************************************************

OP_FMintSoft:				; ...
		ld	(ix+16h), 0	; FM instrument	software
		sub	10h
		ld	(FM_InstSoft), a

WriteFMInst:				; ...
		ld	l, a
		ld	h, 0
		add	hl, hl
		add	hl, hl
		add	hl, hl
		ld	de, (ptable_fmins)
		add	hl, de
		ld	b, 0

WFMinst1:				; ...
		ld	a, (hl)
		inc	hl
		call	WriteFM
		inc	b
		bit	3, b
		jr	z, WFMinst1

WFMinst2:				; ...
		ret	
;****************************************************************************

OP_loopl:				; ...
		call	GetNextOP

OP_loop:				; ...
		push	ix
		pop	hl
		ld	de, 2Dh
		add	hl, de
		ld	d, h
		ld	e, l
		dec	hl
		dec	hl
		dec	hl
		ld	bc, 0Ch
		lddr	
		inc	hl
		ld	(hl), a
		ld	a, (ix+1)
		inc	hl
		ld	(hl), a
		ld	a, (ix+2)
		inc	hl
		ld	(hl), a
		ret	
;****************************************************************************

OP_endl:				; ...
		ld	a, (ix+1Fh)
		or	a
		jr	z, OP_endl1
		dec	a
		jr	z, OP_endl2
		ld	(ix+1Fh), a

OP_endl1:				; ...
		ld	a, (ix+20h)	; It is	necesary other loop iteration
		ld	(ix+1),	a
		ld	a, (ix+21h)
		ld	(ix+2),	a
		ret	
;****************************************************************************

OP_endl2:				; ...
		push	ix		; loop cont is finished
		pop	hl
		ld	de, 1Fh
		add	hl, de
		ex	de, hl
		ld	hl, 3
		add	hl, de
		ld	bc, 0Ch
		ldir	
		ret	
;****************************************************************************

OP_adsr:				; ...
		ld	l, a
		add	a, a
		add	a, l
		ld	hl, (ptable_adsr)
		call	ADD_HL
		ld	a, (hl)
		ld	(ix+0Dh), a
		ld	(ix+0Eh), a
		inc	hl
		ld	a, (hl)
		ld	(ix+0Fh), a
		inc	hl
		ld	a, (hl)
		ld	(ix+10h), a
		ld	a, (ix+9)
		ld	(ix+11h), a
		ret	
;****************************************************************************

OP_vib:					; ...
		ld	l, a
		add	a, a
		add	a, l
		ld	hl, (ptable_vib)
		call	ADD_HL
		ld	a, (hl)
		ld	(ix+12h), a
		ld	(ix+13h), a
		inc	hl
		ld	a, (hl)
		ld	(ix+14h), a
		inc	hl
		ld	a, (hl)
		ld	(ix+15h), a
		ret	
;****************************************************************************

OP_XXX:					; ...
		sub	2
		jr	c, OP_XXX1
		inc	a

OP_XXX1:				; ...
		add	a, (ix+3)

OP_timel:				; ...
		call	GetNextOP

OP_time:				; ...
		ld	(ix+3),	a
		jr	ActTimeSound
;****************************************************************************

OP_divsus:				; ...
		ld	b, a
		ld	a, (ix+0Bh)
		and	0F0h
		or	b
		ld	(ix+0Bh), a

ActTimeSound:				; ...
		ld	h, (ix+3)
		xor	a
		srl	h
		rra	
		srl	h
		rra	
		srl	h
		rra	
		ld	l, a
		ld	a, (ix+0Bh)
		and	7
		ld	d, 0
		ld	e, a
		call	MulHL_DE
		ld	(ix+0Ah), h
		ret	
;****************************************************************************

OP_WriteFM:				; ...
		call	GetNextOP
		ld	b, a
		call	GetNextOP
		jp	WriteFM
;****************************************************************************

OP_effect:				; ...
		call	GetNextOP
		ld	hl, (ptable_effect)
		call	ADD_HL_A4
		ld	a, (hl)
		ld	(ix+17h), a
		inc	hl
		ld	a, (hl)
		ld	(ix+18h), a
		inc	hl
		ld	a, (hl)
		ld	(ix+1Ah), a
		inc	hl
		ld	a, (hl)
		ld	(ix+1Bh), a
		jp	InitEffect
;****************************************************************************

OP_par:					; ...
		call	GetNextOP
		ld	hl, tblParAddress
		call	ADD_HL_A2
		ld	a, (hl)
		inc	hl
		ld	h, (hl)
		ld	l, a
		jp	(hl)
;****************************************************************************
tblParAddress:	dw DisableChannel	; ...
		dw ActiveChannel
		dw DisableSus
		dw EnableSus
		dw ActiveSD_HH
		dw ActiveTOM_TCY
		dw ActiveNoise
		dw DisableNoise
		dw AddUnitsInt
		dw SetTimenSFX
		dw ActiveRhythm
		dw DisableRhythm
;****************************************************************************

DisableChannel:				; ...
		set	6, (ix+0Bh)
		ret	
;****************************************************************************

ActiveChannel:				; ...
		ld	a, (ix+0Bh)
		and	37h
		ld	(ix+0Bh), a
		jp	ChangeModeChn
;****************************************************************************

DisableSus:				; ...
		res	5, (ix+0Bh)	; Disable sustain hard on FM channel
		ret	
;****************************************************************************

EnableSus:				; ...
		set	5, (ix+0Bh)
		ret	
;****************************************************************************

ActiveSD_HH:				; ...
		ld	hl, Rhythm_Mode	; Active SD Instrument
		ld	a, (ChannelAct)
		cp	7
		jr	nz, ActiveHHInst

		res	2, (hl)
		set	3, (hl)
		ret	
;****************************************************************************

ActiveHHInst:				; ...
		res	1, (hl)
		set	0, (hl)
		ret	
;****************************************************************************

ActiveTOM_TCY:				; ...
		ld	hl, Rhythm_Mode
		ld	a, (ChannelAct)
		cp	7
		jr	nz, ActiveTCY

		set	2, (hl)
		res	3, (hl)
		ret	
;****************************************************************************

ActiveTCY:				; ...
		set	1, (hl)
		res	0, (hl)
		ret	
;****************************************************************************

ActiveNoise:				; ...
		res	0, (ix+0Ch)
		ret	
;****************************************************************************

DisableNoise:				; ...
		set	0, (ix+0Ch)
		ret	
;****************************************************************************

AddUnitsInt:				; ...
		call	GetNextOP	; Add units of time on each interrupt
		ld	(NumDecInt), a
		ret	
;****************************************************************************

SetTimenSFX:				; ...
		call	GetNextOP	; Puts number of interrupt that	are needed for begining	of effect (envl)
		ld	(ix+1Eh), a
		ret	
;****************************************************************************

ActiveRhythm:				; ...
		ld	a, 20h
		ld	(ShdFM_E), a
		ld	b, 0Eh
		call	WriteFM
		ret	
;****************************************************************************

DisableRhythm:				; ...
		xor	a
		ld	(ShdFM_E), a
		ld	b, 0Eh
		call	WriteFM
		ret	
;****************************************************************************
		db 18h
		db 0FEh
;****************************************************************************
;a -> New volumen level

WriteVol:				; ...
		push	hl
		push	bc
		push	de
		ld	c, a
		ld	a, (FadeCtl)
		and	80h
		jr	z, WVolChip
		ld	hl, FadeNumPass
		bit	7, (ix+0Ch)
		jr	z, WVolPSG
		ld	a, c
		sub	(hl)
		jr	nc, WVolPSG1
		xor	a
		jr	WVolPSG1
;****************************************************************************

WVolPSG:				; ...
		ld	a, c
		add	a, (hl)
		cp	0Fh
		jr	c, WVolPSG1
		ld	a, 0Fh

WVolPSG1:				; ...
		ld	c, a

WVolChip:				; ...
		ld	a, (SilenceFlag)
		or	a
		jr	nz, WriteVolEnd
		ld	a, (ChannelAct)
		cp	80h
		jr	c, WVolFM
		ld	b, a
		ld	a, (MaskSFX)
		and	b
		jr	nz, WriteVolEnd
		ld	a, b
		add	a, 88h
		ld	b, a
		ld	a, c
		call	WritePSG
		jr	WriteVolEnd
;****************************************************************************

WVolFM:					; ...
		bit	5, (ix+0Ch)
		jr	z, VolFM_Mel	; Is a FM channel Ryhthm?
		cp	7
		jr	c, VolFM_Mel
		jr	z, VolFM_HH	; Is HH	Channel

		ld	a, c		; Is SD	Channel
		ld	(SD_VOL), a
		add	a, a
		add	a, a
		add	a, a
		add	a, a
		ld	c, a
		ld	a, (HH_VOL)
		jr	VolFM_WR
;****************************************************************************

VolFM_HH:				; ...
		ld	a, c
		ld	(HH_VOL), a
		ld	a, (SD_VOL)
		add	a, a
		add	a, a
		add	a, a
		add	a, a

VolFM_WR:				; ...
		or	c		; Write	register #37 (HH and SD	voluemn)
		ld	b, 37h
		call	WriteFM
		rrca	
		rrca	
		rrca	
		rrca	
		inc	b
		jr	WFM_Vol
;****************************************************************************

VolFM_Mel:				; ...
		add	a, 30h		; Write	volumen	in a FM	melody channel
		ld	b, a
		ld	a, (ix+16h)
		add	a, a
		add	a, a
		add	a, a
		add	a, a
		or	c

WFM_Vol:				; ...
		call	WriteFM

WriteVolEnd:				; ...
		pop	de
		pop	bc
		pop	hl
		ret	
;****************************************************************************
;Actualize hardware of a channel with tables valours after a play command

ActChannel:				; ...
		ld	a, (SilenceFlag)
		or	a
		ret	nz
		push	bc
		ld	a, (ChannelAct)
		cp	80h
		jr	nc, ActChnPSG

		bit	5, (ix+0Ch)
		jr	z, ActChnFM_m

		call	PutRhythm
		ld	b, a
		ld	a, (ShdFM_E)
		or	b
		ld	(ShdFM_E), a
		ld	b, 0Eh
		call	WriteFM
		jp	ChgChnModeEnd
;****************************************************************************

ActChnFM_m:				; ...
		add	a, 10h		; FM is	used on	mode melody
		ld	b, a
		ld	a, (ix+5)
		call	WriteFM
		ld	a, b
		add	a, 10h
		ld	b, a
		ld	a, (ix+7)
		add	a, a
		or	(ix+6)
		or	10h
		call	WriteFM
		set	4, (ix+0Bh)
		jr	ActChnSFX
;****************************************************************************

ActChnPSG:				; ...
		ld	b, a
		ld	a, (MaskSFX)
		and	b
		jr	nz, ActChnSFX
		ld	a, b
		and	3
		add	a, a
		jr	nz, ActChnPSG1
		ld	a, 1

ActChnPSG1:				; ...
		ld	c, a
		cpl	
		bit	0, (ix+0Ch)
		jr	z, ActChnPSG2	; PSG noise?
		ld	c, 0

ActChnPSG2:				; ...
		bit	7, (ix+6)
		jr	z, ActChnPSGn
		rlca	
		rlca	
		rlca	
		jr	ActChnPSG_7
;****************************************************************************

ActChnPSGn:				; ...
		rlc	c		; Actualize noise PSG channell
		rlc	c
		rlc	c

ActChnPSG_7:				; ...
		ld	b, a
		ld	a, (ShdPSG_7)
		and	b
		or	c
		ld	(ShdPSG_7), a
		ld	b, 7
		call	WritePSG

ActChnSFX:				; ...
		pop	bc		; does action for effect command
		bit	7, (ix+17h)
		ret	z
		ld	a, (ix+1Eh)
		or	a
		jr	z, acSFX_n1E	; Is there time	for type 0  of effect?
		ld	(ix+1Ch), a
		set	6, (ix+17h)
		ret	
;****************************************************************************

acSFX_n1E:				; ...
		bit	2, (ix+17h)	; Effect command with b7 of +17h set and no +1e	value
		call	nz, InitEffect
		ld	a, (ix+19h)
		jp	ApplyEffect
;****************************************************************************

ChangeModeChn:				; ...
		push	bc
		ld	a, (ChannelAct)
		cp	80h
		jr	c, ChgModeFM

		ld	b, a
		ld	a, (MaskSFX)
		and	b
		jr	nz, ChgChnModeEnd

		ld	a, b
		ld	c, 9
		and	3
		jr	z, ChgModePSG_W

		sla	c
		dec	a
		jr	z, ChgModePSG_W
		sla	c

ChgModePSG_W:				; ...
		bit	0, (ix+0Ch)
		jr	nz, ChgChnModeEnd
		ld	a, (ShdPSG_7)
		or	c
		ld	(ShdPSG_7), a
		ld	b, 7
		call	WritePSG
		jr	ChgChnModeEnd
;****************************************************************************

ChgModeFM:				; ...
		bit	5, (ix+0Ch)
		jr	z, ChgModeFM_Mel
		call	PutRhythm
		cpl	
		ld	b, a
		ld	a, (ShdFM_E)
		and	b
		ld	(ShdFM_E), a
		ld	b, 0Eh
		call	WriteFM
		jr	ChgChnModeEnd
;****************************************************************************

ChgModeFM_Mel:				; ...
		add	a, 20h		; It is	a melodyc FM channel
		ld	b, a
		ld	a, (ix+7)
		add	a, a
		or	(ix+6)
		ld	c, a
		ld	a, (ix+0Bh)
		and	20h
		or	c
		call	WriteFM
		res	4, (ix+0Bh)

ChgChnModeEnd:				; ...
		pop	bc
		ret	
;****************************************************************************
;Return	value for configuring channel as Rhythm	as indicate

PutRhythm:				; ...
		sub	6
		ld	b, a
		ld	a, 10h
		ret	z
		ld	a, (Rhythm_Mode)
		dec	b
		jr	nz, PutRhythm1
		and	0Ch
		ret	
;****************************************************************************

PutRhythm1:				; ...
		and	3
		ret	
;****************************************************************************

WriteFrec:				; ...
		push	hl
		ld	a, (ChannelAct)
		cp	80h
		jr	c, WFrecPSG
		ld	h, (ix+6)
		jr	WFrecFM
;****************************************************************************

WFrecPSG:				; ...
		ld	a, (ix+7)
		add	a, a
		or	(ix+6)
		ld	h, a

WFrecFM:				; ...
		ld	l, (ix+5)
		call	WriteFrecPar
		pop	hl
		ret	
;****************************************************************************

WriteFrecPar:				; ...
		push	bc
		ld	a, (ChannelAct)
		cp	80h
		jr	c, WriteFrecFM
		ld	b, a
		ld	a, (MaskSFX)
		and	b
		jr	nz, WrtFrecEnd
		bit	7, h
		jr	nz, WFrcPSG_Noise
		sla	b
		ld	a, l
		call	WritePSG
		inc	b
		ld	a, h
		jr	WriteFrecPSG1
;****************************************************************************

WFrcPSG_Noise:				; ...
		ld	b, 6
		ld	a, l

WriteFrecPSG1:				; ...
		call	WritePSG
		jr	WrtFrecEnd
;****************************************************************************

WriteFrecFM:				; ...
		bit	5, (ix+0Ch)
		jr	z, WrtFrecFM_W	; Is a rhythm channel?
		cp	7
		jr	c, WrtFrecFM_W	; Is one of multiplexed	rhythm channel?

		ld	a, (Rhythm_Mode)
		and	9
		ld	a, 7
		jr	nz, WrtFrecFM_W
		ld	a, 8

WrtFrecFM_W:				; ...
		add	a, 10h		; Write	new frecuency on OPLL chip
		ld	b, a
		ld	a, l
		call	WriteFM
		ld	a, b
		add	a, 10h
		ld	b, a
		ld	a, (ix+0Bh)	; Active KEY on	channel
		and	10h
		or	h
		call	WriteFM

WrtFrecEnd:				; ...
		pop	bc
		ret	
;****************************************************************************

GetNextOP:				; ...
		ld	h, (ix+2)
		ld	l, (ix+1)
		ld	a, (hl)
		inc	hl
		ld	(ix+2),	h
		ld	(ix+1),	l
		ret







	
;;; SFX !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	
;****************************************************************************

InitEffect:				; ...
		ld	a, (ix+17h)
		and	3
		ld	hl, TjmpInitEffect
		call	ADD_HL_A2
		ld	a, (hl)
		inc	hl
		ld	h, (hl)
		ld	l, a
		jp	(hl)
;****************************************************************************
TjmpInitEffect:	dw initSFXF0		; ...
		dw initSFXF1
		dw initSFXF2
		dw initSFXF2
;****************************************************************************

initSFXF0:				; ...
		xor	a
		ld	(ix+1Dh), a
		ld	(ix+1Ch), a
		ld	(ix+19h), a
		ret	
;****************************************************************************

initSFXF1:				; ...
		ld	a, (ix+18h)
		ld	(ix+19h), a
		ld	a, (ix+1Bh)
		ld	(ix+1Dh), a
		ld	a, (ix+1Ah)
		ld	(ix+1Ch), a

		ld	a, (ix+18h)
		or	a
		jp	m, InitSFX1_1
		bit	4, (ix+17h)
		jr	nz, InitSFX1_2
		jr	InitSFX1_3
;****************************************************************************

InitSFX1_1:				; ...
		bit	4, (ix+17h)
		jr	nz, InitSFX1_3

InitSFX1_2:				; ...
		neg	
		ld	(ix+18h), a

InitSFX1_3:				; ...
		ld	(ix+19h), a
		ret	
;****************************************************************************

initSFXF2:				; ...
		call	initSFXF0
		ld	a, (ix+18h)
		or	a
		jp	m, InitSFX2_1
		bit	4, (ix+17h)
		jr	nz, InitSFX2_2
		ret	
;****************************************************************************

InitSFX2_1:				; ...
		bit	4, (ix+17h)
		ret	nz

InitSFX2_2:				; ...
		neg	
		ld	(ix+18h), a
		ld	h, (ix+1Bh)
		ld	l, (ix+1Ah)
		call	NegHL
		ld	(ix+1Bh), h
		ld	(ix+1Ah), l
		ret	
;****************************************************************************
;This routine makes all	actions	necesary for effect command on each decodification of channell

PlayEffect:				; ...
		ld	a, (ix+17h)
		or	a
		ret	p

		bit	6, a
		jr	z, PlayEffectJ	; Is it	necessary init effect?

		dec	(ix+1Ch)
		ret	nz
		call	InitEffect
		res	6, (ix+17h)

PlayEffectJ:				; ...
		ld	a, (ix+17h)
		and	3
		ld	hl, TblPlaySFX
		call	ADD_HL_A2
		ld	a, (hl)
		inc	hl
		ld	h, (hl)
		ld	l, a
		jp	(hl)
;****************************************************************************

TblPlaySFX:	dw PlaySFXF0		; ...
		dw PlaySFXF1
		dw PlaySFXF2
		dw PlaySFXF2
;****************************************************************************

PlaySFXF0:				; ... square
		ld	a, (ix+1Ch)
		add	a, (ix+1Ah)
		ld	l, a
		ld	a, (ix+1Dh)
		adc	a, (ix+1Bh)
		ld	h, a
		call	CompareS
		jr	nc, PlaySFX0_1
		ld	h, (ix+18h)
		ld	l, 0
		call	NegHL

PlaySFX0_1:				; ...
		ld	a, h
		cp	(ix+19h)	; call only if value has change
		call	nz, ApplyEffect
		ld	(ix+1Dh), h
		ld	(ix+1Ch), l
		ret	
;****************************************************************************

PlaySFXF1:				; ... sawtoot
		ld	l, (ix+1Ch)
		ld	h, (ix+1Dh)
		dec	hl
		ld	a, h
		or	l
		jr	nz, PlaySFX1_1
		ld	a, (ix+18h)
		neg	
		ld	(ix+18h), a
		call	ApplyEffect
		ld	h, (ix+1Bh)
		ld	l, (ix+1Ah)

PlaySFX1_1:				; ...
		ld	(ix+1Ch), l
		ld	(ix+1Dh), h
		ret	
;****************************************************************************

PlaySFXF2:				; ... sin
		ld	a, (ix+1Ch)
		add	a, (ix+1Ah)
		ld	l, a
		ld	a, (ix+1Dh)
		adc	a, (ix+1Bh)
		ld	h, a
		call	CompareS
		jr	nc, PlaySFX2_1
		ex	de, hl
		ld	a, (ix+18h)
		neg	
		ld	(ix+18h), a
		ld	h, (ix+1Bh)
		ld	l, (ix+1Ah)
		call	NegHL
		ld	(ix+1Ah), l
		ld	(ix+1Bh), h
		ex	de, hl

PlaySFX2_1:				; ...
		ld	a, h
		cp	(ix+19h)
		call	nz, ApplyEffect
		ld	(ix+1Ch), l
		ld	(ix+1Dh), h
		ret	
;****************************************************************************

ApplyEffect:				; ...
		push	hl
		ld	(ix+19h), a
		bit	3, (ix+17h)	; Is volumen or	frecuency effect?
		jr	nz, FrecSFX

		add	a, (ix+9)	; Volumen effect
		ld	b, a
		ld	a, (ChannelAct)
		cp	80h
		ld	a, b
		jr	c, VolSFX_FM

		cp	10h
		jr	VolSFX_W
;****************************************************************************

VolSFX_FM:				; ...
		cp	80h

VolSFX_W:				; ...
		call	c, WriteVol	; Write	new volumen if there is	in adecuate range
		pop	hl
		ret	
;****************************************************************************


;;; <GuyveR800> LOL @ sign extend code
;;; <GuyveR800> ld l,a
;;; <GuyveR800> rlca
;;; <GuyveR800> sbc a,a
;;; <GuyveR800> ld h,a
	
FrecSFX:				; ...
		ld	l, a		; frecuency effect
		and	80h
		jr	z, FrecSFX1
		ld	a, 0FFh		; If is	a negative number then expand sign

FrecSFX1:				; ...
		ld	h, a
		ld	a, (ix+5)
		add	a, l
		ld	l, a
		ld	a, (ix+6)
		adc	a, h
		ld	h, a

		ld	a, (ChannelAct)
		cp	80h
		jr	nc, FrecSFX_W
		ld	a, (ix+7)	; if is	a FM channel is	necesary add octave
		add	a, a		; shift	one position
		or	h		; and add lsb of frecuency
		ld	h, a

FrecSFX_W:				; ...
		call	WriteFrecPar
		pop	hl
		ret




;;; MISC Routine!!!!!!!!

	
;****************************************************************************
;Output: C -> 0	if ix+24 is maior than h


;;; **********************************************************************
;;; Name:	CompareS
;;; Function:	Compare ix+24 with h
;;; Input:	ix -> Address -24 to be compared
;;;		h  -> value to be compared
;;; Output:	cf -> 
;;; Modify:	af
;;; **********************************************************************	

;;; <k0ga> compare ix+18 with h
;;; <GuyveR800> only the sign bit?
;;; <GuyveR800> ld a,h
;;; <k0ga> see the sub part
;;; <GuyveR800> cp (ix+18)
;;; <GuyveR800> ...
;;; <GuyveR800> cp = sub
;;; <k0ga> I know
;;; <GuyveR800> really, the coder didn't know the overflow flag
;;; <GuyveR800> this code makes sense
;;; <GuyveR800> it translates overflow flag into carry
;;; <GuyveR800> ld a,h
;;; <GuyveR800> cp (ix+18)
;;; <GuyveR800> jp pe/po
	
	
			
CompareS:				; ...
		bit	7, (ix+18h)
		jr	nz, CompareS1
		ld	a, h
		or	a
		ret	m
		sub	(ix+18h)
		ret	z
		ccf	
		ret

CompareS1:				; ...
		ld	a, h
		or	a
		ret	p
		sub	(ix+18h)
		ret
	
		
;****************************************************************************

NegHL:					; ...
		ld	a, h
		cpl	
		ld	h, a
		ld	a, l
		cpl	
		ld	l, a
		inc	hl
		ret	
;****************************************************************************



ADD_HL_A4:				; HL=HL+A*4 if a<40h
		add	a, a
		jr	nc, ADD_HL_A2   ; then this op is not necessary 
		inc	h	        ; and this to 

ADD_HL_A2:				; HL=HL+A*2 if a<80h
		add	a, a
		jr	nc, ADD_HL
		inc	h

ADD_HL:					; HL=HL+A
		add	a, l
		jr	nc, loc_0_8A22
		inc	h

loc_0_8A22:				; ...
		ld	l, a
		ret	
;****************************************************************************

MulHL_DE:				; ...
		push	bc
		ld	c, e
		ld	b, d
		ex	de, hl
		ld	hl, 0

MUL1:					; ...
		srl	b
		rr	c
		jr	nc, Mul2
		add	hl, de

Mul2:					; ...
		sla	e
		rl	d
		ld	a, b
		or	c
		jr	nz, MUL1
		pop	bc
		ret
	
;****************************************************************************
table_note:	db 0AAh, 0, 0FCh, 1Ah,	0ACh, 0, 0B7h, 1Ah; ...
		db 0ADh, 0, 73h, 1Ah, 0B4h, 0,	7Ah, 19h
		db 0B6h, 0, 39h, 19h, 0B7h, 0,	0F9h, 18h
		db 0BEh, 0, 14h, 18h, 0C0h, 0,	0D6h, 17h
		db 0C2h, 0, 9Ah, 17h, 0CAh, 0,	0B3h, 16h
		db 0CCh, 0, 79h, 16h, 0CEh, 0,	40h, 16h
		db 0D6h, 0, 6Dh, 15h, 0D8h, 0,	36h, 15h
		db 0DAh, 0, 0,	15h, 0E3h, 0, 39h, 14h,	0E5h
		db 0, 5, 14h, 0E7h, 0,	0D3h, 13h, 0F0h
		db 0, 16h, 13h, 0F2h, 0, 0E5h,	12h, 0F5h
		db 0, 0B5h, 12h, 0FEh,	0, 4, 12h, 1, 1
		db 0D6h, 11h, 3, 1, 0A9h, 11h,	0Dh, 1,	1
		db 11h, 10h, 1, 0D6h, 10h, 13h, 1, 0ABh
		db 10h, 1Dh, 1, 0Dh, 10h, 20h,	1, 0E4h
		db 0Fh, 23h, 1, 0BBh, 0Fh, 2Eh, 1, 26h
		db 0Fh, 32h, 1, 0FFh, 0Eh, 35h, 1, 0D9h
		db 0Eh, 40h, 1, 4Ch, 0Eh, 44h,	1, 28h,	0Eh
		db 47h, 1, 4, 0Eh
ptable_adsr:	dw Table_adsr		; ...
ptable_vib:	dw Table_vib		; ...
ptable_effect:	dw Table_effect	; ...
ptable_fmins:	dw Table_fmins		; ...
ChannelOFF:	db 0			; ...
TimeLastChn:	db 0			; ...
FadeOutRest:	db 0			; ...
					; 80h allow decodification follow on silence
SilenceFlag:	db 0			; ...
					; Flag that indicate that driver is on silence mode
SONG_ON:	db 0			; ...
SFX_ON:		db 0			; ...
MaskSFX:	db 0			; ...
					; Used for inhibit PSG1	and PSG2 when a	SFX is playing
FadeContAct:	db 0			; ...
					; Cont number of ints sinde last change	volumen	make for fade routine
FadeNumPass:	db 0			; ...
					; Cont Number of volumen changes for Fade routine
FadeCtl:	db 0			; ...
ChannelAct:	db 8			; ...
ShdPSG_7:	db 0BFh		; ...
FM_InstSoft:	db 0			; ...
					; Number of soft FM define
ChnSFXPSG1:	db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0; ...
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0; PSG1
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
ChnSFXPSG2:	db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0; PSG2
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
ChnTblSong:	db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0A0h; ...
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0; PSG0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0A0h; PSG1
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0A0h; PSG2
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0; FM0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0; FM1
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0; FM2
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0; FM3
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0; FM4
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0; FM5
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	20h; FM6
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	20h; FM7
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	20h; FM8
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0,	0
		db 0, 0, 0, 0,	0, 0, 0
		db 0
		db 0
ShdFM_E:	db 20h			; ...
Rhythm_Mode:	db 9			; ...
					; Indicate Rhythm instrument configuration (b0-b4 0=OFF	1=ON)
HH_VOL:		db 0			; ...
					; Volumen of FM	HD instrument
SD_VOL:		db 0			; ...
					; Volumen of FM	SD instrument
NumDecInt:	db 4Bh			; ...
					; Multipli of #4b does some iteration of main loop of decodification on	each interrupt
NumDecLeft:	db 0			; ...
					; Number of units of decodification that remaining from	last interrupt calling
ShadowVars:	db 0FFh, 0, 0FFh, 0FFh, 0, 0FFh, 0, 0,	0FFh; ...
		db 0, 0FFh, 0FFh, 0, 0FFh, 0, 0, 0FFh,	0
		db 0FBh, 0FFh,	0, 0FFh, 0, 0, 0EFh, 0,	0FFh
		db 0FFh, 0, 0FFh, 0, 0, 0DFh, 0, 7Fh, 0FFh
		db 0, 0FFh, 0,	0, 0FFh, 0, 0FFh, 0FFh,	0
		db 0FFh, 90h, 0, 0FFh,	0, 0FFh, 0FFh, 0
		db 0FFh, 0, 0,	0FFh, 0, 0FFh, 0FFh, 0,	0FFh
		db 0, 0, 0FFh,	0, 0FFh, 0FFh, 0, 0FFh,	0
		db 0, 0FFh, 0,	0FFh, 0FFh, 80h, 0FFh, 0
		db 0, 0FFh, 0,	8Fh, 0FFh, 0, 0FFh, 0, 0
		db 0FFh, 0, 0FFh, 0FFh, 0, 0FFh, 0, 0,	0FFh
		db 0, 7Fh, 0FFh, 0, 0FFh, 0, 0, 0FFh, 0
		db 0FFh, 0FFh,	0, 0FFh, 0, 0, 0FFh, 0,	0FFh
		db 0FFh, 0, 0FFh, 0, 0, 0FFh, 0, 0FFh,	0FFh
		db 0, 0FFh, 0,	0, 0FFh, 0, 0FFh, 0FFh,	0
		db 0FFh, 0, 0,	0FFh, 0, 0FFh, 0FFh, 0,	0FFh
		db 0, 0, 0FFh,	0, 0FFh, 7Fh, 0, 0FFh, 0
		db 0, 0FFh, 0,	0FFh, 0FFh, 0, 0FFh, 0,	0
		db 0FFh, 0, 0FFh, 0FFh, 0, 0FFh, 0, 0,	0FFh
		db 0, 0FFh, 0FFh, 0, 0FFh, 0, 0, 0FFh,	0
		db 0FFh, 0FFh,	0, 0FFh, 0, 0, 0FFh, 0,	0FFh
		db 0FFh, 0, 0FFh, 0, 0, 0FFh, 0, 0FFh,	0FFh
		db 0, 0FFh, 0,	0, 0FFh, 0, 0FFh, 0FFh,	0
		db 0FFh, 0, 0,	0FFh, 0, 0FFh, 0FFh, 0,	0FFh
		db 0, 0, 0FFh,	0, 0FFh, 0FFh, 0, 0FFh,	0
		db 0, 0FFh, 0,	0FFh, 0FFh, 0, 0FFh, 0,	0
		db 0FFh, 0, 0FFh, 0FFh, 0, 0FFh, 0, 0,	0FFh
		db 0, 0FFh, 0FFh, 0, 0FFh, 0, 0, 0FFh,	0
		db 0FFh, 0FFh,	0, 0FFh, 0, 0, 0FFh, 0,	0FFh
		db 0FFh, 0, 0FFh, 0, 0, 0FFh, 0, 0FFh,	0FFh
		db 0, 0FFh, 0,	0, 0FFh, 0, 0FFh, 0FFh,	0
		db 0FFh, 0, 0,	0FFh, 0, 0FFh, 0FFh, 0,	0FFh
		db 0, 0, 0FFh,	0, 0FFh, 0FFh, 0, 0FFh,	0
		db 0, 0FFh, 0,	0FFh, 0FFh, 0, 0FFh, 0,	0
		db 0FFh, 0, 0FFh, 0FFh, 0, 0FFh, 0, 0,	0FFh
		db 0, 0FFh, 0FFh, 0, 0FFh, 0, 0, 0FFh,	0
		db 0FFh, 0FFh,	0, 0FFh, 0, 0, 0FFh, 0,	0FFh
		db 0FFh, 0, 0FFh, 0, 0, 0FFh, 0, 0FFh,	0FFh
		db 0, 0FFh, 0,	0, 0FFh, 0, 0FFh, 0FFh,	0
		db 0FFh, 0, 0,	0FFh, 0, 0FFh, 0FFh, 0,	0FFh
		db 0, 0, 0FFh,	0, 0FFh, 0FFh, 0, 0FFh,	0
		db 0, 0FFh, 0,	0FFh, 0FFh, 0, 0FFh, 0,	0
		db 0FFh, 0, 0FFh, 0FFh, 0, 0FFh, 0, 0,	0FFh
		db 0, 0FFh, 0FFh, 0, 0FFh, 0, 0, 0FFh,	0
		db 0FFh, 0FFh,	0, 0FFh, 0, 0, 0FFh, 0,	0FFh
		db 77h, 0, 0FFh, 0, 0,	0FFh, 0, 0FFh, 0FFh
		db 0, 0FFh, 0,	0, 0FFh, 0, 0FFh, 0FFh,	0
		db 0FFh, 0, 0,	0FFh, 0, 0FFh, 0FFh, 0,	0FFh
		db 0, 0, 0FFh,	0, 7Fh,	0FFh, 0, 0FFh, 0
		db 0, 0FFh, 0,	0FFh, 0FFh, 0, 0FFh, 0,	0
		db 0FFh, 0, 0FFh, 0FFh, 10h, 0FFh, 0, 0
		db 0FFh, 0, 0FFh, 0FFh, 0, 0FFh, 0, 0,	0FFh
		db 0, 0FFh, 0FFh, 0, 0FFh, 0, 0, 0FFh,	0
		db 0FFh, 0FFh,	0, 0FFh, 0, 0, 0FFh, 0,	0FFh
		db 0FFh, 0, 0FFh, 0, 0, 0FFh, 0, 0FFh,	0FFh
		db 0, 0FFh, 0,	0, 7Fh,	0, 0FFh, 0FFh, 0
		db 0FFh, 0, 0,	0FFh, 0, 0FFh, 0FFh, 0,	0FFh
		db 0, 0, 0FFh,	0, 0FFh, 0FFh, 0, 0FFh,	0
		db 0, 0FFh, 0,	0FFh, 0FFh, 0, 0FFh, 0,	0
		db 0FFh, 0, 0FBh, 0FFh, 0, 0FFh, 0, 0,	0EFh
		db 0, 0FFh, 0FFh, 0, 0FFh, 0, 0, 0FFh,	0
		db 7Fh, 0FFh, 0, 0FFh,	0, 0, 0FFh, 0, 0FFh
		db 0FFh, 0, 0FFh, 90h,	0, 0FFh, 0, 0FFh
		db 0FFh, 0, 0FFh, 0, 0, 0FFh, 0, 0FFh,	0FFh
		db 0, 0FFh, 0,	0, 0FFh, 0, 0FFh, 0FFh,	0
		db 0FFh, 0, 0,	0FFh, 0, 0FFh, 0FFh, 80h
		db 0FFh, 0, 0,	0FFh, 0, 0CFh, 0FFh, 0,	0FFh
		db 0, 0, 0FFh,	0, 0FFh, 0FFh, 0, 0FFh,	0
		db 0, 0FFh, 0,	7Fh, 0FFh, 0, 0FFh, 0, 0
		db 0FFh, 0, 0FFh, 0FFh, 0, 0FFh, 0, 0,	0FFh
		db 0, 0FFh, 0FFh, 0, 0FFh, 0, 0, 0FFh,	0
		db 0FFh, 0FFh,	0, 0FFh, 0, 0, 0FFh, 0,	0FFh
		db 0FFh, 0, 0FFh, 0, 0, 0FFh, 0, 0FFh,	0FFh
		db 0, 0FFh, 0
		db    0 
		db 0FFh 
		db    0   
		db 0FFh
	
Table_adsr:	db 0Ah, 7, 0FFh
		db 2, 9,0FFh 
		db 2, 6, 0FFh
		db 1, 8, 1
		db 2,0Ah,0FFh
		db 1, 0, 0FFh
		db 2,9,1
		db 0Ah,0,0FFh
		db 4,0,0FFh
		db 2,2,0FFh
		db 0Fh,0,0FFh
		db 3,0Bh,0FFh
		db 16h,0,0FFh
		db 3,0Dh,0FFh
		db 1,5, 0FFh
		db 5,7,0FCh
	
Table_vib:	db 1, 0F6h, 0FFh, 1, 28h, 0, 1, 5, 0, 1; ...
		db 0FBh, 0FFh,	1, 0FEh, 0FFh, 2, 0FFh,	0FFh
		db 2, 3, 0, 3,	2, 0, 3, 0FEh, 0FFh, 1,	32h
		db 0, 1, 0B5h,	0FFh, 1, 0B0h, 0FFh, 1,	14h
		db 0, 1, 3, 0,	1, 0FDh, 0FFh, 0, 0, 0

Table_effect:	db 1, 2, 55h, 4 
		db 8Eh, 2, 40h, 1
		db 8Ah, 2,90h, 1
		db 9Ah, 3, 0F4h, 1
		db 82h, 1, 0AAh,0
		db 8Ah, 4, 58h, 2
		db 8Ah, 4, 8Ah, 2
		db 8Ah,52h, 2Ch, 1
		db 8Eh, 2, 40h, 1
		db 8Eh, 2,40h, 1
		db 9Ah, 1, 0FAh, 0
		db 9Ah, 1, 25h, 0
		db 8Ah, 4, 0F0h, 0	
		db 9Ah, 0FCh, 10h,0FFh
		db 9Ah, 0C8h, 0, 0FFh	
		db 9Ah, 81h, 80h, 0FDh
		db 8Eh, 60h, 1, 0
		db 8Eh, 6, 0, 2
		db 8Eh, 0FFh,1, 0
		db 9Ah, 0Ch, 0E8h, 3
		db 9Eh, 3, 0F4h,1 
		db 8Eh, 3, 20h, 3
		db 0,0,0,0
		db 0,0,0,0
		db 0,0,0,0
		db 0,0,0,0
		db 0,0,0,0
		db 0,0,0,0
		db 0,0,0,0
		db 0,0,0,0
		db 0,0,0,0
		db 0,0,0,0
	
Table_fmins:	db 51h, 61h, 17h, 7, 50h, 0F1h, 5Ch, 0DCh; ...
		db 0, 21h, 1, 7, 0DAh,	0F6h, 5Fh, 0FFh
		db 62h, 41h, 40h, 7, 0FAh, 0F6h, 0AFh,	0FFh
		db 44h, 71h, 87h, 27h,	0FAh, 0F5h, 2Ah
		db 0FDh, 47h, 62h, 24h, 8, 0F3h, 0F3h,	0F9h
		db 0FCh, 63h, 62h, 15h, 0Fh, 0F0h, 0F3h
		db 0FFh, 0FFh,	67h, 60h, 14h, 27h, 0F7h
		db 0F2h, 8Ah, 0FAh, 61h, 62h, 0Eh, 6, 0F5h
		db 0F2h, 0Fh, 0FEh, 12h, 12h, 1Bh, 7, 50h
		db 40h, 10h, 1Ah, 60h,	61h, 8Ah, 20h, 0F3h
		db 0F1h, 3Ah, 0FAh, 43h, 40h, 92h, 7, 0F4h
		db 0F2h, 0AAh,	0AAh, 66h, 64h,	15h, 6,	50h
		db 83h, 0FAh, 0FAh, 0E1h, 62h,	5, 3, 0D0h
		db 72h, 2Bh, 0FBh, 61h, 65h, 40h, 4, 0F4h
		db 0F4h, 1Ah, 0ACh, 61h, 65h, 40h, 4, 0F4h
		db 0F6h, 1Ah, 7Dh, 60h, 61h, 89h, 20h,	0F2h
		db 0F1h, 5Ah, 1Ah, 43h, 61h, 41h, 0, 0F9h
		db 0F6h, 0FCh,	0FCh, 60h, 61h,	1Dh, 7,	0F1h
		db 71h, 1Ah, 0DAh, 6Ch, 22h, 59h, 6, 0E3h
		db 0D2h, 23h, 4Dh, 62h, 62h, 3Fh, 7, 96h
		db 76h, 2Ah, 0Ah, 61h,	61h, 5Ch, 0, 56h
		db 44h, 1Dh, 0Ch, 61h,	62h, 63h, 26h, 36h
		db 54h, 1Dh, 0Ch, 41h,	41h, 21h, 27h, 0F7h
		db 50h, 3, 0E5h, 25h, 60h, 1Eh, 15h, 0FAh
		db 0F3h, 0FCh,	87h, 0,	17h, 18h, 0, 8Fh
		db 9Fh, 8Fh, 1Fh, 6Ah,	61h, 14h, 7Fh, 0F7h
		db 0F2h, 0FFh,	0CFh, 18h, 41h,	5Ch, 5Fh
		db 0F1h, 0F0h,	0FFh, 0FFh, 4, 2, 14h, 8Fh
		db 71h, 0F1h, 0FFh, 0FFh, 6, 1, 1Fh, 2Fh
		db 0F4h, 0F3h,	0FFh, 0FFh, 7Ch, 61h, 1Ch
		db 5Fh, 0F6h, 0F2h, 8Fh, 2Fh, 0Ch, 2, 51h
		db 1Fh, 0F1h, 0F2h, 0FFh, 0FFh, 2, 42h
		db 56h, 26h, 0F1h, 0F0h, 0FFh,	0FFh
		db 0CDh, 0CAh,	80h, 4,	0CDh, 0CAh, 80h
		db 4, 0CDh, 0CAh, 80h,	0C9h, 0Eh, 9, 6
		db 30h, 3Eh, 0Fh, 0CDh, 0D8h, 80h, 4, 0Dh
		db 20h, 0F7h, 0C3h, 31h, 81h, 21h, 0DBh
		db 8Ah, 3Ah


Tlast_FM:	db 50h		; Time of last FM access	
Table_SFX:	dw 0
TableMinc:	dw 0		
	
		

		
