#
# Makefile for the Free Software Foundations g++ compiler
#
CC=g++
CFLAGS=-g -Wall
CINC=-I../intelhex_class
CNAME=ihIntegrity
DNAME=ihRefactor
ENAME=ihDiff
SNAME=../intelhex_class/intelhexclass

$(CNAME): $(CNAME).cpp $(SNAME).cpp
	$(CC) $(CFLAGS) $(CINC) -o$(CNAME) $(CNAME).cpp $(SNAME).cpp

$(DNAME): $(DNAME).cpp $(SNAME).cpp
	$(CC) $(CFLAGS) $(CINC) -o$(DNAME) $(DNAME).cpp $(SNAME).cpp
	
$(ENAME): $(ENAME).cpp $(SNAME).cpp
	$(CC) $(CFLAGS) $(CINC) -o$(ENAME) $(ENAME).cpp $(SNAME).cpp

clean:
	rm $(CNAME)
	rm $(DNAME)
	rm $(ENAME)
	
