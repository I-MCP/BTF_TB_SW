# $Id: Makefile,v 1.8 2010/08/25 08:36:35 rahatlou Exp $
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)

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

#CINTFILE  = $(PACKAGE)Cint.cc
#CINTOBJ   = $(PACKAGE)Cint.o

LIBFILE	 =  $(LIBDIR)/lib$(PACKAGE).a
SHLIBFILE = $(LIBDIR)/lib$(PACKAGE).so

OBJDIR=$(LIBDIR)/objects

INCLUDES += -I. -I./include/ -I$(ROOTSYS)/include  
ROOTSYS  ?= ERROR_RootSysIsNotDefined

EXTRALIBS  := -L$(LIBDIR) -l$(PACKAGE)

HHLIST=$(filter-out $(PACKAGE)_LinkDef.h,$(wildcard *.h))

# CC files excluding the binaries
CCFILES=$(filter-out $(BINFILES),$(wildcard src/*.cc))

# List of all object files to build
OLIST=$(patsubst src/%.cc,$(OBJDIR)/%.o,$(CCFILES))

# Implicit rule to compile all classes
$(OBJDIR)/%.o : src/%.cc
	@echo "Compiling $<"
	@mkdir -p $(OBJDIR)
	$(CC) $(CCFLAGS) -c $< -o $@ $(INCLUDES)


#$(OBJDIR)/$(CINTOBJ): $(HHLLIST) 
#	@mkdir -p $(OBJDIR)
#	@cp *.hh $(INCDIR)
#	@echo "Running rootcint"
#	$(ROOTSYS)/bin/rootcint -f $(CINTFILE) -c $(INCLUDES) $(HHLIST) $(PACKAGE)_LinkDef.h
#	@echo "Compiling $(CINTFILE)"
#	@$(CC) $(INCLUDES) $(CCFLAGS) -c $(CINTFILE) -o $(OBJDIR)/$(CINTOBJ) 
#

$(PROGRAMS) : $(SHLIBFILE) 
	mkdir -p $(WORKDIR)
	@echo "Linking $@"
	$(CC) $(CCFLAGS) $(INCLUDES) $(ROOTLIBS) $(EXTRALIBS) -o $(WORKDIR)/$@ $(patsubst %,src/%.cpp,$@)
#	$(CC) $(CCFLAGS) $(INCLUDES) -o $(WORKDIR)/$@ $(patsubst %,%.cpp,$@) $(ROOTLIBS) $(EXTRALIBS) 

default : $(PROGRAMS) 
	@echo "Programs are $(PROGRAMS)"

all : $(PROGRAMS) 

# Rule to combine objects into a library
$(LIBFILE): $(OLIST) 
#$(patsubst %.cc,%.o,$(OBJDIR)/$(CINTFILE))
	@echo "Making $(LIBFILE)"
	@rm -f $(LIBFILE) 
#	@ar q $(LIBFILE) $(OLIST) $(addprefix $(OBJDIR)/,$(OLIST) $(patsubst %.cc,%.o,$(CINTFILE)))
	@ar q $(LIBFILE) $(OLIST) $(addprefix $(OBJDIR)/,$(OLIST))
	@ranlib $(LIBFILE)

# Rule to combine objects into a shared library
$(SHLIBFILE): $(OLIST) 
#$(patsubst %.cc,%.o,$(OBJDIR)/$(CINTFILE))
	@echo "Making $(SHLIBFILE)"
	@rm -f $(SHLIBFILE)
#	$(CC) $(CCFLAGS) $(OLIST) $(addprefix $(OBJDIR)/,$(patsubst %.cc,%.o,$(CINTFILE))) -shared -o $(SHLIBFILE) 
	$(CC) $(CCFLAGS) $(OLIST) -shared -o $(SHLIBFILE) 

# Useful build targets
lib: $(LIBFILE)

shlib:	$(SHLIBFILE)

clean:
#	@rm -Rf $(WORKDIR)/* $(LIBDIR)/* $(CINTFILE) $(patsubst %.cc,%.h,$(CINTFILE))	
	@rm -Rf $(WORKDIR)/* $(LIBDIR)/* 
	@rm -f $(LIBFILE)
	@rm -Rf *.o

veryclean:
	rm -Rf $(WORKDIR) 

#.PHONY : shlib lib default clean
