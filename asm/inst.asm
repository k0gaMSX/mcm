	phase 100h	


inicio:
	push af
 	ld a,1
 	call mcdrvc
 	ld a,3
 	call mcdrvc
	
	pop af
	ld hl,cancion
	or a
	jr z,++

	ld hl,8000h
	inc a
	srl a
	jr nc,+
	ld hl,0a000h	
+	out (0feh),a
		
+ 	ld a,9
	call mcdrvc
	
	ret

	
	include "mcdrv2.asm"
cancion: nop	

	
	

	

	
	