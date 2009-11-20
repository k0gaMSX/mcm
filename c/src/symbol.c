#include <stdio.h>
#include <string.h>
#include "symbol.h"
#include "error.h"
#include "tokens.h"



#define MAXSYM 500



static struct symbol symmem[MAXSYM];
static unsigned short symsp=0;


symbol newsymbol (char * lex)
  {   
    symbol sym;

    if(symsp==MAXSYM)
      rerror(EMEMORY,0,0);

    sym=symmem+symsp++;   
    sym->type=IDEN;
    *sym->lex='\0';
    strcat(sym->lex,lex);
      
    return sym;
  }	







symbol vartable[32]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		     NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		     NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
		     NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};







static unsigned char key;
			 
symbol insvar (symbol sym)
  {
  if(searchsym(sym->lex))
    rerror(EMEMORY,0,0);
  
  sym->sig=vartable[key];
  vartable[key]=sym;
  
  return sym;
  }






symbol searchsym (char * name)
  {
    register char * ptr;   
    register unsigned char keytmp;

    keytmp=0;
    for(ptr=name;*ptr;ptr++)
      keytmp+=*ptr;
  
    keytmp&=31;
    key=keytmp;
 
    {
      register symbol ptr2;
      for(ptr2=vartable[keytmp];ptr2;ptr2=ptr2->sig)
	if (!strcmp(ptr2->lex,name))
	  return ptr2;    

      return NULL;
    }
  }
 








