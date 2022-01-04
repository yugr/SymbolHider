/*
 * Copyright 2022 Yury Gribov
 * 
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

#include <dlfcn.h>

#include <stdio.h>

int main() {
  void *ptr = dlsym(RTLD_DEFAULT, "foo");
  if (ptr) {
    puts("Found foo");
  }
  return 0;
}

