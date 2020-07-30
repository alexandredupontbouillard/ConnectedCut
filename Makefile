SYSTEM     = x86-64_sles10_4.1
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------


PATHTUTOMIP =/opt/ibm/ILOG
CPLEXDIR      = $(PATHTUTOMIP)/CPLEX_Studio1210/cplex
CONCERTDIR    = $(PATHTUTOMIP)/CPLEX_Studio1210/concert
LEMONDIR = usr/local
# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -DUSING_CPLEX -ldl

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

LEMONBINDIR = $(LEMONDIR)/bin/lemon-0.x-to-1.x.sh
LEMONLIBDIR = $(LEMONDIR)/lib/libemon.a
# CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -lpthread -lemon
CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -lpthread -ldl -L$(LEMONLIBDIR)


CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
LEMONINCDIR = $(LEMONDIR)/include/lemon

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I$(LEMONINCDIR)

all:	MCCP_Tree_Form formulationFlot w1w2 hojny_flot 

MCCP_Tree_Form: MCCP_Tree_Form.o Graph.o heap2k.o heap2k.h Graph.h 
	$(CCC) $(CCFLAGS) MCCP_Tree_Form.o heap2k.o Graph.o -o MCCP_Tree_Form $(CCLNFLAGS)

MCCP_Tree_Form.o: MCCP_Tree_Form.cpp Graph.h
	$(CCC) -c $(CCFLAGS) MCCP_Tree_Form.cpp -o MCCP_Tree_Form.o

w1w2: w1w2.o Graph.o heap2k.o heap2k.h Graph.h minSeparator.o statStruct.o statStruct.h ModelMinSeparator.o ModelMinSeparator.h
	$(CCC) $(CCFLAGS) w1w2.o   heap2k.o Graph.o statStruct.o minSeparator.o  ModelMinSeparator.o -o w1w2 $(CCLNFLAGS)

w1w2.o: w1w2.cpp Graph.h minSeparator.h statStruct.h ModelMinSeparator.h 
	$(CCC) -c $(CCFLAGS) w1w2.cpp -o w1w2.o

ModelMinSeparator.o: ModelMinSeparator.cpp ModelMinSeparator.h Graph.h 
	$(CCC) -c $(CCFLAGS) ModelMinSeparator.cpp -o ModelMinSeparator.o

minSeparator.o : minSeparator.cpp Graph.h minSeparator.h ModelMinSeparator.h
	$(CCC) -c  $(CCFLAGS) $(CCLNDIRS)  minSeparator.cpp -o minSeparator.o

formulationFlot:formulationFlot.o Graph.o heap2k.o heap2k.h Graph.h  statStruct.o statStruct.h
	$(CCC) $(CCFLAGS) formulationFlot.o heap2k.o Graph.o statStruct.o -o formulationFlot $(CCLNFLAGS)

formulationFlot.o: formulationFlot.cpp Graph.h statStruct.h
	$(CCC) -c $(CCFLAGS) formulationFlot.cpp -o formulationFlot.o

hojny_flot:hojny_flot.o Graph.o heap2k.o heap2k.h Graph.h statStruct.o statStruct.h
	$(CCC) $(CCFLAGS) hojny_flot.o heap2k.o Graph.o statStruct.o -o hojny_flot $(CCLNFLAGS)

hojny_flot.o: hojny_flot.cpp Graph.h statStruct.h
	$(CCC) -c $(CCFLAGS) hojny_flot.cpp -o hojny_flot.o

heap2k.o: heap2k.cpp heap2k.h Graph.h
	$(CCC) -c $(CCFLAGS) heap2k.cpp -o heap2k.o

Graph.o: Graph.cpp Graph.h heap2k.h
	$(CCC) -c $(CCFLAGS) Graph.cpp -o Graph.o 

statStruct.o: statStruct.cpp statStruct.h
	$(CCC) -c $(CCFLAGS) statStruct.cpp -o statStruct.o 

clean:
	rm -f *.o MCCP_Tree_Form
