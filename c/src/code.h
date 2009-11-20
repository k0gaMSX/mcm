#ifndef __CODEH__
#define __CODEH__


#define inscode2(par1,par2) (inscode((par1)),inscode((par2)))
#define inscode3(par1,par2,par3) (inscode((par1)),inscode((par1),(par2)),inscode((par3)))




int initmachine();
int inscode (unsigned char code);
int execute ();
char inscodeI (symbol sym,symbol ant,char);
char writesong ();
int errorsem(char nerror,char comm,char chip,int nline,char * file);


#endif
