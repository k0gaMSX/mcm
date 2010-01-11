#define __LEXC__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokens.h"
#include "error.h"
#include "symbol.h"
#include "lex.h"









int wfgetc(FILE * in)
  {
  char c;
  if ((c=fgetc(in))==EOF)
    rerror(EEOF,0,0);

  return c;
  }






static char car='\n';

/*TODO: Change numbers for status names*/

int gettoken (FILE * in)
{
  static char * cad;
  static char cadsize,status;
  static char ret=LN;
  extern int nline;

  oldin=in;

  *lexcad='\0';
  car=wfgetc(in);
  cad=lexcad;
  for (status = 0, cadsize = 0; ; car = wfgetc(in), cad++)
    {
      *cad=car;
      LOG("read from input = %c ", car);
      LOG("in state %d\n", status);
      switch (status)
        {
        case 0:
          switch (car)
            {
            case '\n': nline++; ret=LN; goto copylex1;
            case ':': *cad='\n'; ret=LN;goto copylex1;
            case ';': flushinr();  *cad='\n';ret=LN; goto copylex1;
            case ' ':
            case '\t':  cad --; break;
            case '#':
              if(ret == LN)
                {
                  nline--;
                  flushinr();
                  *cad = '\n';
                }
              else
                ret=SOS;

              goto copylex1;

            case '&': ret=BEMOL; goto copylex1;
            case '=': ret=EQ; goto copylex1;
            case '*': ret=MUL; goto copylex1;
            case '+': ret=ADD; goto copylex1;
            case ',': ret=COMA; goto copylex1;
            case '-': ret=SUB; goto copylex1;
            case '(': ret=PARI; goto copylex1;
            case ')': ret=PARD; goto copylex1;
            case '/': ret=DIV; goto copylex1;
            case '.': ret=POINT; goto copylex1;

            default:
              if (isdigit(car))
                {
                  status=1;
                  if(++cadsize>3)    /*El maximo numero es 255*/
                    rerror(ELEX,0,car);
                  break;
                }
              else if (isalpha(car))
                {
                  status=2;
                  if(++cadsize > 3)
                    rerror(ELEX,0,car);

                  break;
                }
              else
                rerror(ELEX,0,car);
            }
          break;


        case 1:
          if(!isdigit(car))
            {
              if(car!='?') ungetc(car,in);
              *cad='\0';
              ret=NUMBER;
              goto copylex;
            }

          else if (isdigit(car))
            cadsize++;
          else
            rerror(ELEX,0,car);
          break;


        case 2:
          if(!(isdigit(car) || isalpha(car)))
            {
              register struct wordr * ptr;
              *cad = '\0';
              if (car != '?') ungetc(car,in);
              for (ptr = wordrs; ptr->name; ptr++)
                if (!strcmp(lexcad,ptr->name))
                  {
                    ret = ptr->val;
                    goto copylex;
                  }

              ret=IDEN;
              goto copylex;
            }
          else if(isdigit(car) || isalpha(car))
            cadsize++;
          else
            rerror(ELEX,0,car);
          break;
        }
    }



 copylex1:
  cad++;
 copylex:
  *cad='\0';
  return ret ;
}







int flushinr()
  {
    extern int nline;

    if(car == '\n')
      return;

    while (car != '\n')
      {
        car=wfgetc(oldin);
#ifdef _DEBUG_
        fprintf(stderr,"Limpiando...\n");
#endif
      }
    nline++;
  }



int flushin ()
  {
  extern int nline;

  if (car == '\n' || car==':')
    return;

  while (car != '\n' && car != ':')
    {
    car = wfgetc(oldin);
    LOG("Cleanning...", NULL);
    }


  if(car=='\n') nline++;
  }










int lexerror(char * file,int line,char car)
  {


    fprintf(stderr,"%s:%d: Unexpected character: %c\n",file, line, car);
    return 0;
  }



#undef __LEXC__
