
#ifndef __ERRORH__
#define __ERRORH__


#ifdef __ERRORC__
#define EXTERN
#else
#define EXTERN extern
#endif


EXTERN char errorg;

int rerror (char nerror,char serr,char par1);


#define EMEMORY 0
#define ESYNTAX 1
#define ELEX    2
#define EEOF    3
#define EFILE   4
#define EERROR  5
#define E_INV_PAR 6

#ifdef _DEBUG_
    #define LOG(x, y)     fprintf(stderr, x, y);
#else
   #define LOG(x, y)
#endif


#undef EXTERN
#endif
