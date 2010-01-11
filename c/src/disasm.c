#include <stdio.h>

#define __LITLE__



int decopcode (unsigned char opcode,unsigned char * par);
int writeword(FILE * out,unsigned short word);
int writebyte(FILE * out,unsigned char byte);
unsigned short readword(FILE * in);
unsigned char readbyte (FILE * in);






static char * helps[][2]={{"\t\t\t;End of channel\n","\t\t\t;End of channel\n"},
			  {"\t\t\t;Play tone\n","\t\t\t;Play tone\n"},
			  {"\t\t\t;Silence\n","\t\t\t;Silence\n"},
			  {"\t\t\t;Decrement octavo\n","\t\t\t;Decrement octavo\n"},
			  {"\t\t\t;Increment octavo\n","\t\t\t;Increment octavo\n"},
			  {"\t\t\t;Change octavo\n","\t\t\t;Change octavo\n"},
			  {"\t\t\t;Decrement volume\n","\t\t\t;Decrement volume\n"},
			  {"\t\t\t;Increment volume\n","\t\t\t;Increment volume\n"},
			  {"\t\t\t;Change volume\n","\t\t\t;Change volume\n"},
			  {"\t\t\t;Play drums\n","\t\t\t;Change Instrument\n"},
    		          {"\n","\n"},
			  {"\n\n","\n\n"},
			  {"\t\t\t;Change amplitude modulation ¿?\n","\t\t\t;Change amplitude modulation ¿?\n"},
			  {"\t\t\t;Change vibrato mode ¿?\n","\t\t\t;Change vibrato mode ¿?\n"},
			  {" \t\t;Change timing\n"," \t\t;Change timing\n"},
			  {" \t\t;Change timing\n"," \t\t;Change timing\n"},
			  {"\t\t\t;Change sustain time factor\n","\t\t\t;Change sustain time factor\n"},
			  {" \t\t;Change timing\n"," \t\t;Change timing\n"},
			  {"\n","\n"},
			  {"\t\t\t;Play drums\n","\t\t\t;Change Instrument\n"},
			  {"\t\t\t;Write FM register","Write FM register\n"},
			  {"\t\t\t;Change frecuency modulation mode¿?\n","\t\t\t;Change frecuency modulation mode¿?\n"},
			  {"\t\t;Change driver parameter\n","\t\t;Change driver parameter\n"}};




static char * nmeml[][2] = {
                     {"endc","endc"},
		     {"play","play"},
		     {"rest","rest"},
		     {"decoc","decoc"},
		     {"incoc","incoc"},
		     {"oct","oct"},
		     {"decv","decv"},
		     {"incv","incv"},
		     {"vol","vol"},
	             {"drum","inst"},
		     {"loop","loop"},
		     {"endl","endl"},
		     {"adsr","adsr"},
		     {"vib","vib"},
		     {"timel","timel"},
		     {"times","times"},
		     {"divsus","divsus"},
		     {"timel","timel"},
		     {"loopl","loopl"},
		     {"druml","instl"},
		     {"regfm","regfm"},
		     {"frec","frec"},
		     {"par","par"}};


static char * parl[]={"chnoff",
		   "chnon",
		   "suson",
		   "susoff",
		   "sdhh",
		   "tomtcy",
		   "noise",
		   "noiseoff",
		   "deco",
		   "tamp",
		   "ritmon",
		   "melon"};










int main (int argc,char * argv[])
  {
   FILE * in;
   int canal,code,chip,loopn;
   unsigned char byte,par;
   unsigned short addr,cont;
   char nameout[20];
   static char * chips[]={"PSG","FM"};


   if (argc == 1)
     in=stdin;
   else if(!(in=fopen(argv[1],"rb")))
     {
     fprintf(stderr,"Error opening input file\n");
     exit(-1);
     }


   cont=1;
   if((byte=readbyte(in))==0xfe)
     {
     readword(in);
     readword(in);
     readword(in);
     cont+=6;
     }
   else if (byte==0x0c)
     {
     readbyte(in);
     cont++;
     }
   else
     {
     fprintf(stderr,"Input file is not a MCM song\n");
     exit(-1);
     }




   for(canal=0,addr=readword(in);addr;addr=readword(in),canal++)
     {
       loopn=0;
       chip=(canal<3) ? 0:1;
       fprintf(stdout,"\n\n\t\tchannel %d\t\t;%s\n",canal+1,chips[chip],(canal<3) ? canal : canal-3);
       for(cont+=2;cont<addr;cont++)
	 {
	   byte=readbyte(in);
	   code=decopcode(byte,&par);

	   if (code == 3)       code+=par;
	   else if (code == 5)  code+=par+1;
	   else if (code == 4)  code++;
	   else	if (code > 4)   code+=2;


	   if (code == 0 || code == 2 || code == 3 || code == 4 ||
	       code == 6 || code == 7 || code == 11)
	     {
	       fprintf(stdout,"\t\t%s",nmeml[code][chip]);
	       if (code == 11)
		 {
		   if (loopn <= 0) fprintf(stderr,"Warning: Bad numbers of internal loops\n");
		   fprintf(stdout,"\t\t\t;end of loop %d",--loopn);
		 }
	       fprintf(stdout,"%s",helps[code][chip]);
	     }

	   else if (code == 1 || code == 5 || code == 8 || code == 9 ||
		    code == 10 || code == 12 || code == 13 ||
		    code == 15 || code == 16)
	     {
	       if (code == 10)
		 {
		   fprintf(stdout,"\n\t\t%s %d",nmeml[code][chip],par);
		   fprintf(stdout,"\t\t\t;begin of loop %d",loopn++);
		 }
	       else  fprintf(stdout,"\t\t%s %d",nmeml[code][chip],par);
	       fprintf(stdout,"%s",helps[code][chip]);
	     }
	   else
	     {
	       if (cont+1 >= addr)
		 {
		   fprintf(stderr,"Input file corrupt\n");
		   exit(-1);
		 }

	       byte=readbyte(in);
	       cont++;

	       if (code == 22)
		 if (byte > 11)
		   {
		     fprintf(stderr,"Warning: Number of paramenter function incorrect\n");
		     fprintf(stdout,"\t\t%s %d",nmeml[code][chip],byte);
		   }
		 else if (byte == 8 || byte == 9)
		   {
		     par=readbyte(in);
		     cont++;
		     fprintf(stdout,"\t\t%s %s %d",nmeml[code][chip],parl[byte],par);
		   }
		 else
		   fprintf(stdout,"\t\t%s %s",nmeml[code][chip],parl[byte]);

	       else if (code == 20)
		 {
		   if (cont+1 >= addr)
		     {
		       fprintf(stderr,"Input file corrupt\n");
		       exit(-1);
		     }

		   par=readbyte(in);
		   cont++;
		   fprintf(stdout,"\t\t%s %d %d",nmeml[code][chip],byte,par);
		 }
	       else
		 if (code == 18)
		   {
		     fprintf(stdout,"\n\t\t%s %d",nmeml[code][chip],byte);
		     fprintf(stdout,"\t\t\t;begin of loop",loopn++);
		   }
		 else
		   fprintf(stdout,"\t\t%s %d",nmeml[code][chip],byte);

	       fprintf(stdout,"%s",helps[code][chip]);
	     }
	 }

     }
   fprintf(stdout,"\n\n\n\t\twrite\n");
  }






int decopcode (unsigned char opcode,unsigned char * par)
  {
  int i;
  static  unsigned char val[]={0x24,0x01,0x02,0x08,0x02,
			       0x10,0x20,0x10,0x01,0x10,
			       0x10,0x04,0x5b,0x08,0x01,
			       0x01,0x01,0x01,0x01,0x01};



  if (!opcode) return 0;

  for(i=0,opcode--;opcode>=val[i];i++)
    opcode-=val[i];

  *par=opcode;
   return ++i;
  }




unsigned char readbyte (FILE * in)
  {
    unsigned char car;
    int aux;


    if(!fread(&car,1,1,in))
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


