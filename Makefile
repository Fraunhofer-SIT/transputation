# Debug flags
DEBUGFLAGS := -D_DEBUG -g

# Compile flags
CXXFLAGS := -Wall -Wextra -O3 -Werror -std=c++11 -pedantic -pthread -fPIC -Wno-strict-aliasing
LDFLAGS := -lpthread -lm -fPIC -L.

# Source dir
SRCDIR = src

# Install prefix
PREFIX := /usr/local

# library name
LIB_NAME = transputation

# Get Unix name
UNAME != uname
UNAME ?= $(shell uname)

# set prefixes
INC_PREFIX = $(PREFIX)/include
LIB_PREFIX = $(PREFIX)/lib

# Make GNU make compatible with BSD make
.ALLSRC ?= $^
.TARGET ?= $@

# dylib for macOS otherwise so
DYLIB_SUFFIX != test $(UNAME) = Darwin && echo dylib || echo so
DYLIB_SUFFIX ?= $(shell test $(UNAME) = Darwin && echo dylib || echo so)

# just for completeness the suffix for static libraries
STLIB_SUFFIX := a

# set the install command
INSTALL ?= install

# nicer output
STATUS_ != test `tput colors` -gt 0 && \
	echo "printf \"  [1;36m%-9s[0m [1m%s[0m\\\\n\""  || \
	echo "printf \"  %-9s%s\\\\n\""
STATUS_ ?= $(shell test `tput colors` -gt 0 && \
	echo "printf \"  [1;36m%-9s[0m [1m%s[0m\\\\n\""  || \
	echo "printf \"  %-9s%s\\\\n\"")
STATUS = @$(STATUS_)

# how to detect main files
MAIN = "main([^)]"

# header files
HEADERS != find . -name '*.h'
HEADERS ?= $(shell find . -name '*.h')

# source files
CSRCS != grep -RlL --include="*.c" $(MAIN) $(SRCDIR)/* || echo ""
CSRCS ?= $(shell grep -RlL --include="*.c" $(MAIN) $(SRCDIR)/* )
CCSRCS != grep -RlL --include="*.cc" $(MAIN) $(SRCDIR)/* || echo ""
CCSRCS ?= $(shell grep -RlL --include="*.cc" $(MAIN) $(SRCDIR)/* )
CPPSRCS != grep -RlL --include="*.cpp" $(MAIN) $(SRCDIR)/* || echo ""
CPPSRCS ?= $(shell grep -RlL --include="*.cpp" $(MAIN) $(SRCDIR)/* )

# targets sources
TARGET_SRCS != grep -Rl --include="*.cc" --include="*.cpp" --include="*.c" \
	$(MAIN) $(SRCDIR)/* || echo ""
TARGET_SRCS ?= $(shell grep -Rl --include="*.cc" --include="*.cpp" \
	--include="*.c" $(MAIN) $(SRCDIR)/* )

# object files
OBJS := $(CSRCS:.c=.o) $(CCSRCS:.cc=.o) $(CPPSRCS:.cpp=.o)
TARGET_OBJS := $(TARGET_SRCS:.cc=.o)

# targets
TARGETS = $(TARGET_SRCS:.cc=)
LIB_TARGETS != test $(LIB_NAME) && \
	echo $(SRCDIR)/lib$(LIB_NAME).a $(SRCDIR)/lib$(LIB_NAME).$(DYLIB_SUFFIX) \
	|| echo ""
LIB_TARGETS ?= $(shell test $(LIB_NAME) && \
	echo $(SRCDIR)/lib$(LIB_NAME).a $(SRCDIR)/lib$(LIB_NAME).$(DYLIB_SUFFIX))

# enable debug options when DEBUG is set
_DEBUGFLAGS != test $(DEBUG) && echo $(DEBUGFLAGS) || echo ""
_DEBUGFLAGS := $(shell test $(DEBUG) && echo $(DEBUGFLAGS))
CXXFLAGS += $(_DEBUGFLAGS)

.SUFFIXES: .cc .c .cpp .o .a .dylib .so
.PHONY: all clean test

all: $(TARGETS) $(LIB_TARGETS)
	@echo done.

.c.o:
	$(CC) $(INC) -c $(CFLAGS) -o $(.TARGET) $(.ALLSRC)

.cc.o:
	$(CXX) $(INC) -c $(CXXFLAGS) -o $(.TARGET) $(.ALLSRC)

.cpp.o:
	$(CXX) $(INC) -c $(CXXFLAGS) -o $(.TARGET) $(.ALLSRC)

$(SRCDIR)/lib$(LIB_NAME).dylib: $(OBJS)
	$(CXX) -dynamiclib -o $(.TARGET) $(OBJS)

$(SRCDIR)/lib$(LIB_NAME).so: $(OBJS)
	$(CXX) -shared -o $(.TARGET) $(LDFLAGS) $(OBJS)

$(SRCDIR)/lib$(LIB_NAME).a: $(OBJS)
	$(AR) rcs $(.TARGET) $(OBJS)

$(TARGETS): $(TARGET_OBJS) $(OBJS)
	$(CXX) $(INC) -o $(.TARGET) $(.TARGET).o $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGETS) $(TARGETS:=.o) $(LIB_TARGETS)

test: $(TARGETS)
	for test in tests/*; do\
		$$test || exit 1;\
	done

install: $(LIB_TARGETS)
	for file in `find $(SRCDIR) -name '*.h'`; do\
		target=$(INC_PREFIX)/$(LIB_NAME)/`echo $${file} | sed 's#$(SRCDIR)/*##g'`;\
		install -d -m644 `dirname $${target}`;\
		install -c -m644 $${file} $${target};\
	done
	for file in $(LIB_TARGETS); do\
		target=$(LIB_PREFIX)/`echo $${file} | sed 's#$(SRCDIR)/*##g'`;\
		install -c -m644 $${file} $${target};\
	done

uninstall:
	test $(INC_PREFIX) && test $(LIB_NAME) && rm -rf $(INC_PREFIX)/$(LIB_NAME)
	test $(LIB_PREFIX) && test $(LIB_NAME) &&\
		rm -rf $(LIB_PREFIX)/lib$(LIB_NAME).*

