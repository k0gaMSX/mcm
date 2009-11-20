#ifndef __LEXH__
#define __LEXH_


#ifdef __LEXC__
#define EXTERN
#else
#define EXTERN extern
#endif

#include "tokens.h"
#include "symbol.h"




struct wordr
  {  
  char * name;
  int val;
  };



EXTERN struct wordr wordrs []

#ifdef __LEXC__		   
		= {"endc",ENDC,
		   "play",PLAY,
		   "rest",SIL,
		   "decoc",DECOC,
		   "incoc",INCOC,
		   "oct",OCT,
		   "decv",DECV,
		   "incv",INCV,
		   "vol",VOL,
		   "noise",BAT,
		   "inst",INST,
		   "loop",LOOP,
		   "endl",ENDL,
		   "adsr",AMPL,
		   "vib",VIB,
		   "timel",TIMEL,
		   "times",TIMES,
		   "time",TIMEP,
		   "divsus",DIVSUS,
		   "loopl",LOOPL,
		   "druml",BATL,
		   "instl",INSTL,
		   "regfm",REGFM,
		   "frec",FREC,
		   "par",PAR,
		   "chnoff",ENVLOFF,
		   "chnon",ENVLON,
		   "suson",SUSON,
		   "susoff",SUSOFF,
		   "sd_hh",HFNUM,
		   "tom_tcy",LFNUM,
		   "noise",NOISE,
		   "noiseoff",TONE,
		   "deco",DECO,
		   "tamp",TAMP,
		   "ritmon",RITMON,
		   "melon",MELON,
		   "channel",CHANNEL,
		   "var", VAR,
		   "write",WRITE,		   
		   0,0}
#endif
;





EXTERN char lexcad[CADSIZE];
static FILE * oldin;




int gettoken (FILE * in);
int flushin ();
int lexerror(char * file,int line,char car);

#undef EXTERN
#endif
