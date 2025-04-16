# Hey Emacs, this is a -*- makefile -*-
# Copyright (C) 2013 Y.Okamura
# https://gist.github.com/2343121
# MIT License

# Target Executable file
TARGET = slurminfo

# C source code
SRC = slurminfo.c nodes.c partitions.c common.c jobs.c slurmd.c reservations.c term.c spawn.c assoc.c slurm_users.c

# CPP source code
CXXSRC = summary.cpp summary_common.cpp summary_job.cpp summary_node.cpp summary_partitions.cpp summary_users.cpp summary_account.cpp table.cpp

# Common Flags
CPPFLAGS += -Wall -Wextra -pipe -D_POSIX_C_SOURCE=200809 -Iincludes -I${INSTALLDIR_BOOST}/include
COMMON_FLAGS += 

# C compiler
CC = gcc
CFLAGS += -std=c99

# C++ compiler
CXX = g++
CXXFLAGS += -std=c++11

# AR
AR = ar
LIB = lib$(TARGET).a
ARFLAGS = urcv

# Linker
LD = gcc
LDFLAGS += 
LIBS += -lslurm -lm -lstdc++

# Windows Resource Compiler
WINDRES = windres

# Windows Executable file's extension
ifdef WINDIR
  EXE = $(TARGET).exe
else

# Unix like OS
UNAME = $(strip $(shell uname))
CPPFLAGS += 
LDFLAGS  +=
LIBS += 
EXE = $(TARGET)

ifeq ($(UNAME), Darwin) # Mac OS X
  LDFLAGS += 
endif
ifeq ($(UNAME), Linux) # Linux
  LDFLAGS += 
endif

endif


# For Emacs
ifdef EMACS
ifeq ($(CC),clang)
  CPPFLAGS += -fno-color-diagnostics
endif
endif

# Debug Options
ifdef RELEASE
  CPPFLAGS += -Os
else
  CPPFLAGS += -g -O0
endif

# copy commong flags
CPPFLAGS += $(COMMON_FLAGS)
LDFLAGS += $(COMMON_FLAGS)

# generate dependence file
OBJDIR = ./objs
CPPFLAGS += -MMD -MP -MF $@.d
OBJ += $(SRC:%.c=$(OBJDIR)/%.o) $(CXXSRC:%.cpp=$(OBJDIR)/%.o)
DEPENDS = $(SRC:%.c=$(OBJDIR)/%.o.d) $(CXXSRC:%.cpp=$(OBJDIR)/%.o.d)

# Add target to build library
all:exe

zip:distfiles
	zip -r slurminfo-`cat version.txt`.zip slurminfo-`cat version.txt`

tar:distfiles
	tar czf slurminfo-`cat version.txt`.tar.gz slurminfo-`cat version.txt`

distfiles:exe version.txt
	-rm -rf slurminfo-`cat version.txt`
	mkdir -p slurminfo-`cat version.txt`
	install -m 755 -d slurminfo-`cat version.txt`/bin
	install -m 755 slurminfo slurminfo-`cat version.txt`/bin
	install -m 644 README.md slurminfo-`cat version.txt`
	install -m 644 LICENSE slurminfo-`cat version.txt`

exe:$(EXE)

$(EXE):version.h $(OBJ) $(DEPEND_TARGET)
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	strip $@

$(OBJDIR)/%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
$(OBJDIR)/%.res : %.rc
	$(WINDRES) $< -O coff -o $@ $(RESFLAGS)

version.h:
	./makeversion.sh

version.txt:
	./makeversion.sh

clean:
	-rm $(OBJ)
	-rm $(DEPENDS)
	-rmdir $(OBJDIR)
	-rm $(EXE)
	-rm version.h
	-rm version.txt
#   -rm -r $(OBJDIR)

.PHONY:clean all exe lib zip tar distfiles

-include $(shell mkdir $(OBJDIR) 2>/dev/null) $(wildcard $(OBJDIR)/*.d)
