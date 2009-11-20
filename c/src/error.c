#define __ERRORC__
#include <stdio.h>
#include <setjmp.h>
#include "error.h"



extern int nline;
extern char *  filen;
static jmp_buf recover;




int ierror()
  {
  errorg=0;
  setjmp(recover);
  }



int rerror (char nerror,char serr,char par1)
  {
    errorg=1;

    switch (nerror)
      {
      case EMEMORY :
        fprintf(stderr,"%s:%d: Internal memory error\n",filen,nline);
        exit(-1);
      case ESYNTAX :
        syntaxerror(filen,nline,serr);
        flushin();
        longjmp(recover,1);
      case ELEX :
        lexerror(filen,nline,par1);
        syntaxerror(filen,nline,0);
        flushin();
        longjmp(recover,1);
      case EEOF :
        fprintf(stderr,"%s:%d: Found EOF before write sentence\n",filen,nline);
        exit(-1);
      case EFILE:
        fprintf(stderr,"%s:%d: It can't be written output file\n",filen,nline);
        exit(-1);
      case EERROR:
        fprintf(stderr,"%s:%d: No output file written due to finding errors\n",filen,nline);
        exit(-1);
      case E_INV_PAR:
        errorsem(nerror,serr,par1,nline,filen);
        longjmp(recover,1);
      }

  }



#undef __ERRORC__
