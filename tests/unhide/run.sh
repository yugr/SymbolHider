#!/bin/sh

set -eu

if test -n "${TRAVIS:-}" -o -n "${GITHUB_ACTIONS:-}"; then
  set -x
fi

cd $(dirname $0)

CC=${CC:-gcc}

$CC a.c -c -fPIC -fvisibility=hidden

readelf -sW a.o | grep 'HIDDEN.*foo'

../../bin/sym-hider --unhide a.o foo
readelf -sW a.o | grep 'DEFAULT.*foo'
