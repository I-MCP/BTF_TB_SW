# $Id: Makefile,v 1.8 2010/08/25 08:36:35 rahatlou Exp $
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)

BOOSTINC      = $(shell cd $(CMSSW_BASE); scram tool tag boost INCLUDE)
BOOSTLIB      = $(shell cd $(CMSSW_BASE); scram tool tag boost LIBDIR)
BOOSTLIBS     = $(shell cd $(CMSSW_BASE); scram tool tag boost LIB)

BINFILES = $(wildcard src/*.cpp)

PROGRAMS = $(patsubst src/%.cpp,%,$(BINFILES))

# --- External configuration ---------------------------------
THREADTST     = -D_THREADLIB_ 
CC         = g++  
CCFLAGS    =  $(shell $(ROOTSYS)/bin/root-config --cflags) -fPIC
MFLAGS     = -MM
INCLUDES   =
WORKDIR    = bin
LIBDIR     = lib
PACKAGE = BTFTBSW

CINTFILE  = $(PACKAGE)Cint.cc
CINTOBJ   = $(PACKAGE)Cint.o

LIBFILE	 =  $(LIBDIR)/lib$(PACKAGE).a
SHLIBFILE = $(LIBDIR)/lib$(PACKAGE).so

OBJDIR=$(LIBDIR)/objects

INCLUDES += -I. -I./include/ -I$(ROOTSYS)/include -I$(BOOSTINC)
ROOTSYS  ?= ERROR_RootSysIsNotDefined

EXTRALIBS  := -L$(LIBDIR) -l$(PACKAGE) 

HHLIST=$(filter-out include/$(PACKAGE)_LinkDef.h,$(wildcard include/*.h))

# CC files excluding the binaries
CCFILES=$(filter-out $(BINFILES),$(wildcard src/*.cc))

# List of all object files to build
OLIST=$(filter-out src/$(CINTFILE),$(patsubst src/%.cc,$(OBJDIR)/%.o,$(CCFILES)))

# Implicit rule to compile all classes
$(OBJDIR)/$(CINTOBJ): $(HHLLIST) 
	@mkdir -p $(OBJDIR)
	@echo "Running rootcint"
	$(ROOTSYS)/bin/rootcint -f src/$(CINTFILE) -c $(INCLUDES) $(HHLIST) include/$(PACKAGE)_LinkDef.h
	@echo "Compiling $(CINTFILE)"
	$(CC) $(INCLUDES) $(CCFLAGS) -c src/$(CINTFILE) -o $(OBJDIR)/$(CINTOBJ) 

$(OBJDIR)/%.o: src/%.cc
	@echo "Compiling $<"
	@mkdir -p $(OBJDIR)
	$(CC) $(CCFLAGS) -c $< -o $@ $(INCLUDES)

$(PROGRAMS): shlib
	mkdir -p $(WORKDIR)
	@echo "Linking $@"
	$(CC) $(CCFLAGS) $(INCLUDES) $(ROOTLIBS) $(EXTRALIBS) -o $(WORKDIR)/$@ $(patsubst %,src/%.cpp,$@)

default: all

all: shlib $(PROGRAMS) 

# Rule to combine objects into a library
$(LIBFILE): $(OLIST) $(OBJDIR)/$(CINTOBJ)
	@echo "Making $(LIBFILE)"
	@rm -f $(LIBFILE) 
	@ar q $(LIBFILE) $(OLIST) $(addprefix $(OBJDIR)/,$(OLIST) $(patsubst src/%.cc,%.o,$(CINTFILE)))
	@ranlib $(LIBFILE)

# Rule to combine objects into a shared library
$(SHLIBFILE): $(OLIST) $(OBJDIR)/$(CINTOBJ)
	@echo "Making $(SHLIBFILE)"
	@rm -f $(SHLIBFILE)
	$(CC) $(CCFLAGS) $(ROOTLIBS) $(OLIST) $(OBJDIR)/$(CINTOBJ) -shared -o $(SHLIBFILE) 

# Useful build targets
lib: $(LIBFILE)

shlib:	$(SHLIBFILE)

clean:
	@rm -Rf $(WORKDIR)/* $(LIBDIR)/* $(OBJDIR)/$(CINTOBJ) src/$(CINTFILE) 
	@rm -f $(LIBFILE)
	@rm -Rf *.o

veryclean:
	rm -Rf $(WORKDIR) 

.PHONY : shlib lib default clean
