INC        = ./include
SRC        = src
OBJ        = objs
SRCS       = $(SRC)/*.cc
TEMP       = $(addprefix $(OBJ)/,$(notdir $(wildcard $(SRC)/*.cc)))
OBJS       = $(TEMP:.cc=.o)
CCC        ?= clang++
CCCFLAGS   = -g -Wall -Wextra -Werror -Wunused-parameter -Wshadow -march=native -O3 -ftrapv \
             -pedantic -I$(INC) \
             -DETC_DIR='"/usr/local/var/lib/tuelib"'
ifeq ($(CCC),clang++)
  ifeq ("$(wildcard /etc/centos-release)","") # Not on CentOS!
    CCCFLAGS += -std=gnu++11 -Wno-vla-extension -Wno-c++1y-extensions -Wno-c++1z-extensions
  else
    CCCFLAGS += -std=gnu++11 -Wno-vla-extension -Wno-c++1y-extensions
  endif
else
  CCCFLAGS += -std=gnu++14
endif
MAKE_DEPS=iViaCore-mkdep

.PHONY: clean
.PRECIOUS: $(OBJ)/%.o

# Rules for building:
$(OBJ)/%.o: $(SRC)/%.cc Makefile
	@echo "Compiling $<..."
	@$(CCC) $(CCCFLAGS) $< -c -o $@

all: /usr/local/bin/iViaCore-mkdep .deps libnyaa.a

libnyaa.a: $(OBJS) Makefile
	@echo "Generating $@..."
	@ar crs $@ $^

/usr/local/bin/iViaCore-mkdep:
	$(MAKE) -C mkdep install 

-include .deps
.deps: $(SRCS) $(INC)/*.h Makefile
	$(MAKE_DEPS) -I $(INC) $(SRCS)

clean:
	rm -f *~ $(OBJS) *.a
	$(MAKE) -C mkdep clean

