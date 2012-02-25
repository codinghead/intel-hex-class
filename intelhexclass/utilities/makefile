#
# Makefile for the Free Software Foundations g++ compiler
#
CC=g++
CFLAGS=-g -Wall
CINC=-I../intelhex_class
CNAME=ihIntegrity
SNAME=../intelhex_class/intelhexclass

$(CNAME): $(CNAME).cpp $(SNAME).cpp
	$(CC) $(CFLAGS) $(CINC) -o$(CNAME) $(CNAME).cpp $(SNAME).cpp

clean:
	rm $(CNAME)
