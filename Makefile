# Copyright 2022 Yury Gribov
# 
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE.txt file.

CC ?= gcc
DESTDIR ?= /usr/local

CFLAGS = -g -Wall -Wextra -Werror
LDFLAGS = -g -Wl,--warn-common

ifneq (,$(COVERAGE))
  DEBUG = 1
  CFLAGS += --coverage -DNDEBUG
  LDFLAGS += --coverage
endif
ifeq (,$(DEBUG))
  CFLAGS += -O2
  LDFLAGS += -Wl,-O2
else
  CFLAGS += -O0
endif
ifneq (,$(ASAN))
  CFLAGS += -fsanitize=address -fsanitize-address-use-after-scope -U_FORTIFY_SOURCE -fno-common -D_GLIBCXX_DEBUG -D_GLIBCXX_SANITIZE_VECTOR
  LDFLAGS += -fsanitize=address
endif
ifneq (,$(UBSAN))
  ifneq (,$(shell $(CXX) --version | grep clang))
    # Isan is clang-only...
    CFLAGS += -fsanitize=undefined,integer -fno-sanitize-recover=undefined,integer
    LDFLAGS += -fsanitize=undefined,integer -fno-sanitize-recover=undefined,integer
  else
    CFLAGS += -fsanitize=undefined -fno-sanitize-recover=undefined
    LDFLAGS += -fsanitize=undefined -fno-sanitize-recover=undefined
  endif
endif

OBJS = bin/hider.o bin/driver.o

$(shell mkdir -p bin)

all: bin/sym-hider

check:
	tests/basic/run.sh
	tests/relocatable/run.sh
	tests/unhide/run.sh
	@echo SUCCESS

bin/sym-hider: $(OBJS) Makefile bin/FLAGS
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

bin/%.o: src/%.c Makefile bin/FLAGS
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

bin/FLAGS: FORCE
	if test x"$(CFLAGS) $(CXXFLAGS) $(LDFLAGS)" != x"$$(cat $@)"; then \
		echo "$(CFLAGS) $(CXXFLAGS) $(LDFLAGS)" > $@; \
	fi

clean:
	rm -f bin/*
	find -name \*.gcov -o -name \*.gcno -o -name \*.gcda | xargs rm -rf

install:
	mkdir -p $(DESTDIR)
	install bin/sym-hider $(DESTDIR)/bin

.PHONY: clean all check install FORCE
