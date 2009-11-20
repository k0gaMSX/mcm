#include <stdio.h>
#include <math.h>
#include <string.h>
#include "lex.h"
#include "tokens.h"
#include "error.h"
#include "symbol.h"
#include "expr.h"
#include "code.h"



#ifdef HI_TECH_C
#define SIGNED 
#else
#define SIGNED signed
#endif


#define EIDEN_NAME   0
#define E_VAR_DECL   1
#define E_CHANNEL    2
#define E_UNK_COMM   3
#define E_BAD_COMM   4
#define E_EXP        5
#define E_PAR        6
#define E_ASIG       7
#define E_BEMOL      8
#define E_SOS        9
#define E_POINT     10
#define E_INI       11
#define E_BAD_CH    12
#define E_DECL_CH   13


static char status=0;
static char recst=0;
static unsigned char pointp;
static unsigned char point;
static symbol syms1;
static symbol syms2;
static symbol syms3;
static symbol tempo;
static symbol hz;




int getstatus()
  {
  return status;
  }



int initsyn()
  {
    syms1=newsymbol("");
    syms2=newsymbol("");
    syms3=newsymbol("");
    tempo=searchsym("tempo");
    hz=searchsym("Hz");
  }




int syntaxan (int token,symbol * sym)
  {      
    static unsigned char val;
    static unsigned char channels[12]={0,0,0,0,0,0,0,0,0,0,0,0};
  
    switch (status)
      {
      case 0 :
        recst=0;
	switch (token)
	  {
	  case VAR : status = 1; return 0;
	  case CHANNEL : status = 3; return 0;
          case IDEN : status=14;{(*sym=searchsym(lexcad))? 0:rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  case LN: return 1;
	  default: rerror(ESYNTAX,E_INI,0);
	  }  
       
      case 1 :
	switch (token)
	  {
	  case IDEN : status=2; {*sym=newsymbol(lexcad);} return 0;
	  default: rerror(ESYNTAX,E_VAR_DECL,0);
	  }
	      
      case 2 :
	switch (token)
	  {
	  case LN : status=0;{insvar(*sym);};return 1;
	  case COMA : status=2; {insvar(*sym);};return 0;
	  case IDEN :status=2;{*sym=newsymbol(lexcad);} return 0; 
	  default: rerror(ESYNTAX,E_VAR_DECL,0);
	  }

      case 3:
	switch (token)
	  {
	  case NUMBER : status = 4;
	    {
	    val=atoi(lexcad)-1;
	    if(val>=12)
	      rerror(ESYNTAX,E_BAD_CH,0);
	    else
	      {
		ssymval(syms1,val);
		*sym=syms1;
	      }

	    if(channels[val])
	      rerror(ESYNTAX,E_DECL_CH,0);

	    channels[val]=1;
	    }return 0;

	  case IDEN : status = 4;
	    {symbol ptr;
	    if(!(*sym=searchsym(lexcad)))
	      rerror(ESYNTAX,EIDEN_NAME,0);	    
	    else if((val=gsymval(*sym)) >=12)
	      rerror(ESYNTAX,E_BAD_CH,0);
	    
	    if(channels[val])
	      rerror(ESYNTAX,E_DECL_CH,0);

	    channels[val]=1;
	    }return 0;

	  default: rerror(ESYNTAX,E_CHANNEL,0);
	  }
  
      case 4:
	switch (token)
	  {
	  case LN : status=5; {inscode2(CHANNEL,gsymval(*sym));}return 1;
	  default: rerror(ESYNTAX,E_CHANNEL,0);
	  }
      case 5:
	recst=5;
	switch (token)
	  {
	  case LN: return 1;
	  case ENDC :  status=12;{initsym(syms1,token,token);*sym=syms1;} return 0;
	  case PLAY :  status=11;{initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case SIL :   status=6; {initsym(syms1,token,token);*sym=syms1;}return 0;
	  case DECOC : status=6; {initsym(syms1,token,token);*sym=syms1;}return 0;
	  case INCOC : status=6; {initsym(syms1,token,token);*sym=syms1;}return 0;
	  case OCT :   status=7; {initexpr();initsym(syms1,token,token);*sym=syms1;};return 0;
	  case DECV :  status=6; {initsym(syms1,token,token);*sym=syms1;}return 0;
	  case INCV :  status=6; {initsym(syms1,token,token);*sym=syms1;}return 0;
	  case VOL :   status=7; {initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case BAT :   status=7; {initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case INST :  status=7; {initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case LOOP :  status=7; {initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case ENDL :  status=6; {initsym(syms1,token,token);*sym=syms1;}return 0;
	  case AMPL :  status=7; {initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case VIB :   status=7; {initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case TIMEP : status=39;
	    {initexpr();initsym(syms1,token,token);
	     initsym(syms2,NUMBER,0);ssymsig(syms2,syms1);*sym=syms2;}  return 0;
	  case TIMES : status=7; {initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case TIMEL :  status=7;{initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case DIVSUS : status=7;{initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case BATL :  status=7; {initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case INSTL : status=7; {initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case REGFM : status=8; {initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case FREC :  status=7; {initexpr();initsym(syms1,token,token);*sym=syms1;}return 0;
	  case PAR :   status=9; {initsym(syms1,token,token);*sym=syms1;}return 0;
	  case IDEN : status = 10;{(*sym=searchsym(lexcad))? 0:rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  default: rerror(ESYNTAX,E_UNK_COMM,0);
	  }
      case 6:
	switch (token)
	  {
	  case LN: status=5;{inscodeI(*sym,NULL,val);}return 1;
	  default: rerror(ESYNTAX,E_BAD_COMM,0);
	  }
      case 7:
	switch (token)
	  {
	  case NUMBER : status=16; {initsym(syms2,NUMBER,0);ssymsig(syms2,*sym);*sym=syms2;pushexp(NUMBER,atoi(lexcad));}   return 0; 
	  case IDEN :  status=16;
	    {symbol ptr;
	    if(!(ptr=searchsym(lexcad)))	      
	      rerror(ESYNTAX,EIDEN_NAME,0);
	    initsym(syms2,NUMBER,0);
	    ssymsig(syms2,*sym);
	    *sym=syms2;
	    pushexp(NUMBER,gsymval(ptr));
	    } return 0;
	  case PARI :  status=17;{ssymsig(syms2,*sym);*sym=syms2;pushexp(token,0);};return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }
      case 8:
	switch (token)
	  {
	  case NUMBER : status=21;{initsym(syms3,NUMBER,0);ssymsig(syms3,*sym);*sym=syms3;pushexp(NUMBER,atoi(lexcad));} return 0;
          case IDEN : status=21; 
	    {symbol ptr;
	    if(!(ptr=searchsym(lexcad)))	      
	      rerror(ESYNTAX,EIDEN_NAME,0);
	    initsym(syms3,NUMBER,0);
	    ssymsig(syms3,*sym);
	    *sym=syms3;
	    pushexp(NUMBER,gsymval(ptr));
	    ;}return 0;
	  case PARI : status=22;{ssymsig(syms3,*sym);*sym=syms3;pushexp(token,0);}; return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }
      case 9:
	switch (token)
	  {
	  case ENVLOFF : status=6;{initsym(syms2,token,token);ssymsig(syms2,*sym);*sym=syms2;}return 0;
	  case ENVLON :  status=6;{initsym(syms2,token,token);ssymsig(syms2,*sym);*sym=syms2;}return 0;
	  case SUSON :   status=6;{initsym(syms2,token,token);ssymsig(syms2,*sym);*sym=syms2;}return 0;
	  case HFNUM :   status=6;{initsym(syms2,token,token);ssymsig(syms2,*sym);*sym=syms2;}return 0;
	  case LFNUM :   status=6;{initsym(syms2,token,token);ssymsig(syms2,*sym);*sym=syms2;}return 0;
	  case NOISE :   status=6;{initsym(syms2,token,token);ssymsig(syms2,*sym);*sym=syms2;}return 0;
	  case TONE :    status=6;{initsym(syms2,token,token);ssymsig(syms2,*sym);*sym=syms2;}return 0;
	  case DECO :    status=7;{initexpr();initsym(syms3,token,token);ssymsig(syms3,*sym);*sym=syms3;}return 0;
	  case TAMP :    status=7;{initexpr();initsym(syms3,token,token);ssymsig(syms3,*sym);*sym=syms3;}return 0;
	  case RITMON :  status=6;{initsym(syms2,token,token);ssymsig(syms2,*sym);*sym=syms2;}return 0;
	  case MELON :   status=6;{initsym(syms2,token,token);ssymsig(syms2,*sym);*sym=syms2;}return 0;
	  default: rerror(ESYNTAX,E_PAR,0);
	  }
      case 10:
	switch (token)
	  {
	  case EQ: status=7;{initexpr();} return 0;
	  default: rerror(ESYNTAX,E_ASIG,0);
	  }

      case 11:	    
	switch(token)
	  {
	  case NUMBER : status=31; {initsym(syms2,NUMBER,0);ssymsig(syms2,*sym);*sym=syms2;pushexp(NUMBER,atoi(lexcad));}   return 0; 
	  case IDEN :  status=31;
	    {symbol ptr;
	    if(!(ptr=searchsym(lexcad)))	      
	      rerror(ESYNTAX,EIDEN_NAME,0);
	    initsym(syms2,NUMBER,0);
	    ssymsig(syms2,*sym);
	    *sym=syms2;
	    pushexp(NUMBER,gsymval(ptr));
	    } return 0;
	  case PARI :  status=31;{ssymsig(syms2,*sym);*sym=syms2;pushexp(token,0);};return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }	

      case 12: 
	switch(token)
	  {
	  case LN: status=13;{inscode(gsymval(*sym));execute();} return 1;
	  default: rerror(ESYNTAX,E_BAD_COMM,0);
	  }


      case 13:
        recst=13;
	switch(token)
	  {	    
	  case WRITE :   status=38;return 0;
	  case CHANNEL : status=3;return 0;
	  case LN: return 1;
	  default: rerror(ESYNTAX,E_INI,0);
	  }



      case 14:
	switch(token)
	  {
	  case EQ: status=15;{initexpr();}return 0;
	  default: rerror(ESYNTAX,E_ASIG,0);
	  }

      case 15:
	switch(token)
	  {
	  case NUMBER : status=26;{pushexp(NUMBER,atoi(lexcad));};return 0; 
	  case IDEN :  status=26;{symbol ptr;(ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  case PARI :  status=27;{pushexp(token,0);}return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }


/*Expresion de comandos de 1 sola expresion*/

      case 16 :
	switch(token)
	  {
	  case ADD :status=20;{pushexp(token,0);} return 0;
	  case SUB :status=20;{pushexp(token,0);} return 0;
	  case MUL :status=20;{pushexp(token,0);} return 0;
	  case DIV :status=20;{pushexp(token,0);} return 0;
	  case LN : status=5; {pushexp(token,0);ssymval(*sym,evalexpr());inscodeI(*sym,NULL,val);}  return 1;
 	  default: rerror(ESYNTAX,E_EXP,0);  
	  }
      case 17 :
	switch(token)
	  {
	  case NUMBER : status=18;{pushexp(NUMBER,atoi(lexcad));}return 0; 
	  case IDEN :  status=18;{symbol ptr;(ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  case PARI :  status=17;{pushexp(token,0);}return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }
	   
      case 18 :
	switch(token)
	  {
          case PARD : status=18;{pushexp(token,0);}  return 0;
	  case ADD : status=19; {pushexp(token,0);}  return 0;
	  case SUB : status=19; {pushexp(token,0);} return 0;
	  case MUL : status=19; {pushexp(token,0);}return 0;
	  case DIV : status=17; {pushexp(token,0);}  return 0;
          case LN : status=5;   {pushexp(token,0);ssymval(*sym,evalexpr());inscodeI(*sym,NULL,val);}return 1;	
    	  default: rerror(ESYNTAX,E_EXP,0);
	  }	  

      case 19 :
	switch(token)
	  {
	  case NUMBER : status=18;{pushexp(NUMBER,atoi(lexcad));}return 0; 
	  case IDEN :  status=18; {symbol ptr;(ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  case PARI :  status=19; {pushexp(token,0);} return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }

      case 20 :   
	switch(token)	  
	  {
	  case NUMBER : status=18;{pushexp(NUMBER,atoi(lexcad));}return 0; 
	  case IDEN :  status=18; {symbol ptr;(ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);} return 0;
	  case PARI :  status=19; {pushexp(token,0);} return 0;
	  case PARD :  status=18; {pushexp(token,0);} return 0;
    	  default: rerror(ESYNTAX,E_EXP,0);
	  }

/*Expresion para instruccion con dos expresiones*/
	
      case 21 :
	switch(token)
	  {
	  case ADD :status=22;{pushexp(token,0);}return 0;
	  case SUB :status=22;{pushexp(token,0);}return 0;
	  case MUL :status=22;{pushexp(token,0);}return 0;
	  case DIV :status=22;{pushexp(token,0);}return 0;
	  case IDEN : status=16;
	    {
	    symbol ptr;
	    pushexp(LN,0);ssymval(*sym,evalexpr());initexpr();
	    (ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);
	    initsym(syms2,NUMBER,0);ssymsig(syms2,*sym);*sym=syms2;
	    }return 0;
	  case NUMBER :status=16;  
	    {
	    pushexp(LN,0);ssymval(*sym,evalexpr());initexpr();
	    pushexp(NUMBER,atoi(lexcad));
	    initsym(syms2,NUMBER,0);ssymsig(syms2,*sym);*sym=syms2;
	    }return 0;
	  case PARI: status=16;
	    {
	    pushexp(LN,0);ssymval(*sym,evalexpr());initexpr();
	    pushexp(token,0);
	    initsym(syms2,NUMBER,0);ssymsig(syms2,*sym);*sym=syms2;
	    }return 0;
 	  default: rerror(ESYNTAX,E_EXP,0);  
	  }
      case 22 :
	switch(token)
	  {
	  case NUMBER : status=23;{pushexp(NUMBER,atoi(lexcad));}return 0; 
	  case IDEN :  status=23;{symbol ptr;(ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  case PARI :  status=22;{pushexp(token,0);}return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }
	   
      case 23 :
	switch(token)
	  {
          case PARD : status=25;{pushexp(token,0);}return 0;
	  case ADD : status=24;{pushexp(token,0);}return 0;
	  case SUB : status=24;{pushexp(token,0);}return 0;
	  case MUL : status=24;{pushexp(token,0);}return 0;
	  case DIV : status=24;{pushexp(token,0);}return 0;
    	  default: rerror(ESYNTAX,E_EXP,0);
	  }	  

      case 24 :
	switch(token)
	  {
	  case NUMBER : status=23;{pushexp(NUMBER,atoi(lexcad));}return 0; 
	  case IDEN :  status=23;{symbol ptr;(ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  case PARI :  status=22;{pushexp(token,0);}return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }
      case 25:
	switch (token)
	  {
          case PARD : status=25;{pushexp(token,0);}return 0;
	  case ADD : status=24;{pushexp(token,0);}return 0;
	  case SUB : status=24;{pushexp(token,0);}return 0;
	  case MUL : status=24;{pushexp(token,0);}return 0;
	  case DIV : status=24;{pushexp(token,0);}return 0;
	  case IDEN : status=16;
	    {
	    symbol ptr;
	    pushexp(LN,0);ssymval(*sym,evalexpr());initexpr();
	    (ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);
	    initsym(syms2,NUMBER,0);ssymsig(syms2,*sym);*sym=syms2;
	    }return 0;
	  case NUMBER :status=16;  
	    {
	    pushexp(LN,0);ssymval(*sym,evalexpr());initexpr();
	    pushexp(NUMBER,atoi(lexcad));
	    initsym(syms2,NUMBER,0);ssymsig(syms2,*sym);*sym=syms2;
	    }return 0;
	  case PARI: status=16;
	    {
	    pushexp(LN,0);ssymval(*sym,evalexpr());initexpr();
	    pushexp(token,0);
	    initsym(syms2,NUMBER,0);ssymsig(syms2,*sym);*sym=syms2;
	    }return 0;

	  default: rerror(ESYNTAX,E_EXP,0);
          }





/*Expresion de asignacion inicial*/


      case 26 :
	switch(token)
	  {
	  case ADD :status=15;{pushexp(token,0);}return 0;
	  case SUB :status=15;{pushexp(token,0);}return 0;
	  case MUL :status=15;{pushexp(token,0);}return 0;
	  case DIV :status=15;{pushexp(token,0);}return 0;
	  case LN : status=0;{pushexp(token,0);ssymval(*sym,evalexpr());}  return 1;
 	  default: rerror(ESYNTAX,E_EXP,0);  
	  }
      case 27 :
	switch(token)
	  {
	  case NUMBER : status=28;{pushexp(NUMBER,atoi(lexcad));}return 0; 
	  case IDEN :  status=28;{symbol ptr;(ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  case PARI :  status=27;{pushexp(token,0);}return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }
	   
      case 28 :
	switch(token)
	  {
          case PARD : status=28;{pushexp(token,0);}return 0;
	  case ADD :  status=27;{pushexp(token,0);}return 0;
	  case SUB :  status=27;{pushexp(token,0);}return 0;
	  case MUL :  status=27;{pushexp(token,0);}return 0;
	  case DIV :  status=17;{pushexp(token,0);}return 0;
          case LN :   status=0;{pushexp(token,0);ssymval(*sym,evalexpr());}return 1;	  
    	  default: rerror(ESYNTAX,E_EXP,0);
	  }	  


      case 30 :
	  

/*Expresion de play*/

      case 31 :
	switch(token)
	  {
	  case ADD : status=32;{pushexp(token,0);}return 0;
	  case SUB : status=32;{pushexp(token,0);}return 0;
	  case MUL : status=32;{pushexp(token,0);}return 0;
	  case DIV : status=32;{pushexp(token,0);}return 0;
	  case SOS : status=36;return 0;
	  case BEMOL : status=36;return 0;
	  case COMA : status=32;{pushexp(LN,0);ssymval(*sym,evalexpr());inscodeI(*sym,NULL,val);initexpr();} return 0;
	  case LN : status=5;{pushexp(token,0);ssymval(*sym,evalexpr());inscodeI(*sym,NULL,val);} return 1;
 	  default: rerror(ESYNTAX,E_EXP,0);  
	  }
      case 32 :
	switch(token)
	  {
	  case NUMBER : status=33;{pushexp(NUMBER,atoi(lexcad));}return 0; 
	  case IDEN :  status=33;{symbol ptr;(ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  case PARI :  status=32;{pushexp(token,0);}return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }
	   
      case 33 :
	switch(token)
	  {
          case PARD : status=33;{pushexp(token,0);}return 0;
	  case ADD : status=34;{pushexp(token,0);}return 0;
	  case SUB : status=34;{pushexp(token,0);}return 0;
	  case MUL : status=34;{pushexp(token,0);}return 0;
	  case DIV : status=34;{pushexp(token,0);}return 0;
          case SOS : status=36;{pushexp(token,0);ssymval(*sym,evalexpr());}return 0;
	  case BEMOL : status=35;{pushexp(token,0);ssymval(*sym,evalexpr());}return 0;
	  case COMA : status=32;{pushexp(LN,0);ssymval(*sym,evalexpr());inscodeI(*sym,NULL,val);initexpr();} return 0;
	  case LN : status=5;{pushexp(token,0);ssymval(*sym,evalexpr());inscodeI(*sym,NULL,val);}return 1;
    	  default: rerror(ESYNTAX,E_EXP,0);
	  }	  
      case 34 :
	switch(token)
	  {
	  case NUMBER : status=31;{pushexp(NUMBER,atoi(lexcad));}return 0; 
	  case IDEN :  status=31;{symbol ptr;(ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  case PARI :  status=32;{pushexp(token,0);}return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }

      case 35:
	switch(token)
	  {
	  case BEMOL: status=35;{ssymval(*sym,((SIGNED char)gsymval(*sym))-3);}return 0;
	  case LN : status=5;
	    {SIGNED char val;
	    val=gsymval(*sym);
	    val-=3;
	    if(val<0)
	      val=33-val;
	    
	    ssymval(*sym,val);inscodeI(*sym,NULL,val);}return 1;
	  default: rerror(ESYNTAX,E_BEMOL,0);
	  }
 
      case 36 :
	switch(token)
	  {
	  case SOS: status=36;{ssymval(*sym,gsymval(*sym)+3);}return 0;
	  case LN : status=5;{ssymval(*sym,(gsymval(*sym)+3)%34);inscodeI(*sym,NULL,val);}return 1;
	  case COMA : status=32;{pushexp(LN,0);ssymval(*sym,evalexpr());inscodeI(*sym,NULL,val);initexpr();} return 0;
	  default: rerror(ESYNTAX,E_SOS,0);
	  }


/*      case 37 : 	
	switch(token)
//	  {
//	  case IDEN : status=14;{(*sym=searchsym(lexcad))? 0:rerror(0);}return 0;
//	  case LN : return 1;
//	  case CHANNEL : status=3;return 0;
//	  default: rerror(ESYNTAX);
//	  }

*/

      case 38 :
	switch(token)
	  {
	  case LN : status=37;{errorg?rerror(EERROR,0,0):writesong();}return 2;            /*Fin de entrada*/
	  default: rerror(ESYNTAX,E_BAD_COMM,0);
	  }


      case 39:
	switch (token)
	  {
	  case NUMBER : status=40;{pushexp(NUMBER,atoi(lexcad));}return 0; 
	  case IDEN :  status=40;{symbol ptr;(ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  case PARI :  status=41;{pushexp(token,0);}return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }


      case 40 :
	switch(token)
	  {
	  case ADD : status=39;{pushexp(token,0);}return 0;
	  case SUB : status=39;{pushexp(token,0);}return 0;
	  case MUL : status=39;{pushexp(token,0);}return 0;
	  case DIV : status=39;{pushexp(token,0);}return 0;
          case POINT : status=43;{pointp=point=32;};return 0;
	  case LN : status=5;
	    {
	    float time;

	    pushexp(token,0);
	    time=evalexpr();
            time/=64.0;
	    time*=(240.0*((float)(gsymval(hz))))/((float)(gsymval(tempo)));
	    if((time-((float)((unsigned char)time))) > 0.5)
	      time=((float)((unsigned char)time))+1.0;
	    else
	      time=(float)((unsigned char)time);

	    ssymval(*sym,(unsigned char)time);
	    inscodeI(*sym,NULL,val);
	    }return 1;
	    

 	  default: rerror(ESYNTAX,E_EXP,0);  
	  }

      case 41 :
	switch(token)
	  {
	  case NUMBER : status=42;{pushexp(NUMBER,atoi(lexcad));}return 0; 
	  case IDEN :  status=42;{symbol ptr;(ptr=searchsym(lexcad))?pushexp(NUMBER,gsymval(ptr)):rerror(ESYNTAX,EIDEN_NAME,0);}return 0;
	  case PARI :  status=41;{pushexp(token,0);}return 0;
	  default: rerror(ESYNTAX,E_EXP,0);
	  }
	   
      case 42 :
	switch(token)
	  {
          case PARD : status=42;{pushexp(token,0);}return 0;
	  case ADD : status=41;{pushexp(token,0);}return 0;
	  case SUB : status=41;{pushexp(token,0);}return 0;
	  case MUL : status=41;{pushexp(token,0);}return 0;
	  case DIV : status=41;{pushexp(token,0);}return 0;
	  case POINT : status=43;{pointp=point=32;} return 0;
	  case LN : status=5;
	    {
	    float time,timei;

	    pushexp(token,0);
	    time=evalexpr();
            time/=64.0;
	    time*=(240.0*((float)(gsymval(hz))))/((float)(gsymval(tempo)));
	    time=fabs(time);

	    timei=floor(time);
	    if(time-timei)
	      time=timei+1.0;
	    else
	      time=timei;

	    ssymval(*sym,(unsigned char)time);
	    inscodeI(*sym,NULL,val);
	    }return 1;
    	  default: rerror(ESYNTAX,E_EXP,0);
	  }	  
      case 43:
	switch(token)
	  {
	  case POINT: {pointp>>=1;point+=pointp;};return 0;
	  case LN: status=5;
	    {
	    float time,timei;
	    	    	
      	    pushexp(token,0);
	    time=evalexpr();
            time/=64;
	    time*=(240.0*((float)(gsymval(hz))))/((float)(gsymval(tempo)));
	    time=fabs(time);
	    time+=time*(((float)point)/64.0);

	    
	    timei=floor(time);
	    if(time-timei)
	      time=timei+1.0;
	    else
	      time=timei;


	    ssymval(*sym,(unsigned char)time);
	    inscodeI(*sym,NULL,val);
	    }return 1;
	  }	    
      default: rerror(ESYNTAX,E_POINT,0);
	
      }
  }
   






/*
int syntaxan1 (int  status,int token)
  {
    static struct reglar  ptr;

    ptr=rtable[status]+token;

    if(ptr->status!=-1)
      {
	status=ptr->status;
	return ptr->producc;
      }
    else
	rerror(ESYNTAX);
      
  }

  */







int syntaxerror (char * file,int line,char etp)
  {
    static char * lserrors[]={			      
      "%s:%d: Variable not declarated\n",
      "%s:%d: Bad variable declaration\n",
      "%s:%d: Channel Identificator must be either variable or number\n",      
      "%s:%d: Unknown command\n",
      "%s:%d: Bad command format\n",
      "%s:%d: Expression error\n",
      "%s:%d: Incorrect argument for par command\n",
      "%s:%d: Assignament error\n",
      "%s:%d: Bad @ modifier\n",
      "%s:%d: Bad # modifier\n",
      "%s:%d: Bad . modifier\n",
      "%s:%d: Expected channel or var sentence\n",
      "%s:%d: Number of channel must be on 1-12 range\n",
      "%s:%d: Channel already has been described\n"};

 


    fprintf(stderr,"%s:%d: Syntax error before %s (\\0x%02.2x)\n",file,line,lexcad,((unsigned)(*lexcad))&0xFF);
    if(etp==-1) return;
    fprintf(stderr,lserrors[etp],file,line);    
    status=recst;
  }
