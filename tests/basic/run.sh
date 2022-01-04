#!/bin/sh

set -eu

if test -n "${TRAVIS:-}" -o -n "${GITHUB_ACTIONS:-}"; then
  set -x
fi

cd $(dirname $0)

CC=${CC:-gcc}

$CC a.c -fPIC -shared -o liba.so
$CC tester.c -Wl,--no-as-needed liba.so -D_GNU_SOURCE -ldl

export LD_LIBRARY_PATH=$PWD:${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}

readelf -W --dyn-syms liba.so | grep 'GLOBAL.*foo'
./a.out | grep "Found foo"

../../bin/sym-hider liba.so foo
readelf -W --dyn-syms liba.so | grep 'LOCAL.*foo'
! ./a.out | grep "Found foo"
