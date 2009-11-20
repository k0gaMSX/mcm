#include <stdio.h>
#include "file.h"
#ifdef __ASM__
#include "error.h"
#endif




unsigned char readbyte (FILE * in)
  {
    unsigned char car;


    if(!fread(&car,1,1,in))
#ifdef __ASM__
	rerror(EFILE,0,0);
#else
      {
	fprintf(stderr,"I can't read file input\n");
        exit(-1);
       }
#endif
    return car;
  }





 unsigned short readword(FILE * in)
  {
    unsigned short value;

    value=readbyte(in);

#ifdef __LITLE__
    value=value+(readbyte(in)<<8);
#else
    value=(value<<8)+readbyte(in);
#endif
    
    return (value);


  } 






int writebyte(FILE * out,unsigned char byte)
  {
    if(!fwrite(&byte,1,1,out))
#ifdef __ASM__
      rerror(EFILE,0,0);
#else

      {
	fprintf(stderr,"I can't write file output\n");
        exit(-1);
       }
#endif
    return 0;
  }  



int writeword(FILE * out,unsigned short word)
  {
    unsigned char aux;

#ifdef __LITLE__
    aux=(unsigned char) word;
    writebyte(out,aux);
    aux=(unsigned char) (word>>8);
    writebyte(out,aux);
#else
    aux=(unsigned char) (word>>8);
    writebyte(out,aux);
    aux=(unsigned char) word;
    writebyte(out,aux);
#endif

    return 0;
  }

