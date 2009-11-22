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


enum
  {
    STATUS_INIT = 0,
    STATUS_BEGIN_VAR_DECL,
    STATUS_NAME_VAR_DECL,
    STATUS_BEGIN_CHN_DECL,
    STATUS_END_OF_CHANNEL,
    STATUS_BEGIN_CHN_BODY,

    STATUS_BEGIN_GLOBAL_ASGN = 100,
    STATUS_BEGIN_ENDC,
    STATUS_END_OF_1CMD,
    STATUS_BEGIN_EXP_2CMD,
    STATUS_EXP2_OPERAND,
    STATUS_EXP2_OPERATOR
  };

int syntaxan (int token,symbol * sym)
{
  static unsigned char val;
  static unsigned char channels[12]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static symbol ptr;

  switch (status)
    {
    case STATUS_INIT:     /* Initial status */
      recst = STATUS_INIT;    /*Save actual state for error handling  */
      switch (token)
        {
        case VAR:            /* Begin variable declaration */
          status = STATUS_BEGIN_VAR_DECL;
          return 0;

        case CHANNEL:        /* Begin channel declaration */
          status = STATUS_BEGIN_CHN_DECL;
          return 0;

        case IDEN:            /* Begin assign statement */
          status = STATUS_BEGIN_GLOBAL_ASGN;
          if ((*sym = searchsym(lexcad)) == NULL)
            rerror(ESYNTAX, EIDEN_NAME, 0);
          return 0;

        case LN:              /* Empty sentence  */
          return 1;

        default:
          rerror(ESYNTAX, E_INI, 0);
        }

      /* Variable declaration states  */

    case STATUS_BEGIN_VAR_DECL: /* VAR ; variable declaration */
      switch (token)
        {
        case IDEN:            /* Name of the variable */
          status = STATUS_NAME_VAR_DECL;
          *sym = newsymbol(lexcad);
          return 0;

        default:
          rerror(ESYNTAX, E_VAR_DECL, 0);
        }

    case STATUS_NAME_VAR_DECL: /* VAR IDEN ; variable declaration */
      switch (token)
        {
        case LN:             /* end of declaration */
          status = STATUS_INIT;
          insvar(*sym);
          return 1;

        case COMA:            /* multiple declaration */
          status = STATUS_NAME_VAR_DECL;
          insvar(*sym);
          return 0;

        case IDEN:            /* FIXME: This rule must be in other state */
          status = STATUS_NAME_VAR_DECL;
          *sym = newsymbol(lexcad);
          return 0;

        default:
          rerror(ESYNTAX, E_VAR_DECL, 0);
        }

      /* Channel declaration states */

    case STATUS_BEGIN_CHN_DECL: /* CHANNEL ; channel declaration */
      switch (token)
        {
        case NUMBER:         /* Number of the channel */
          status = STATUS_END_OF_CHANNEL;
          val = atoi(lexcad) - 1;
          if(val >= 12)
            rerror(ESYNTAX, E_BAD_CH, 0);
          else
            {
              ssymval(syms1, val);
              *sym = syms1;
            }

          if(channels[val])
            rerror(ESYNTAX, E_DECL_CH, 0);

          channels[val]=1;
          return 0;

        case IDEN:           /* Variable with the name of the channel */
          status = STATUS_END_OF_CHANNEL;
          if(!(*sym = searchsym(lexcad)))
            rerror(ESYNTAX, EIDEN_NAME, 0);
          else if((val = gsymval(*sym)) >=12)
            rerror(ESYNTAX, E_BAD_CH, 0);

          if(channels[val])
            rerror(ESYNTAX, E_DECL_CH, 0);

          channels[val]=1;
          return 0;

        default: rerror(ESYNTAX, E_CHANNEL, 0);
        }

      /* end of channel declaration */
    case STATUS_END_OF_CHANNEL: /* CHANNEL IDEN | NUMBER ; end of channl dcl*/
      switch (token)
        {
        case LN:             /* finish channel dcl */
          status = STATUS_BEGIN_CHN_BODY;
          inscode2(CHANNEL, gsymval(*sym));
          return 1;

        default:
          rerror(ESYNTAX, E_CHANNEL, 0);
        }

      /* Begin channel body */
    case STATUS_BEGIN_CHN_BODY:      /* CHANNEL IDEN | NUMBER LN ; chl body*/
      recst = STATUS_BEGIN_CHN_BODY;   /*Save state for error handling*/
      switch (token)
        {
        case LN:
          return 1;

        case ENDC:           /* end of channel statement */
          status = STATUS_BEGIN_ENDC;
          initsym(syms1, token, token);
          *sym = syms1;
          return 0;

        case PLAY:
          status = 11;          /* play statement */
          initexpr();
          initsym(syms1, token, token);
          *sym = syms1;
          return 0;

        case SIL:               /* silence statement  */
        case DECOC:             /* dec octave statement */
        case INCOC:             /* inc octave statement */
        case ENDL:              /* end of loop statement */
        case DECV:              /* dec volume */
        case INCV:              /* inc volume */
          status = STATUS_END_OF_1CMD;
          initsym(syms1, token, token);
          *sym = syms1;
          return 0;

        case OCT:               /* octave statement */
        case VOL:               /* volume statement */
        case BAT:               /* battery statement */
        case INST:              /* instrument statement */
        case LOOP:              /* loop statement */
        case AMPL:
        case VIB:               /* vibrato statement */
        case TIMES:             /* time short format statement */
        case TIMEL:             /* time long format statement */
        case DIVSUS:
        case BATL:              /* battery long format statement */
        case INSTL:             /* instrument long format statement */
        case FREC:              /* set frequency statement */
          status = STATUS_BEGIN_EXP_2CMD;
          initexpr();
          initsym(syms1, token, token);
          *sym = syms1;
          return 0;

        case TIMEP:
          status = 39;
          initexpr();
          initsym(syms1, token, token);
          initsym(syms2, NUMBER, 0);
          ssymsig(syms2, syms1);
          *sym = syms2;
          return 0;

        case REGFM:             /* write to fm register statement */
          status = 8;
          initexpr();
          initsym(syms1, token, token);
          *sym = syms1;
          return 0;

        case PAR:               /* parameter statement */
          status = 9;
          initsym(syms1, token, token);
          *sym = syms1;
          return 0;

        case IDEN:              /* Begin a assign statement */
          status = 10;
          if ((*sym = searchsym(lexcad)) == NULL)
            rerror(ESYNTAX, EIDEN_NAME, 0);

          return 0;

        default:
          rerror(ESYNTAX, E_UNK_COMM, 0);
        }

      /* End of 1 word statement */
    case STATUS_END_OF_1CMD:
      switch (token)
        {
        case LN:
          status = STATUS_BEGIN_CHN_BODY;
          inscodeI(*sym, NULL, val);
          return 1;

        default:
          rerror(ESYNTAX, E_BAD_COMM, 0);
        }

    case 8:
      switch (token)
        {
        case NUMBER:
          status = 21;
          initsym(syms3, NUMBER, 0);
          ssymsig(syms3,*sym);
          *sym = syms3;
          pushexp(NUMBER, atoi(lexcad));
          return 0;

        case IDEN:
          status = 21;
          if((ptr = searchsym(lexcad)) == NULL)
            rerror(ESYNTAX, EIDEN_NAME, 0);

          initsym(syms3, NUMBER, 0);
          ssymsig(syms3,*sym);
          *sym = syms3;
          pushexp(NUMBER, gsymval(ptr));
          return 0;

        case PARI:
          status = 22;
          ssymsig(syms3,*sym);
          *sym = syms3;
          pushexp(token, 0);
          return 0;

        default:
          rerror(ESYNTAX, E_EXP, 0);
        }
    case 9:
      switch (token)
        {
        case ENVLOFF:
        case ENVLON:
        case SUSON:
        case HFNUM:
        case LFNUM:
        case NOISE:
        case TONE:
          status = STATUS_END_OF_1CMD;
          initsym(syms2, token, token);
          ssymsig(syms2,*sym);
          *sym = syms2;
          return 0;

        case DECO:
          status = STATUS_BEGIN_EXP_2CMD;
          initexpr();
          initsym(syms3, token, token);
          ssymsig(syms3,*sym);
          *sym = syms3;
          return 0;

        case TAMP:
          status = STATUS_BEGIN_EXP_2CMD;
          initexpr();
          initsym(syms3, token, token);
          ssymsig(syms3,*sym);
          *sym = syms3;
          return 0;

        case RITMON:
          status = STATUS_END_OF_1CMD;
          initsym(syms2, token, token);
          ssymsig(syms2,*sym);
          *sym = syms2;
          return 0;

        case MELON:
          status = STATUS_END_OF_1CMD;
          initsym(syms2, token, token);
          ssymsig(syms2,*sym);
          *sym = syms2;
          return 0;

        default:
          rerror(ESYNTAX, E_PAR, 0);
        }
    case 10:
      switch (token)
        {
        case EQ:
          status = STATUS_BEGIN_EXP_2CMD;
          initexpr();
          return 0;

        default:
          rerror(ESYNTAX, E_ASIG, 0);
        }

    case 11:
      switch(token)
        {
        case NUMBER:
          status = 31;
          initsym(syms2, NUMBER, 0);
          ssymsig(syms2,*sym);
          *sym = syms2;
          pushexp(NUMBER, atoi(lexcad));
          return 0;

        case IDEN:
          status = 31;
          if((ptr = searchsym(lexcad)) == NULL)
            rerror(ESYNTAX, EIDEN_NAME, 0);

          initsym(syms2, NUMBER, 0);
          ssymsig(syms2,*sym);
          *sym = syms2;
          pushexp(NUMBER, gsymval(ptr));
          return 0;

        case PARI:
          status = 31;
          ssymsig(syms2,*sym);
          *sym = syms2;
          pushexp(token, 0);
          return 0;

        default:
          rerror(ESYNTAX, E_EXP, 0);
        }

      /* Begin end of channel statement */
    case STATUS_BEGIN_ENDC:   /* ENDC ; endc command */
      switch(token)
        {
        case LN:
          status = 13;
          inscode(gsymval(*sym));
          execute();
          return 1;

        default:
          rerror(ESYNTAX, E_BAD_COMM, 0);
        }


    case 13:
      recst = 13;

      switch(token)
        {
        case WRITE:
          status = 38;
          return 0;

        case CHANNEL:
          status = STATUS_BEGIN_CHN_DECL;
          return 0;

        case LN:
          return 1;

        default:
          rerror(ESYNTAX, E_INI, 0);
        }



    case STATUS_BEGIN_GLOBAL_ASGN:
      switch(token)
        {
        case EQ:
          status = 15;
          initexpr();
          return 0;

        default: rerror(ESYNTAX, E_ASIG, 0);
        }

    case 15:
      switch(token)
        {
        case NUMBER:
          status = 26;
          {pushexp(NUMBER, atoi(lexcad));};
          return 0;
        case IDEN:
          status = 26;
          (ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0);
          return 0;
        case PARI:
          status = 27;
          pushexp(token, 0);
          return 0;
        default: rerror(ESYNTAX, E_EXP, 0);
        }




      /*******************************************************************/


    case STATUS_BEGIN_EXP_2CMD:
      fputs("Iniciando\n", stderr);
      switch (token)
        {
        case PARI:
          status = STATUS_EXP2_OPERAND;
          ssymsig(syms2,*sym);
          *sym = syms2;
          pushexp(token, 0);
          return 0;

        case NUMBER:
          status = STATUS_EXP2_OPERAND;
          initsym(syms2, NUMBER, 0);
          ssymsig(syms2,*sym);
          *sym = syms2;
          pushexp(NUMBER, atoi(lexcad));
          return 0;

        case IDEN:
          status = STATUS_EXP2_OPERAND;
          if(!(ptr = searchsym(lexcad)))
            rerror(ESYNTAX, EIDEN_NAME, 0);

          initsym(syms2, NUMBER, 0);
          ssymsig(syms2,*sym);
          *sym = syms2;
          pushexp(NUMBER, gsymval(ptr));
          return 0;

        default:
          rerror(ESYNTAX, E_EXP, 0);

        }


    case STATUS_EXP2_OPERAND:
      fprintf(stderr, "En operando %d\n", token);
      switch(token)
        {
        case NUMBER:
          status = STATUS_EXP2_OPERAND;
          pushexp(NUMBER, atoi(lexcad));
          return 0;

        case IDEN:
          status = STATUS_EXP2_OPERAND;
          if ((ptr = searchsym(lexcad)) == NULL)
            rerror(ESYNTAX, EIDEN_NAME, 0);

          pushexp(NUMBER, gsymval(ptr));
          return 0;

        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case PARD:
        case PARI:              /* left parenthesis  */
          status = STATUS_EXP2_OPERAND;
          pushexp(token, 0);
          return 0;

        case LN:
          status = STATUS_BEGIN_CHN_BODY;
          pushexp(token, 0);
          ssymval(*sym, evalexpr());
          inscodeI(*sym, NULL, val);
          return 1;

        default:
          rerror(ESYNTAX, E_EXP, 0);
        }



    /* case STATUS_EXP2_OPERATOR: */
    /*   fputs("En operator\n", stderr); */
    /*   switch(token) */
    /*     { */
    /*     case PARI: */
    /*     case PARD: */
    /*     case ADD: */
    /*     case SUB: */
    /*     case MUL: */
    /*     case DIV: */
    /*       status = STATUS_EXP2_OPERAND; */
    /*       pushexp(token, 0); */
    /*       return 0; */
    /*     case LN: */
    /*       status = STATUS_BEGIN_CHN_BODY; */
    /*       pushexp(token, 0); */
    /*       ssymval(*sym, evalexpr()); */
    /*       inscodeI(*sym, NULL, val); */
    /*       return 1; */

    /*     default: */
    /*       rerror(ESYNTAX, E_EXP, 0); */
    /*     } */

    /* case 17: */
    /*   switch(token) */
    /*     { */
    /*     case NUMBER: */
    /*       status = 18; */
    /*       pushexp(NUMBER, atoi(lexcad)); */
    /*       return 0; */
    /*     case IDEN: */
    /*       status = 18; */
    /*       (ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0); */
    /*       return 0; */
    /*     case PARI: */
    /*       status = 17; */
    /*       pushexp(token, 0); */
    /*       return 0; */
    /*     default: rerror(ESYNTAX, E_EXP, 0); */
    /*     } */

      /* case 18: */
      /*   switch(token) */
      /*     { */
      /*     case PARD: */
      /*       status = 18; */
      /*       pushexp(token, 0); */
      /*       return 0; */

      /*     case ADD: */
      /*     case SUB: */
      /*     case MUL: */
      /*       status = 19; */
      /*       pushexp(token, 0); */

      /*       return 0; */

      /*     case DIV: */
      /*       status = 17; */
      /*       pushexp(token, 0); */
      /*       return 0; */

      /*     case LN: */
      /*       status = STATUS_BEGIN_CHN_BODY; */
      /*       pushexp(token, 0); */
      /*       ssymval(*sym, evalexpr()); */
      /*       inscodeI(*sym, NULL, val); */
      /*       return 1; */

      /*     default: rerror(ESYNTAX, E_EXP, 0); */
      /*     } */

    /* case 19: */
    /*   switch(token) */
    /*     { */
    /*     case NUMBER: */
    /*       status = 18; */
    /*       pushexp(NUMBER, atoi(lexcad)); */
    /*       return 0; */

    /*     case IDEN: */
    /*       status = 18; */
    /*       (ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0); */
    /*       return 0; */

    /*     case PARI: */
    /*       status = 19; */
    /*       pushexp(token, 0); */
    /*       return 0; */

    /*     default: rerror(ESYNTAX, E_EXP, 0); */
    /*     } */


      /***********************************************/
      /*Expresion para instruccion con dos expresiones*/

    case 21:
      switch(token)
        {
        case ADD:
        case SUB:
        case MUL:
        case DIV:
          status = 22;
          pushexp(token, 0);
          return 0;

        case IDEN:
          STATUS_EXP2_OPERAND;
          pushexp(LN, 0);
          ssymval(*sym, evalexpr());
          initexpr();
          (ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0);
          initsym(syms2, NUMBER, 0);ssymsig(syms2,*sym);*sym = syms2;
          return 0;

        case NUMBER:
          STATUS_EXP2_OPERAND;
          pushexp(LN, 0);ssymval(*sym, evalexpr());initexpr();
          pushexp(NUMBER, atoi(lexcad));
          initsym(syms2, NUMBER, 0);ssymsig(syms2,*sym);*sym = syms2;
          return 0;
        case PARI:
          STATUS_EXP2_OPERAND;
          pushexp(LN, 0);ssymval(*sym, evalexpr());initexpr();
          pushexp(token, 0);
          initsym(syms2, NUMBER, 0);ssymsig(syms2,*sym);*sym = syms2;
          return 0;

        default: rerror(ESYNTAX, E_EXP, 0);
        }
    case 22:
      switch(token)
        {
        case NUMBER:
          status = 23;{pushexp(NUMBER, atoi(lexcad));}return 0;
        case IDEN:
          status = 23;{symbol ptr;(ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0);}return 0;
        case PARI:
          status = 22;
          pushexp(token, 0);
          return 0;
        default: rerror(ESYNTAX, E_EXP, 0);
        }

    case 23:
      switch(token)
        {
        case PARD:
          status = 25;
          pushexp(token, 0);
          return 0;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
          status = 24;
          pushexp(token, 0);
          return 0;

        default:
          rerror(ESYNTAX, E_EXP, 0);
        }

    case 24:
      switch(token)
        {
        case NUMBER:
          status = 23;{pushexp(NUMBER, atoi(lexcad));}return 0;
        case IDEN:
          status = 23;{symbol ptr;(ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0);}return 0;
        case PARI:
          status = 22;
          pushexp(token, 0);
          return 0;
        default: rerror(ESYNTAX, E_EXP, 0);
        }
    case 25:
      switch (token)
        {
        case PARD:
          status = 25;
          pushexp(token, 0);
          return 0;
        case ADD:
          status = 24;
          pushexp(token, 0);
          return 0;
        case SUB:
          status = 24;
          pushexp(token, 0);
          return 0;
        case MUL:
          status = 24;
          pushexp(token, 0);
          return 0;
        case DIV:
          status = 24;
          pushexp(token, 0);
          return 0;
        case IDEN:
          STATUS_EXP2_OPERAND;
          {
            symbol ptr;
            pushexp(LN, 0);ssymval(*sym, evalexpr());initexpr();
            (ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0);
            initsym(syms2, NUMBER, 0);ssymsig(syms2,*sym);*sym = syms2;
          }return 0;
        case NUMBER:
          STATUS_EXP2_OPERAND;
          {
            pushexp(LN, 0);ssymval(*sym, evalexpr());initexpr();
            pushexp(NUMBER, atoi(lexcad));
            initsym(syms2, NUMBER, 0);ssymsig(syms2,*sym);*sym = syms2;
          }return 0;
        case PARI:
          STATUS_EXP2_OPERAND;
          {
            pushexp(LN, 0);ssymval(*sym, evalexpr());initexpr();
            pushexp(token, 0);
            initsym(syms2, NUMBER, 0);ssymsig(syms2,*sym);*sym = syms2;
          }return 0;

        default: rerror(ESYNTAX, E_EXP, 0);
        }


      /* initial assign expression*/
    case 26:
      switch(token)
        {
        case ADD:
          status = 15;
          pushexp(token, 0);
          return 0;
        case SUB:
          status = 15;
          pushexp(token, 0);
          return 0;
        case MUL:
          status = 15;
          pushexp(token, 0);
          return 0;
        case DIV:
          status = 15;
          pushexp(token, 0);
          return 0;
        case LN:
          status = STATUS_INIT;{pushexp(token, 0);ssymval(*sym, evalexpr());}  return 1;
        default: rerror(ESYNTAX, E_EXP, 0);
        }
    case 27:
      switch(token)
        {
        case NUMBER:
          status = 28;{pushexp(NUMBER, atoi(lexcad));}return 0;
        case IDEN:
          status = 28;{symbol ptr;(ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0);}return 0;
        case PARI:
          status = 27;
          pushexp(token, 0);
          return 0;
        default: rerror(ESYNTAX, E_EXP, 0);
        }

    case 28:
      switch(token)
        {
        case PARD:
          status = 28;
          pushexp(token, 0);
          return 0;
        case ADD:
          status = 27;
          pushexp(token, 0);
          return 0;
        case SUB:
          status = 27;
          pushexp(token, 0);
          return 0;
        case MUL:
          status = 27;
          pushexp(token, 0);
          return 0;
        case DIV:
          status = 27;
          pushexp(token, 0);
          return 0;
        case LN:
          status = STATUS_INIT;{pushexp(token, 0);ssymval(*sym, evalexpr());}return 1;
        default: rerror(ESYNTAX, E_EXP, 0);
        }


    case 30:


      /*Expresion de play*/

    case 31:
      switch(token)
        {
        case ADD:
          status = 32;
          pushexp(token, 0);
          return 0;
        case SUB:
          status = 32;
          pushexp(token, 0);
          return 0;
        case MUL:
          status = 32;
          pushexp(token, 0);
          return 0;
        case DIV:
          status = 32;
          pushexp(token, 0);
          return 0;
        case SOS:
          status = 36;
          return 0;
        case BEMOL:
          status = 36;
          return 0;
        case COMA:
          status = 32;{pushexp(LN, 0);ssymval(*sym, evalexpr());inscodeI(*sym, NULL, val);initexpr();} return 0;
        case LN:
          status = STATUS_BEGIN_CHN_BODY;{pushexp(token, 0);ssymval(*sym, evalexpr());inscodeI(*sym, NULL, val);} return 1;
        default: rerror(ESYNTAX, E_EXP, 0);
        }
    case 32:
      switch(token)
        {
        case NUMBER:
          status = 33;{pushexp(NUMBER, atoi(lexcad));}return 0;
        case IDEN:
          status = 33;{symbol ptr;(ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0);}return 0;
        case PARI:
          status = 32;
          pushexp(token, 0);
          return 0;
        default: rerror(ESYNTAX, E_EXP, 0);
        }

    case 33:
      switch(token)
        {
        case PARD:
          status = 33;
          pushexp(token, 0);
          return 0;
        case ADD:
          status = 34;
          pushexp(token, 0);
          return 0;
        case SUB:
          status = 34;
          pushexp(token, 0);
          return 0;
        case MUL:
          status = 34;
          pushexp(token, 0);
          return 0;
        case DIV:
          status = 34;
          pushexp(token, 0);
          return 0;
        case SOS:
          status = 36;{pushexp(token, 0);ssymval(*sym, evalexpr());}return 0;
        case BEMOL:
          status = 35;{pushexp(token, 0);ssymval(*sym, evalexpr());}return 0;
        case COMA:
          status = 32;{pushexp(LN, 0);ssymval(*sym, evalexpr());inscodeI(*sym, NULL, val);initexpr();} return 0;
        case LN:
          status = STATUS_BEGIN_CHN_BODY;{pushexp(token, 0);ssymval(*sym, evalexpr());inscodeI(*sym, NULL, val);}return 1;
        default: rerror(ESYNTAX, E_EXP, 0);
        }
    case 34:
      switch(token)
        {
        case NUMBER:
          status = 31;{pushexp(NUMBER, atoi(lexcad));}return 0;
        case IDEN:
          status = 31;{symbol ptr;(ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0);}return 0;
        case PARI:
          status = 32;
          pushexp(token, 0);
          return 0;
        default: rerror(ESYNTAX, E_EXP, 0);
        }

    case 35:
      switch(token)
        {
        case BEMOL:
          status = 35;{ssymval(*sym,((SIGNED char)gsymval(*sym))-3);}return 0;
        case LN:
          status = STATUS_BEGIN_CHN_BODY;
          {SIGNED char val;
            val = gsymval(*sym);
            val-=3;
            if(val<0)
              val = 33-val;

            ssymval(*sym, val);inscodeI(*sym, NULL, val);}return 1;
        default: rerror(ESYNTAX, E_BEMOL, 0);
        }

    case 36:
      switch(token)
        {
        case SOS:
          status = 36;{ssymval(*sym, gsymval(*sym)+3);}return 0;
        case LN:
          status = STATUS_BEGIN_CHN_BODY;{ssymval(*sym,(gsymval(*sym)+3)%34);inscodeI(*sym, NULL, val);}return 1;
        case COMA:
          status = 32;{pushexp(LN, 0);ssymval(*sym, evalexpr());inscodeI(*sym, NULL, val);initexpr();} return 0;
        default: rerror(ESYNTAX, E_SOS, 0);
        }


      /*      case 37:
              switch(token)
              //        {
              //        case IDEN:
              status = STATUS_BEGIN_GLOBAL_ASGN;{(*sym = searchsym(lexcad))? 0:rerror(0);}return 0;
              //        case LN: return 1;
              //        case CHANNEL:
              status = STATUS_BEGIN_CHN_DECL;
              return 0;
              //        default: rerror(ESYNTAX);
              //        }

              */

    case 38:
      switch(token)
        {
        case LN:
          status = 37;{errorg?rerror(EERROR, 0, 0):writesong();}return 2;            /*Fin de entrada*/
        default: rerror(ESYNTAX, E_BAD_COMM, 0);
        }


    case 39:
      switch (token)
        {
        case NUMBER:
          status = 40;{pushexp(NUMBER, atoi(lexcad));}return 0;
        case IDEN:
          status = 40;{symbol ptr;(ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0);}return 0;
        case PARI:
          status = 41;
          pushexp(token, 0);
          return 0;
        default: rerror(ESYNTAX, E_EXP, 0);
        }


    case 40:
      switch(token)
        {
        case ADD:
          status = 39;
          pushexp(token, 0);
          return 0;
        case SUB:
          status = 39;
          pushexp(token, 0);
          return 0;
        case MUL:
          status = 39;
          pushexp(token, 0);
          return 0;
        case DIV:
          status = 39;
          pushexp(token, 0);
          return 0;
        case POINT:
          status = 43;{pointp = point=32;};
          return 0;
        case LN:
          status = STATUS_BEGIN_CHN_BODY;
          {
            float time;

            pushexp(token, 0);
            time = evalexpr();
            time/=64.0;
            time*=(240.0*((float)(gsymval(hz))))/((float)(gsymval(tempo)));
            if((time-((float)((unsigned char)time))) > 0.5)
              time=((float)((unsigned char)time))+1.0;
            else
              time=(float)((unsigned char)time);

            ssymval(*sym,(unsigned char)time);
            inscodeI(*sym, NULL, val);
          }return 1;


        default: rerror(ESYNTAX, E_EXP, 0);
        }

    case 41:
      switch(token)
        {
        case NUMBER:
          status = 42;{pushexp(NUMBER, atoi(lexcad));}return 0;
        case IDEN:
          status = 42;{symbol ptr;(ptr = searchsym(lexcad))?pushexp(NUMBER, gsymval(ptr)):rerror(ESYNTAX, EIDEN_NAME, 0);}return 0;
        case PARI:
          status = 41;
          pushexp(token, 0);
          return 0;
        default: rerror(ESYNTAX, E_EXP, 0);
        }

    case 42:
      switch(token)
        {
        case PARD:
          status = 42;
          pushexp(token, 0);
          return 0;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
          status = 41;
          pushexp(token, 0);
          return 0;

        case POINT:
          status = 43;{pointp = point=32;} return 0;
        case LN:
          status = STATUS_BEGIN_CHN_BODY;
          {
            float time, timei;

            pushexp(token, 0);
            time = evalexpr();
            time/=64.0;
            time*=(240.0*((float)(gsymval(hz))))/((float)(gsymval(tempo)));
            time = fabs(time);

            timei = floor(time);
            if(time-timei)
              time = timei+1.0;
            else
              time = timei;

            ssymval(*sym,(unsigned char)time);
            inscodeI(*sym, NULL, val);
          }return 1;
        default: rerror(ESYNTAX, E_EXP, 0);
        }
    case 43:
      switch(token)
        {
        case POINT:
          pointp >>= 1;
          point += pointp;
          return 0;

        case LN:
          status = STATUS_BEGIN_CHN_BODY;
          {
            float time, timei;

            pushexp(token, 0);
            time = evalexpr();
            time/=64;
            time*=(240.0*((float)(gsymval(hz))))/((float)(gsymval(tempo)));
            time = fabs(time);
            time+=time*(((float)point)/64.0);


            timei = floor(time);
            if(time-timei)
              time = timei+1.0;
            else
              time = timei;

            ssymval(*sym,(unsigned char)time);
            inscodeI(*sym, NULL, val);
          }return 1;
        }
    default: rerror(ESYNTAX, E_POINT, 0);

    }
}







/*
int syntaxan1 (int  status, int token)
  {
    static struct reglar  ptr;

    ptr = rtable[status]+token;

    if(ptr->status!=-1)
      {
        status = ptr->status;
        return ptr->producc;
      }
    else
        rerror(ESYNTAX);

  }

  */







int syntaxerror (char * file, int line, char etp)
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
      "%s:%d: Channel already has been described\n",
      ""
    };




    fprintf(stderr,"%s:%d: Syntax error before %s (\\0x%02.2x)\n",
            file, line, lexcad,((unsigned)(*lexcad))&0xFF);
    if(etp==-1) return;
    fprintf(stderr, lserrors[etp], file, line);
    status = recst;
  }
