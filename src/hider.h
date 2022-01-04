/*
 * Copyright 2022 Yury Gribov
 * 
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

#ifndef HIDER_H
#define HIDER_H

#include <stdlib.h>

void hide_symbols(const char *file, const char *out_file,
                  const char **syms, size_t nsyms, int hide_or_unhide);

#endif
