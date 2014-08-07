CSRCS    = $(wildcard *.cc)
YACC     = $(wildcard *.y)
LEX      = $(wildcard *.l)
COBJS    = $(CSRCS:.cc=.o) $(YACC:.y=.tab.o) $(LEX:.l=.o)
PDFS    = $(NSRCS:.nw=.pdf)
LIBD =
LIBS =
CXX?=g++
CC=g++

ifdef PROF
CFLAGS+=-pg
LNFLAGS+=-pg
endif

ifdef DBG
CFLAGS+=-O0
CFLAGS+=-ggdb
CFLAGS+=-DDBG
MSAT=libd
else
CFLAGS+=-DNDEBUG
CFLAGS+=-O3
MSAT=libr
endif

ifdef NOO
CFLAGS+=-O0
endif

CFLAGS += -Wall -DBUILDDATE='"$(BUILDDATE)"' -DDISTDATE='"$(DISTDATE)"'
CFLAGS += -DCHANGESET='"$(CHANGESET)"' -DRELDATE='"$(RELDATE)"'
#CFLAGS+=-std=c++0x
CFLAGS+=-std=gnu++0x
CFLAGS+=-MMD
LIBS+=-lz

ifdef STATIC
CFLAGS+=-static
LNFLAGS+=-static
endif

.PHONY: all


all: qcir_sort

qcir_sort:  $(COBJS)
	@echo Linking: $@
	@$(CXX) -o $@ $(COBJS) $(LNFLAGS) $(LIBD) $(LIBS)

%.o:	%.cc
	@echo Compiling: $@
	@$(CXX) $(CFLAGS) -c -o $@ $<

%.o:	%.c Makefile
	@echo Compiling: $@
	@$(CXX) $(CFLAGS) -c -o $@ $<

%.tab.h %.tab.c: %.y
	@echo Generating: $*
	@bison $<

%.c:	%.l Makefile
	@echo Generating: $*
	@flex -o$@ $<

main.o: main.cc qcir_parse.tab.h

t:
	./qcir_sort.exe perr.qcir

## Clean rule
clean:
	@rm -f qcir_sort qcir_sort.exe $(COBJS) *.tab.[ch]

DEPENDS = ${COBJS:.o=.d}
-include ${DEPENDS}
