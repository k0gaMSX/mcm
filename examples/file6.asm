Hz=60			;Frecuencia de 60 Hz
tempo=120		;120 negras por segundo

channel fm1
     loop 0
     oct 0 
     vol 15
     inst original	;Aqui le decimes que coloque el instrumento soft
     inst 16		;ESTE ES EL INSTRUMENTO QUE SE PRUEBA
     
     time negra
     loop 8
     play do,re,mi,fa,sol,la,si
     incoc		;incrementa la octava
     endl
     endc
     
write

