all: intelhextest
 
# $@ matches the target; $< matches the first dependent
intelhextest: intelhextest.o intelhexclass.o
	g++ -o $@ intelhextest.o intelhexclass.o

intelhextest.o: intelhextest.cpp
	g++ -c $<

intelhexclass.o: ../intelhex_class/intelhexclass.cpp
	g++ -c $<
     
clean:
	rm intelhextest intelhextest.o intelhexclass.o
