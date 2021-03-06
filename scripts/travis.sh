#!/bin/sh

# The MIT License (MIT)
# 
# Copyright (c) 2022 Yury Gribov
# 
# Use of this source code is governed by The MIT License (MIT)
# that can be found in the LICENSE.txt file.

set -eu
set -x

cd $(dirname $0)/..

# Build

make clean all

# Run tests

if test -n "${VALGRIND:-}"; then
  cp -r bin bin-real
  for f in $(find bin -type f -a -executable); do
    cat > $f <<EOF
#!/bin/sh
valgrind -q --error-exitcode=1 $PWD/bin-real/$(basename $f) "\$@"
EOF
    chmod +x $f
  done
fi

export ASAN_OPTIONS='detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1:strict_string_checks=1'

make check

if test -n "${COVERAGE:-}"; then
  # Generate coverage
  mv bin/*.gc[dn][ao] src
  gcov src/*.gcno
  # Upload coverage
  curl --retry 5 -s https://codecov.io/bash > codecov.bash
  bash codecov.bash -Z -X gcov
fi
