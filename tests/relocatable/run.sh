#!/bin/sh

set -eu

if test -n "${TRAVIS:-}" -o -n "${GITHUB_ACTIONS:-}"; then
  set -x
fi

cd $(dirname $0)

CC=${CC:-gcc}

$CC a.c -c -fPIC

readelf -sW a.o | grep 'DEFAULT.*foo'

../../bin/sym-hider a.o foo
readelf -sW a.o | grep 'HIDDEN.*foo'
