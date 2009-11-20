#include <stdio.h>
#include <string.h>


#define __LITLE__

int writeword(FILE * out,unsigned short word);
int writebyte(FILE * out,unsigned char byte);
unsigned short readword(FILE * in);
unsigned char readbyte (FILE * in);
     

int main (int argc, char * argv[])
  {
    FILE * in, * out;
    unsigned char byte;
    unsigned short word,addr,numb;
    unsigned long cont;
    int i,j;
    static char ccanal[]="canal";
    static char cmcm[]=".mcm";
    char nameout [12];
    char cnumber[4];
        

    if (argc != 2)
       {
	 fprintf(stderr,"Command line is: extract namefile\n");
         exit(-1);
       }
    
    if(!(in=fopen(argv[1],"r+b")))
      {
	fprintf(stderr,"Error reading input file\n");
        exit(-1);
       }
   


    cont=2;
    if((byte=readbyte(in))==0xf0)
      {
      readword(in);
      readword(in);
      readword(in);
      cont+=6;
      }
    readbyte(in);  
    



    for(i=0;i<10;i++)
      {
	sprintf(cnumber,"%d",i);	
	if(!(out=fopen(strcat(strcat(strcpy(nameout,ccanal),cnumber),cmcm),"w+b")))
          {
	    fprintf(stderr,"I can't open %s for writing\n");
            continue;
          }

        numb=0;        
        addr=readword(in);
        cont+=2;
        writebyte(out,0x0c);
        writebyte(out,0x00);
        numb+=2;
        for(j=0;j<10;j++)
          {
	    if(i!=j)
              {
	      writeword(out,numb+3);
              writebyte(out,0);
              numb+=3;
              }
            else
	      {
                numb+=2;
                writeword(out,numb+(addr-cont));
                while(cont!=addr)
	          {
		  writebyte(out,readbyte(in));
                  numb++;
		  cont++;
                  }
              }          
          }
        writeword(out,0);
        fclose(out);        
      }
    fclose(in);
  }







unsigned char readbyte (FILE * in)
  {
    unsigned char car;


    if(!(fread(&car,1,1,in)))
      {
	fprintf(stderr,"I can't read file input\n");
        exit(-1);
       }
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
      {
	fprintf(stderr,"I can't write file input\n");
        exit(-1);
       }
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
   



    
