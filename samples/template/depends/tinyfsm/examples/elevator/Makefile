# Compiler prefix, in case your default compiler does not implement all C++11 features:
#CROSS = /opt/toolchain/x86_64-pc-linux-gnu-gcc-4.7.0/bin/x86_64-pc-linux-gnu-

PROJECT      = elevator

# HINT: g++ -Q -O2 --help=optimizers
OPTIMIZER    = -Os

CC           = $(CROSS)gcc
CXX          = $(CROSS)g++
AS           = $(CROSS)gcc -x assembler-with-cpp
LD           = $(CROSS)g++
OBJCOPY      = $(CROSS)objcopy
OBJDUMP      = $(CROSS)objdump
SIZE         = size -d
RM           = rm -f
RM_R         = rm -rf
CP           = cp
MKDIR_P      = mkdir -p
DOXYGEN      = doxygen


SRC_DIRS     = .
INCLUDE      = -I ../../include

SRCS         = $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS         = $(SRCS:.cpp=.o)
DEPENDS      = $(OBJS:.o=.d)

EXE          = $(PROJECT)
MAP          = $(PROJECT).map


#------------------------------------------------------------------------------
# flags
#

# commmon flags propagated to CFLAGS, CXXFLAGS, ASFLAGS (not LDFLAGS)
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

LDFLAGS     += -fno-exceptions
LDFLAGS     += -fno-rtti

# Produce debugging information (for use with gdb)
#OPTIMIZER  = -Og
#FLAGS     += -g

# Use LLVM
#CXX = $(CROSS)clang++
#CXXFLAGS  += -stdlib=libc++
#LDFLAGS   += -lc++

# Enable link-time optimizer
#CXXFLAGS += -flto
#LDFLAGS  += -flto

# Strip dead code (enable garbage collection)
#OPTIMIZER += -ffunction-sections -fdata-sections
#LDFLAGS   += -Wl,$(if $(shell ld -v | grep GNU),--gc-sections,-dead_strip)

# Enable automatic template instantiation at link time
#CXXFLAGS += -frepo
#LDFLAGS  += -frepo

# Create link map file
#LDFLAGS  += -Wl,-Map="$(MAP)",--cref


.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXE)
	$(SIZE) $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	$(RM) *.o
	$(RM) *.d
	$(RM) $(EXE)


-include $(DEPENDS)
