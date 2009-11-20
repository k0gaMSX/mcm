
Hz=60

channel psg2
	
	loop 0	

	vol 0
	adsr 3
	times 8
	play do 
	
        vol 8
        times 12
        adsr 8                  
	play do
	vol 5
	adsr 5
	times 5
	play do

;	noise 0
;        vol 9:adsr 8                  
;        noise 9                   
;        times 6                 
;        noise 9                  
;        noise 9                   
        times 30
	rest
;        endl                                                                         
        endc

	
write

	
