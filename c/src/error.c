#define __ERRORC__
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "error.h"
#include "syntax.h"



extern int nline;
extern char *  filen;
jmp_buf recover;




int ierror()
  {
  errorg=0;
  }



int rerror (char nerror,char serr,char par1)
  {
    errorg=1;

    switch (nerror)
      {
      case EMEMORY :
        fprintf(stderr,"%s:%d: Internal memory error\n",filen,nline);
        exit(EXIT_FAILURE);
      case ESYNTAX :
        syntaxerror(filen,nline,serr);
        flushin();
        longjmp(recover,1);
      case ELEX :
        flushin();
        lexerror(filen,nline,par1);
        syntaxerror(filen, nline, E_LEX);
        longjmp(recover,1);
      case EEOF :
        fprintf(stderr,"%s:%d: Found EOF before write sentence\n",filen,nline);
        exit(EXIT_FAILURE);
      case EFILE:
        fprintf(stderr,"%s:%d: It can't be written output file\n",filen,nline);
        exit(EXIT_FAILURE);
      case EERROR:
        fprintf(stderr,"%s:%d: No output file written due to found errors\n",filen,nline);
        exit(EXIT_FAILURE);
      case E_INV_PAR:
        errorsem(nerror,serr,par1,nline,filen);
        longjmp(recover,1);
      }

  }



#undef __ERRORC__
