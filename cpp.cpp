#include "hpp.hpp"
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cerr<<YYERR; cout<<YYERR; exit(-1); }
int main() { env_init(); return yyparse(); }

void W(Sym*o) { cout << o->dump(); }
void W(string s) { cout << s; }

Sym::Sym(string T, string V) { tag=T; val=V; }
Sym::Sym(string V):Sym("",V) {}
void Sym::push(Sym*o) { nest.push_back(o); }

string Sym::tagval() { return "<"+tag+":"+val+">"; }
string Sym::tagstr() { return "<"+tag+":'"+val+"'>"; }
string Sym::pad(int n) { string S; for (int i=0;i<n;i++) S+='\t'; return S; }
string Sym::dump(int depth) { string S = "\n"+pad(depth)+tagval();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) S+=(*it)->dump(depth+1);
	return S;}

Sym* Sym::eval() {
	Sym*E = env[val]; if (E) return E;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) (*it)=(*it)->eval();
	return this;
}

Sym* Sym::eq(Sym*o)	{
	env[val]=o; //return o; }
	Sym* R = new Op("="); R->push(this); R->push(o); return o; }
Sym* Sym::at(Sym*o)	{
	Sym* R = new Op("@"); R->push(this); R->push(o); return R; }
Sym* Sym::add(Sym*o)	{
	Sym* R = new Op("+"); R->push(this); R->push(o); return R; }
Sym* Sym::str() { return new Str(val); }	

Str::Str(string V):Sym("str",V) {}
string Str::tagval() { return tagstr(); }
Sym* Str::add(Sym*o) { return new Str(val+o->str()->val); }
Sym* upcase(Sym*o) { string S = o->val;
	transform(S.begin(),S.end(),S.begin(),::toupper);
	return new Str(S); }

List::List():Sym("[","]") {}

Op::Op(string V):Sym("op",V) {}
Sym* Op::eval() { 
	if (val=="~") return nest[0]; else Sym::eval();
	if (nest.size()==2) {
		if (val=="=") return nest[0]->eq(nest[1]);
		if (val=="@") return nest[0]->at(nest[1]);
		if (val=="+") return nest[0]->add(nest[1]);
	}
	return this; }

Fn::Fn(string V, FN F):Sym("fn",V) { fn=F; }
Sym* Fn::at(Sym*o) { return fn(o); }

File::File(Sym*o):Sym("file",o->val) {}
string File::tagval() { return tagstr(); }
Sym* File::eq(Sym*o) {
	for (auto it=o->nest.begin(),e=o->nest.end();it!=e;it++) push(*it); 
	return this; }
Sym* file(Sym*o) { return new File(o); }

map<string,Sym*> env;
void env_init() {
	env["MODULE"]	= new Str(MODULE);
	// string
	env["upcase"]	= new Fn("upcase",upcase);
	// fileio
	env["file"]		= new Fn("file",file);
}

