#ifndef __FILEH__
#define __FILEH__
#include <stdio.h>

#define __ASM__
#define __LITLE__



unsigned char readbyte (FILE * in);
unsigned short readword(FILE * in);
int writebyte(FILE * out,unsigned char byte);
int writeword(FILE * out,unsigned short word);


#endif
