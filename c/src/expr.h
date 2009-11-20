#ifndef __EXPRH__
#define __EXPRH__


#define SIZESTK 50


struct eexprac 
  {
  char act;
  void (* func)();   
  };



#ifdef __EXPRC__
#define EXTERN
#else
#define EXTERN extern
#endif


#define pushexp(type,value) (push((type),(value),&tokff))



struct valuestack
  {
   float stack [SIZESTK];
   unsigned char sp;
   unsigned char head;
  };



struct expritem
  {
  unsigned char type;
  unsigned char value;
  };
  

struct tokfifo
  {
  struct expritem stack[SIZESTK];
  unsigned char sp;
  unsigned char head;
  };




EXTERN struct tokfifo tokff;




int push (char type,unsigned char value,struct tokfifo * tok);
unsigned char pop(struct tokfifo * tok);
int popn(struct tokfifo * tok,char n);
struct expritem * next(struct tokfifo * tok);
int pushval (float val);
float popval ();
int initexpr();
float evalexpr();

#undef EXTERN
#endif
