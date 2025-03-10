# This file is part of mfaktc (mfakto).
# Copyright (C) 2009 - 2011, 2014  Oliver Weihe (o.weihe@t-online.de)
#                                  Bertram Franz (bertramf@gmx.net)
#
# mfaktc (mfakto) is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# mfaktc (mfakto) is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with mfaktc (mfakto).  If not, see <http://www.gnu.org/licenses/>.
#
#
# Run "make" to compile mfakto. You can pass parameters on the make command
# line to override defaults. For example, use "make bitness=32 static=yes"
# to compile a 32-bit binary and link statically.
#
# MacOS Users may see an "out of sync" warning when compiling mfakto. Although
# harmless, the warning can be silenced by running the following command prior
# to compilation:
#
# export SDKROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk


ARCH := $(shell uname -m)

# check whether to build 32-bit version
ifdef bitness
  ifeq ($(bitness), 32)
    ARCH := x86
    BITS := -m32
  endif
endif

# Check if static linking is requested
ifdef static
  ifeq ($(static), yes)
    STATIC := -static
  endif
endif

# OS commands
RM = rm -f
INSTALL = install

# MacOS specific flags
OS = $(shell uname -s)
ifeq ($(OS), Darwin)
  ARCHFLAGS = -arch $(ARCH)
  OPENCL_LIB = -framework OpenCL
else
  ARCHFLAGS =
  OPENCL_LIB = -lOpenCL
endif

# where is the OpenCL SDK installed?
AMD_APP_DIR = /opt/rocm/opencl
AMD_APP_INCLUDE = -I$(AMD_APP_DIR)/include
AMD_APP_LIB = -L$(AMD_APP_DIR)/lib/$(ARCH)

# Change needed for compilation with amdgpu-pro
# AMD_APP_DIR = /opt/amdgpu-pro/opencl

# compiler settings for C and C++ files
CC = gcc
CPP = g++
CFLAGS = $(ARCHFLAGS) $(BITS) -Wall $(OPTIMIZE_FLAG) $(AMD_APP_INCLUDE)
CPPFLAGS =

# Linker
LD = $(CPP)
LDFLAGS = $(ARCHFLAGS) $(BITS) $(STATIC) $(OPTIMIZE_FLAG) $(AMD_APP_LIB) $(OPENCL_LIB)

CC_VERSION = $(shell $(CC) --version)

# optimize or debug
ifneq (, $(findstring gcc, $(CC_VERSION)))
  OPTIMIZE_FLAG = -O3 -funroll-loops -ffast-math -finline-functions -frerun-loop-opt -fgcse-sm -fgcse-las -flto
  CFLAGS_EXTRA_SIEVE = -funroll-all-loops -funsafe-loop-optimizations -fira-region=all -fsched-spec-load -fsched-stalled-insns=10 -fsched-stalled-insns-dep=10 -fno-align-labels
else ifneq (, $(findstring clang, $(CC_VERSION)))
  OPTIMIZE_FLAG = -O3 -funroll-loops -ffast-math -finline-functions -flto
  CFLAGS_EXTRA_SIEVE =
else
  OPTIMIZE_FLAG = -O3
  CFLAGS_EXTRA_SIEVE =
endif

##############################################################################

CSRC = sieve.c timer.c parse.c read_config.c mfaktc.c checkpoint.c \
	signal_handler.c filelocking.c output.c

# CLSRC = barrett15.cl  barrett.cl  common.cl  gpusieve.cl  mfakto_Kernels.cl  montgomery.cl  mul24.cl

COBJS = $(CSRC:.c=.o) mfakto.o gpusieve.o perftest.o menu.o kbhit.o

##############################################################################

ALL_TARGETS = ../mfakto ../barrett15.cl ../barrett.cl ../common.cl \
	      ../gpusieve.cl ../mfakto_Kernels.cl ../montgomery.cl \
	      ../mul24.cl ../datatypes.h ../tf_debug.h ../mfakto.ini

all: $(ALL_TARGETS)

../mfakto : $(COBJS)
	$(LD) $^ $(LDFLAGS) -o $@

clean:
	$(RM) *.o *~ depend

sieve.o : sieve.c
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA_SIEVE) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.cpp
	$(CPP) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

../%.cl : %.cl
	$(INSTALL) -m 444 $< ..

../%.h : %.h
	$(INSTALL) -m 444 $< ..

../%.ini : %.ini
	$(INSTALL) -m 644 $< ..

.PHONY: all clean


##############################################################################

depend: $(CSRC)
	$(CC) -MM $(CFLAGS) $(CSRC) > $@.tmp
	mv $@.tmp $@

ifneq ($(MAKECMDGOALS), clean)
ifneq (, $(wildcard depend))
include depend
endif
endif
