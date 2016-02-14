#include "hpp.hpp"
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cerr<<YYERR; cout<<YYERR; exit(-1); }
int main() { glob_init(); return yyparse(); }

void W(Sym*o) { cout << o->dump(); }
void W(string s) { cout << s; }

Sym::Sym(string T, string V) { tag=T; val=V; env = new Env(&glob); }
Sym::Sym(string V):Sym("",V) {}
void Sym::push(Sym*o) { nest.push_back(o); }
//void Sym::parval(Sym*o) { par[o->str()->val]=o; }

void Sym::par(Sym*o) { env->set(o->str()->val,o); }

string Sym::tagval() { return "<"+tag+":"+val+">"; }
string Sym::tagstr() { return "<"+tag+":'"+val+"'>"; }
string Sym::pad(int n) { string S; for (int i=0;i<n;i++) S+='\t'; return S; }
string Sym::dump(int depth) { string S = "\n"+pad(depth)+tagval()+env->dump();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		S+=(*it)->dump(depth+1);
	return S;}

Sym* Sym::eval() {
	Sym*E = env->lookup(val); if (E) return E;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) (*it)=(*it)->eval();
	return this;
}

Sym* Sym::eq(Sym*o)	{
	env->next->set(val,o); return o; }
//	Sym* R = new Op("="); R->push(this); R->push(o); return R; }
Sym* Sym::at(Sym*o)	{
	Sym* R = new Op("@"); R->push(this); R->push(o); return R; }
Sym* Sym::add(Sym*o)	{
	Sym* R = new Op("+"); R->push(this); R->push(o); return R; }
Sym* Sym::div(Sym*o)	{
	Sym* R = new Op("/"); R->push(this); R->push(o); return R; }
Sym* Sym::str() { return new Str(val); }	

Str::Str(string V):Sym("str",V) {}
string Str::tagval() { return tagstr(); }
Sym* Str::add(Sym*o) { return new Str(val+o->str()->val); }
Sym* upcase(Sym*o) { string S = o->val;
	transform(S.begin(),S.end(),S.begin(),::toupper);
	return new Str(S); }

List::List():Sym("[","]") {}
Sym* List::str() { string S;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) S+=(*it)->str()->val;
	return new Str(S); }
Sym* List::div(Sym*o) {
	Sym* L = new List();
	if (nest.size()>=2) {
		for (auto it=nest.begin(),e=nest.end();it!=e;it++) {
			L->push(*it); L->push(o); }
		L->nest.pop_back(); }
	return L; }

Op::Op(string V):Sym("op",V) {}
Sym* Op::eval() { 
	if (val=="~") return nest[0]; else Sym::eval();
	if (nest.size()==2) {
		if (val=="=") return nest[0]->eq(nest[1]);
		if (val=="@") return nest[0]->at(nest[1]);
		if (val=="+") return nest[0]->add(nest[1]);
		if (val=="/") return nest[0]->div(nest[1]);
	}
	return this; }

Fn::Fn(string V, FN F):Sym("fn",V) { fn=F; }
Sym* Fn::at(Sym*o) { return fn(o); }

Lambda::Lambda():Sym("^","^") {}

File::File(Sym*o):Sym("file",o->val) {}
string File::tagval() { return tagstr(); }
Sym* File::eq(Sym*o) {
	for (auto it=o->nest.begin(),e=o->nest.end();it!=e;it++) push(*it); 
	return this; }
Sym* file(Sym*o) { return new File(o); }

Env::Env(Env*X) { next=X; }
void Env::set(string V,Sym*o) { iron[V]=o; }
Sym* Env::lookup(string V) {
	auto it = iron.find(V); if (it!=iron.end()) return it->second;
	if (next) return next->lookup(V);
	return NULL; }

string Env::dump() { string S;
	for (auto it=iron.begin(),e=iron.end();it!=e;it++)
		S += ","+it->first;
	return S; }

Env glob(NULL);
void glob_init() {
	glob.iron["MODULE"]	= new Str(MODULE);
	// string
	glob.iron["upcase"]	= new Fn("upcase",upcase);
	// fileio
	glob.iron["file"]	= new Fn("file",file);
}

