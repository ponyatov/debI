#ifndef _H_DECL
#define _H_DECL

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <map>
using namespace std;

struct Sym {
	string tag,val;
	// ---------------
	Sym(string,string); Sym(string);
	// ---------------
	vector<Sym*> nest; void push(Sym*);
	// ---------------
	string dump(int depth=0);
	virtual string tagval();
	string tagstr();
	string pad(int);
	// ---------------
	virtual Sym* eval();
	// ---------------
	virtual Sym* eq(Sym*);
	virtual Sym* at(Sym*);
	virtual Sym* add(Sym*);
	virtual Sym* str();
};

extern map<string,Sym*> env;
extern void env_init();

extern void W(Sym*);
extern void W(string);

struct Str:Sym { Str(string); string tagval(); Sym*add(Sym*); };

struct List:Sym { List(); };

struct Op:Sym { Op(string); Sym*eval(); };

typedef Sym*(*FN)(Sym*);
struct Fn:Sym { Fn(string,FN); FN fn; Sym*at(Sym*); };

struct Dir:Sym { Dir(Sym*); string tagval(); };
struct File:Sym { File(Sym*); string tagval(); Sym*eq(Sym*); };

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(C,X) { yylval.o = new C(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_DECL
