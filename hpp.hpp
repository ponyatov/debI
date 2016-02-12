#ifndef _H_DECL
#define _H_DECL

#include <iostream>
#include <cstdlib>
#include <vector>
using namespace std;

struct Sym {
	string val;
	Sym(string);
	vector<Sym*> nest; void push(Sym*);
	string dump(int depth=0); string tagval(); string pad(int);
};

extern void W(Sym*);
extern void W(string);

struct List:Sym { List(); };

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(X) { yylval.o = new Sym(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_DECL
