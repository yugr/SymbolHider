#!/bin/sh

# Copyright 2022 Yury Gribov
# 
# Use of this source code is governed by MIT license that can be
# found in the LICENSE.txt file.
#
# Wrapper for ld which automatically calls implib-gen.
# To enable, add it to PATH.
# Flags are passed via env variable IMPLIBSO_LD_OPTIONS.

# Runs hider on all installed libraries

set -eu

cd $(dirname $0)/..

if test "${1:-}" != noscan; then
  rm -f libs.txt
  for lib in $(find /lib /usr/lib /usr/local/lib -name \*.so\*); do
    if file $lib | grep -q 'ELF .* LSB shared object'; then
      echo $lib >> libs.txt
    fi
  done
fi

tmp=$(mktemp)
trap "rm -f $tmp" EXIT INT

for lib in $(cat libs.txt); do
  echo "Checking $lib..."
  first_sym=$(readelf --dyn-syms -W $lib | awk '/DEFAULT/{if ($8) print $8}'  | head -n1)
  test -n "$first_sym" || continue
  bin/sym-hider $lib $first_sym -w -o $tmp || true
  bin/sym-hider $tmp $first_sym -w --unhide || true
done
