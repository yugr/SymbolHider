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

make check

if test -n "${COVERAGE:-}"; then
  # Generate coverage
  mv bin/*.gc[dn][ao] src
  gcov src/*.gcno
  # Upload coverage
  curl --retry 5 -s https://codecov.io/bash > codecov.bash
  bash codecov.bash -Z -X gcov
fi
