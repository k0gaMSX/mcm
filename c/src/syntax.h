#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#define EIDEN_NAME   0
#define E_VAR_DECL   1
#define E_CHANNEL    2
#define E_UNK_COMM   3
#define E_BAD_COMM   4
#define E_EXP        5
#define E_PAR        6
#define E_ASIG       7
#define E_BEMOL      8
#define E_SOS        9
#define E_POINT     10
#define E_INI       11
#define E_BAD_CH    12
#define E_DECL_CH   13
#define E_LEX       14
#include "symbol.h"


int getstatus();
int syntaxan (int token,symbol * sym);
int syntaxerror (char * file, int line, char etp);

#endif /* _SYNTAX_H_ */
