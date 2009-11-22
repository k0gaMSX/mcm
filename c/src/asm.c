#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "symbol.h"
#include "error.h"
#include "code.h"
#include "lex.h"


int syntaxan (int token,symbol *);
int initsyn();


char * filen;
int nline;
FILE * out;
extern jmp_buf recover;



main (int argc,char * argv[])
  {
    static FILE * in;
    static int token;
    static symbol sym;

    nline=0;
    if(argc==1)
      {
        in=stdin;
        filen="stdin";
      }
    else
      {
        filen=argv[1];
        if(!(in=fopen(argv[1],"r")))
          {
            fprintf(stderr,"Error opening input file\n");
            exit(-1);
          }
      }



    initasm();                  /* Init symbol table */
    initmachine();
    ierror();                       /*Point of error recover*/
    setjmp(recover);
    for(;;)
      {
        token=gettoken(in);
        if ((syntaxan(token,&sym))==2) goto end;
#ifdef _DEBUG_
        fprintf(stderr,"Estado=%d\n",getstatus());
#endif
      }

end:
    fprintf(stderr,"Programa correcto!!!!!!\n");
  }







int initasm()
  {
  static struct initvars
    {
      char * name;
      unsigned char value;
    }vars[]={
             {"do",0},
             {"re",6},
             {"mi",12},
             {"fa",15},
             {"sol",21},
             {"la",27},
             {"si",33},
             {"C",0},
             {"D",6},
             {"E",12},
             {"F",15},
             {"G",21},
             {"A",27},
             {"B",33},
             {"c",0},
             {"d",6},
             {"e",12},
             {"f",15},
             {"g",21},
             {"a",27},
             {"b",33},
             {"tempo",120},
             {"Hz",60},
             {"d1",64},
             {"d2",32},
             {"d4",16},
             {"d8",8},
             {"d16",4},
             {"d32",2},
             {"d64",1},
             {"whole",64},
             {"half",32},
             {"quarter",16},
             {"eight",8},
             {"sixteenth",4},
             {"thirtysecond",2},
             {"sixtyfourth",1},
             {"redonda",64},
             {"blanca",32},
             {"negra",16},
             {"corchea",8},
             {"semicorchea",4},
             {"fusa",2},
             {"semifusa",1},
             {"psg1",0},
             {"psg2",1},
             {"psg3",2},
             {"fm1",3},
             {"fm2",4},
             {"fm3",5},
             {"fm4",6},
             {"fm5",7},
             {"fm6",8},
             {"fm7",9},
             {"fm8",10},
             {"fm9",11},
             {"original",0},
             {"violin",1},
             {"guitar",2},
             {"piano",3},
             {"flute",4},
             {"clarinet",5},
             {"oboe",6},
             {"trumpet",7},
             {"organ",8},
             {"horn",9},
             {"synth",10},
             {"harp",11},
             {"vibra",12},
             {"synthbass",13},
             {"acustic",14},
             {"electric",15},
             {NULL,0}};


  register struct initvars * ptr;


  for(ptr=vars;ptr->name;ptr++)
    ssymval(insvar(newsymbol(ptr->name)),ptr->value);


  initsyn();
  }















