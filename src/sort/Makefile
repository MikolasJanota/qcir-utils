GENCC=Options.cc
GENHH=Options.hh
## nongenerated sources
ORIGCSRCS = $(filter-out $(GENCC), $(wildcard *.cc) )

## generated and nongenerated sources and objects
CSRCS     = $(ORIGCSRCS) $(GENCC)
YACC     = $(wildcard *.y)
LEX      = $(wildcard *.l)
COBJS    = $(CSRCS:.cc=.o) $(YACC:.y=.tab.o) $(LEX:.l=.o)
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


all:
	make sources
	make qcir_sort

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

sources: $(GENCC) $(GENHH)

./Options.o: Options.cc option_generation
	@echo Compiling: $@
	@$(CXX) $(CFLAGS) -c -o $@ $<

./Options.hh Options.cc: option_generation
	@echo Generating: $@
	mkdir -p generated
	./option_generation  >Options.hh 2>Options.cc

## Clean rule
clean:
	@rm -f qcir_sort qcir_sort.exe $(COBJS) *.tab.[ch] $(GENCC) $(GENHH)


DEPENDS = ${COBJS:.o=.d}
-include ${DEPENDS}
