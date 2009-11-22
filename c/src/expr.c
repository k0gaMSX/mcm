#define __EXPRC__
#include <stdio.h>
#include "symbol.h"
#include "error.h"
#include "tokens.h"
#include "expr.h"
#include "syntax.h"



#define tailt(str) ((str).stack[(str).sp-1].type)
#define tailv(str) ((str).stack[(str).sp-1].value)
#define headf(str) ((str).stack[(str).head].type)




#define D 1
#define R 2
#define A 0
#define E 3
#define NF NULL

void R1(char * status);
void R2(char * status);
void R3(char * status);
void R4(char * status);
void R5(char * status);
void R6(char * status);
void R7(char * status);
void R8(char * status);



/*                            a  +  *  (  )  $  -  /  E  T  F*/
static char transtb[][11]={
/*0*/                       { 5,-1,-1, 4,-1,-1,-1,-1, 1, 2, 3},
/*1*/                       {-1, 7,-1,-1,-1, 6, 8,-1,-1,-1,-1},
/*2*/                       {-1,-1, 9,-1,-1,-1,-1,10,-1,-1,-1},
/*3*/                       {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*4*/                       { 5,-1,-1, 4,-1,-1,-1,-1,11,12, 3},
/*5*/                       {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*6*/                       {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*7*/                       { 5,-1,-1, 4,-1,-1,-1,-1,-1,13, 3},
/*8*/                       { 5,-1,-1,-1,-1,-1,-1,-1,-1,14, 3},
/*9*/                       { 5,-1,-1, 4,-1,-1,-1,-1,-1,-1,15},
/*10*/                      { 5,-1,-1, 4,-1,-1,-1,-1,-1,-1,16},
/*11*/                      { 5, 8,-1, 4,17,-1,-1,-1,-1,18,-1},
/*12*/                      {-1,-1, 9,-1,-1,-1,-1,10,-1,-1,-1},
/*13*/                      {-1,-1, 9,-1,-1,-1,-1,10,-1,-1,-1},
/*14*/                      {-1,-1,-9,-1,-1,-1,-1,10,-1,-1,-1},
/*15*/                      {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*16*/                      {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*17*/                      {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*18*/                      {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}};




static struct eexprac acttb[][8]={
/*         a      +      *      (      )      $      -      /
/*0*/   {{D,NF},{E,NF},{E,NF},{D,NF},{E,NF},{E,NF},{E,NF},{E,NF}},
/*1*/   {{E,NF},{D,NF},{E,NF},{E,NF},{E,NF},{A,NF},{D,NF},{E,NF}},
/*2*/   {{E,NF},{R,R2},{D,NF},{E,NF},{R,R2},{R,R2},{R,R2},{D,NF}},
/*3*/   {{E,NF},{R,R4},{R,R4},{E,NF},{R,R4},{R,R4},{R,R4},{R,R4}},
/*4*/   {{D,NF},{E,NF},{E,NF},{D,NF},{E,NF},{E,NF},{E,NF},{E,NF}},
/*5*/   {{E,NF},{R,R6},{R,R6},{E,NF},{R,R6},{R,R6},{R,R6},{R,R6}},
/*6*/   {{E,NF},{E,NF},{E,NF},{E,NF},{E,NF},{A,NF},{E,NF},{E,NF}},
/*7*/   {{D,NF},{E,NF},{E,NF},{D,NF},{E,NF},{E,NF},{E,NF},{E,NF}},
/*8*/   {{D,NF},{E,NF},{E,NF},{D,NF},{E,NF},{E,NF},{E,NF},{E,NF}},
/*9*/   {{D,NF},{E,NF},{E,NF},{D,NF},{E,NF},{E,NF},{E,NF},{E,NF}},
/*10*/  {{D,NF},{E,NF},{E,NF},{D,NF},{E,NF},{E,NF},{E,NF},{E,NF}},
/*11*/  {{D,NF},{D,NF},{E,NF},{D,NF},{D,NF},{E,NF},{D,NF},{E,NF}},
/*12*/  {{E,NF},{R,R2},{D,NF},{E,NF},{R,R2},{E,NF},{R,R2},{D,NF}},
/*13*/  {{E,NF},{R,R1},{D,NF},{E,NF},{R,R1},{R,R1},{R,R1},{D,NF}},
/*14*/  {{E,NF},{R,R7},{D,NF},{E,NF},{R,R7},{R,R7},{R,R7},{D,NF}},
/*15*/  {{E,NF},{R,R3},{R,R3},{E,NF},{R,R3},{R,R3},{R,R3},{R,R3}},
/*16*/  {{E,NF},{R,R8},{R,R8},{E,NF},{R,R8},{R,R8},{R,R8},{R,R8}},
/*17*/  {{E,NF},{R,R5},{R,R5},{E,NF},{R,R5},{R,R5},{R,R5},{R,R5}},
/*18*/  {{E,NF},{E,NF},{D,NF},{E,NF},{E,NF},{E,NF},{E,NF},{D,NF}}};










static struct valuestack valstk;
static struct tokfifo stkSLR;



int initexpr()
  {
    valstk.sp=0;
    tokff.sp=0;
    tokff.head=0;
    stkSLR.sp=0;
    stkSLR.head=0;
  }






float evalexpr()
  {
  static char status;
  static unsigned char term;
  static struct eexprac * actual;
  static struct expritem * ptr;
  static float val;

  status=0;
  push(0,0,&stkSLR);

    for(;;)
      {
        if (status == -1)       /*esto solo se da por los parentesis, esta comprobado por el analisis anterior*/
          rerror(ESYNTAX, E_EXP,0);


        actual=&acttb[status][headf(tokff)];
#ifdef _DEBUG_
        fprintf(stderr,"Estado del evaluador: %d %d %d\n",status,actual->act,headf(tokff));
#endif
        if (actual->act == D)
           {
           ptr=next(&tokff);
           term=ptr->type;
           push(term,ptr->value,&stkSLR);
           }
        else if (actual->act == R)
          {
          (*actual->func)(&status);
          term=tailt(stkSLR);
          }
        else if(actual->act == E)
          rerror(ESYNTAX, E_EXP,0);
        else
           {
             val=popval();
#ifdef _DEBUG_
             fprintf(stderr,"Calculo=%f\n", val);
#endif
             return val;
           }



         status=transtb[status][term];
         push(status,0,&stkSLR);
      }
  }







int push (char type,unsigned char value,struct tokfifo * tok)
  {
  register struct expritem * ptr;
  register struct tokfifo * tk;


  tk=tok;
  if(tk->sp == SIZESTK)
    rerror(EMEMORY,0,0);

  ptr=tk->stack+tk->sp++;
  ptr->type=type;
  ptr->value=value;
  }


unsigned char pop(struct tokfifo * tok)
  {
  register struct tokfifo * ptr;

  ptr=tok;
  if(!ptr->sp)
    rerror(EMEMORY,0,0);

  return ptr->stack[--ptr->sp].value;
  }


int popn(struct tokfifo * tok,char n)
  {
  register struct tokfifo * ptr;
  register unsigned int val;

  ptr=tok;
  if((val=ptr->sp-n)<=0)
    rerror(EMEMORY,0,0);

  ptr->sp=(unsigned char)val;
  }





struct expritem * next(struct tokfifo * tok)
  {
  static struct tokfifo * ptr;

  ptr=tok;
  if(ptr->head == ptr->sp)
     rerror(EMEMORY,0,0);

 return &ptr->stack[ptr->head++];
 }




int pushval (float val)
  {
  if(valstk.sp == SIZESTK)
     rerror(EMEMORY,0,0);

#ifdef _DEBUG_
  fprintf(stderr,"En cola -> %f\n",val);
#endif
  valstk.stack[valstk.sp++]=val;
  }



float popval ()
  {
  if(!valstk.sp)
     rerror(EMEMORY,0,0);

  return valstk.stack[--valstk.sp];
  }





void R1(char * status)
  {
  static float val1;
  static float val2;

#ifdef _DEBUG_
  fprintf(stderr,"Reduccion 1\n");
#endif
  popn(&stkSLR,6);
  *status=tailt(stkSLR);
  push(E_NT,0,&stkSLR);


  val1=popval(&valstk);
  val2=popval(&valstk);
  pushval(val1+val2);
  }



void R2(char * status)
  {
#ifdef _DEBUG_
  fprintf(stderr,"Reduccion 2\n");
#endif
  popn(&stkSLR,2);
  *status=tailt(stkSLR);
  push(E_NT,0,&stkSLR);
  }



void R3 (char * status)
  {
  static float val1;
  static float val2;
#ifdef _DEBUG_
  fprintf(stderr,"Reduccion 3\n");
#endif
  popn(&stkSLR,6);
  *status=tailt(stkSLR);
  push(T_NT,0,&stkSLR);


  val1=popval(&valstk);
  val2=popval(&valstk);
  pushval(val1*val2);
  }


void R4 (char * status)
  {
#ifdef _DEBUG_
  fprintf(stderr,"Reduccion 4\n");
#endif
  popn(&stkSLR,2);
  *status=tailt(stkSLR);
  push(T_NT,0,&stkSLR);
  }



void R5 (char * status)
  {
#ifdef _DEBUG_
  fprintf(stderr,"Reduccion 5\n");
#endif
  popn(&stkSLR,6);
  *status=tailt(stkSLR);
  push(F_NT,0,&stkSLR);
  }



void R6(char * status)
  {
  char val;
#ifdef _DEBUG_
  fprintf(stderr,"Reduccion 6\n");
#endif
  pop(&stkSLR);
  val=pop(&stkSLR);
  *status=tailt(stkSLR);
  push(F_NT,0,&stkSLR);

  pushval(val);
  }



void R7 (char * status)
  {
  static float val1;
  static float val2;

#ifdef _DEBUG_
  fprintf(stderr,"Reduccion 7\n");
#endif
  popn(&stkSLR,6);
  *status=tailt(stkSLR);
  push(T_NT,0,&stkSLR);


  val1=popval(&valstk);
  val2=popval(&valstk);
  pushval(val2-val1);
  }


void R8 (char * status)
  {
  static float val1;
  static float val2;

#ifdef _DEBUG_
  fprintf(stderr,"Reduccion 8\n");
#endif
  popn(&stkSLR,6);
  *status=tailt(stkSLR);
  push(T_NT,0,&stkSLR);


  val1=popval(&valstk);
  val2=popval(&valstk);
  pushval(val2/val1);
  }




#undef __EXPRC__
