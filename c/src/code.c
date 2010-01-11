#include <stdio.h>
#include <assert.h>
#include "error.h"
#include "tokens.h"
#include "file.h"
#include "symbol.h"
#include "lex.h"


#define MAXCODE 4000
#define MAXCANAL 2000



static unsigned char machine[MAXCODE];
static int codesp;

struct
{
  unsigned char data[MAXCANAL];
  unsigned short cont;
}channels[12];




int initmachine()
{
  static int i;
  for(i=0;i<10;i++)
    channels[i].cont=0;

  codesp=0;
}




int inscode (unsigned char code)
{
  if(codesp==MAXCODE)
    rerror(EMEMORY,0,0);

#ifdef _DEBUG_
  fprintf(stderr,"Insertando codigo: %d\n",code);
#endif
  machine[codesp++]=code;
}




char checkpar (symbol sym,symbol ant,char chip)
{
  unsigned char val;
  if (!ant)
    return 0;

  val=ant->val.val;
  switch(sym->type)
    {
    case PLAY:
      if(val >= 0x24)
        rerror(E_INV_PAR,PLAY,0);
      break;
    case BAT:
      if(chip > 2)
        rerror(E_INV_PAR,BAT,chip+1);
      if (val >= 0x20)
        rerror(E_INV_PAR,BAT,0);
      break;
    case OCT:
      if(val >= 0x08)
        rerror(E_INV_PAR,OCT,0);
      break;
    case VOL:
      if(val >= 0x10)
        rerror(E_INV_PAR,VOL,0);
      break;
    case INST:
      if (chip < 3)
        rerror(E_INV_PAR,INST,chip+1);
      if  (val >= 0x20)
        rerror(E_INV_PAR,INST,0);
      sym->val.val=BAT;
      break;
    case LOOP:
      if (val >= 0x10)
        rerror(E_INV_PAR,LOOP,0);
      break;
    case TIMES:
      if(val >= 0x5B)
        rerror(E_INV_PAR,TIMES,0);
      break;

    case TIMEP:
      if(val >= 0x5B)
        sym->val.val=TIMEL;
      else
        sym->val.val=TIMES;
      break;
    case INSTL:                     /*Falta comprobacion superior de rango*/
      if (chip<3)
        rerror(E_INV_PAR,INSTL,chip+1);
      if( val>0x20)
        rerror(E_INV_PAR,INSTL,0);
      sym->val.val=BATL;
      break;
    case AMPL:                      /*Falta comprobacion superior de rango*/
      if (chip>2)
        rerror(E_INV_PAR,AMPL,1);
      break;
    case VIB:
      if (val>=0x10)
        rerror(E_INV_PAR,VIB,0);
      break;
    case DIVSUS:
      if (val>=0x08)
        rerror(E_INV_PAR,DIVSUS,0);
      break;

    case BATL:                      /*Falta comprobacion superior de rango*/
      if(chip>2)
        rerror(E_INV_PAR,BATL,chip+1);
      break;
    }
  return 0;
}





int errorsem(char nerror,char comm,char chip,int nline,char * file)
{
  char * strerror;
  struct wordr * ptr;


  for(ptr=wordrs;ptr->name;ptr++)
    if(ptr->val==comm)
      break;


  if(chip)
    strerror="%s:%d: Bad command %s for channel %s\n";
  else
    strerror="%s:%d: Bad value for %s command\n";

  fprintf(stderr,strerror,file,nline,ptr->name,chip>3?"FM":"PSG");
  return 0;
}







char inscodeI (symbol sym, symbol ant, char chip)
{
  LOG("inscodeI %p", sym);
  LOG(" %p", gsymsig(sym));
  LOG(" %s", gsymlex(sym));
  LOG(" %d\n", gsymval(sym));

  if (gsymtype(sym) == NUMBER && gsymtype(gsymsig(sym)) == EQ) {
      ssymval(gsymsig(gsymsig(sym)), gsymval(sym)); /* This is a assign */
      return 1;
  }

  if (gsymtype(sym) == IDEN)  /* This is the case the ID of the command name */
    return 1;

  if (!gsymsig(sym)) {
    checkpar(sym, ant, chip);
    inscode(gsymval(sym));
    return 0;
  }

  if(!inscodeI(gsymsig(sym), sym, chip)) {
    inscode(gsymval(sym));
    return 0;
  }

  return 1;
}





int execute ()
{
  register char * pc;
  register char * ptr;
  static char canal;
  static unsigned short cont;


  cont=0;
  for(pc=machine;;cont++)
    {
#ifdef _DEBUG_
      fprintf(stderr,"Codificando codigo: %x\n",((unsigned char)(*pc))&0xFF);
#endif
      switch(*pc++)
        {
        case PLAY:   *ptr++=0x01+*pc++;break;
        case DECOC:  *ptr++=0x26;break;
        case INCOC:  *ptr++=0x27;break;
        case SIL:    *ptr++=0x25;break;
        case OCT:    *ptr++=0x28+*pc++;break;
        case DECV:   *ptr++=0x30;break;
        case INCV:   *ptr++=0x31;break;
        case VOL:    *ptr++=0x32+*pc++;break;
        case LOOP:   *ptr++=0x62+*pc++;break;
        case ENDL:   *ptr++=0x72;break;
        case TIMES:  *ptr++=0x97+*pc++;break;
        case BAT:     *ptr++=0x42+*pc++;break;
        case AMPL:    *ptr++=0x73+*pc++;break;
        case VIB:     *ptr++=0x83+*pc++;break;
        case DIVSUS:  *ptr++=0xF2+*pc++;break;
        case FREC:    *ptr++=0xFE;*ptr++=*pc++;cont++;break;
        case TIMEL:   *ptr++=0xFA;*ptr++=*pc++;cont++;break;
        case LOOPL:   *ptr++=0xFB;*ptr++=*pc++;cont++;break;
        case BATL:    *ptr++=0xFC;*ptr++=*pc++;cont++;break;
        case CHANNEL: canal=*pc++;ptr=channels[canal].data;break;
        case ENDC:    *ptr++=0x00;channels[canal].cont=cont;codesp=0;return 1;
        case REGFM:   *ptr++=0xFD;*ptr++=*pc++;*ptr=*pc++;cont+=2; break;
        case PAR:
          {
            *ptr++=0xFF;
            cont++;
            switch(*pc++)
              {
              case ENVLOFF: *ptr++=0x00;break;
              case ENVLON:  *ptr++=0x01;break;
              case SUSON:   *ptr++=0x02;break;
              case SUSOFF:  *ptr++=0x03;break;
              case HFNUM:   *ptr++=0x04;break;
              case LFNUM:   *ptr++=0x05;break;
              case NOISE:   *ptr++=0x06;break;
              case TONE:    *ptr++=0x07;break;
              case DECO:    *ptr++=0x08;*ptr++=*pc++;cont++;break;
              case TAMP:    *ptr++=0x09;*ptr++=*pc++;cont++;break;
              case RITMON:  *ptr++=0x10;break;
              case MELON:   *ptr++=0x11;break;
              }
          }
        }
    }
}






char writesong ()
{
  static char i,*ptr;
  static unsigned short cont,top,j;

  writebyte(stdout,0x0C);
  writebyte(stdout,0x00);

  cont=2;
  for(i=0;i<12;i++)
    {
      cont+=2;
      top=channels[i].cont;
      if(!top)
        {
          top++;
          writeword(stdout,cont+top);
          writebyte(stdout,0);
          cont++;
        }
      else
        {
          writeword(stdout,cont+top);
          for(j=0,ptr=channels[i].data;j<top;j++)
            writebyte(stdout,*ptr++);

          cont+=top;
        }
    }
  writeword(stdout,0);
}





