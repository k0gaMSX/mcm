#ifndef __TOKENSH__
#define __TOKENSH__


#define NUMBER 0
#define ADD 1
#define MUL 2
#define PARI 3
#define PARD 4
#define LN 5
#define SUB 6
#define DIV 7


#define E_NT 8
#define T_NT 9
#define F_NT 10




#define ENDC 8
#define PLAY 9
#define SIL 10
#define DECOC 11
#define INCOC 12
#define OCT 13
#define DECV 14
#define INCV 15
#define VOL 16
#define BAT 17
#define INST 18
#define LOOP 19
#define ENDL 20
#define AMPL 21
#define VIB 22
#define TIMEL 23
#define TIMES 24
#define DIVSUS 25
#define BATL 26
#define INSTL 27
#define REGFM 28
#define FREC 29
#define PAR 30
#define ENVLOFF 31
#define ENVLON 32
#define SUSON 33
#define SUSOFF 34
#define HFNUM 35
#define LFNUM 36
#define NOISE 37
#define TONE 38
#define DECO 39
#define TAMP 40
#define RITMON 41
#define MELON 42
#define IDEN 43
#define LOOPL 44
#define CHANNEL 45
#define MODEFM 46
#define VAR 47
#define SOS 48
#define BEMOL 49
#define EQ 50
#define COMA 51
#define WRITE 52
#define POINT 53
#define TIMEP 54




struct token
  {
  char * cad;
  int i; 
  struct token * ptr;
  };




#endif
