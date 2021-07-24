# Compiler prefix, in case your default compiler does not implement all C++11 features:
#CROSS = /opt/toolchain/x86_64-pc-linux-gnu-gcc-4.7.0/bin/x86_64-pc-linux-gnu-

# HINT: g++ -Q -O2 --help=optimizers
OPTIMIZER    = -Os

CC           = $(CROSS)gcc
CXX          = $(CROSS)g++
SIZE         = size -d
RM           = rm -f

SRC_DIRS     = .
INCLUDE      = -I ../../include

SRCS         = $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS         = $(SRCS:.cpp=.o)
DEPENDS      = $(OBJS:.o=.d)

EXE          = $(SRCS:.cpp=)


#------------------------------------------------------------------------------
# flags
#

FLAGS       += $(INCLUDE)
FLAGS       += -MMD

CXXFLAGS     = $(FLAGS)
CXXFLAGS    += $(OPTIMIZER)
CXXFLAGS    += -std=c++11
CXXFLAGS    += -fno-exceptions
CXXFLAGS    += -fno-rtti

CXXFLAGS    += -Wall -Wextra
CXXFLAGS    += -Wctor-dtor-privacy
CXXFLAGS    += -Wcast-align -Wpointer-arith -Wredundant-decls
CXXFLAGS    += -Wshadow -Wcast-qual -Wcast-align -pedantic

# Produce debugging information (for use with gdb)
#OPTIMIZER  = -Og
#FLAGS     += -g

# Use LLVM
#CXX = $(CROSS)clang++
#CXXFLAGS  += -stdlib=libc++
#LDFLAGS   += -lc++


.PHONY: all clean

all: $(EXE)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	$(SIZE) $@

clean:
	$(RM) *.d
	$(RM) $(EXE)


-include $(DEPENDS)
