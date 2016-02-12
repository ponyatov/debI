log.log: src.src ./exe.exe
	./exe.exe < src.src > log.log
C = cpp.cpp ypp.tab.cpp lex.yy.c
H = hpp.hpp ypp.tab.hpp
./exe.exe: $(C) $(H)
	$(CXX) $(F) -o $@ $(C)
ypp.tab.cpp: ypp.ypp
	bison $<
lex.yy.c: ypp.ypp	
	flex $<
