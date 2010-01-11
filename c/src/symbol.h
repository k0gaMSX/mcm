#ifndef __SYMBOLH__
#define __SYMBOLH__



#define CADSIZE 14


struct symbol
 {
   char lex[CADSIZE+1];
   char type;
   union
     {
       unsigned char val;
       char car;
   }val;
   struct symbol * sig;
 };


typedef struct symbol * symbol;


symbol newsymbol (char *);
symbol insvar (symbol sym);
symbol searchsym (char *);

#define gsymval(sym) ((sym)->val.val)
#define ssymval(sym,num) ((sym)->val.val=(num))
#define ssymtype(sym, x) ((sym)->type = x)
#define gsymtype(sym) ((sym)->type)
#define ssymsig(sym1,sym2) ((sym1)->sig=(sym2))
#define gsymsig(sym) ((sym)->sig)
#define gsymlex(sym) ((sym)->lex)
#define initsym(sym,t,v) ((sym)->type=(t),(sym)->val.val=(v),(sym)->sig=NULL)

#endif
