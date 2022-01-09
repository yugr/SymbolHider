/*
 * Copyright 2022 Yury Gribov
 * 
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

#include "hider.h"
#include "common.h"

#include <getopt.h>
#include <libgen.h>

#include <stdio.h>
#include <string.h>

int verbose;
int warn = 1;

static void usage(const char *prog) {
  printf("\
Usage: %s [OPT]... SOFILE SYM...\n\
Hide symbols in shared library.\n\
\n\
Options:\n\
  -o OFILE, --output OFILE   Output result to another file.\n\
  --hide                     Hide symbols (default).\n\
  --unhide                   Un-hide symbols.\n\
  -h, --help                 Print this help and exit.\n\
  -v, --verbose              Enable debug prints.\n\
  -w, --no-warnings          Disable warnings.\n\
", prog);
  exit(0);
}

int main(int argc, char *argv[]) {
  const char *me = basename((char *)argv[0]);

  const char *out_file = NULL;
  int hide = 1;
  while (1) {
    static struct option long_opts[] = {
      {"help", no_argument, 0, 'h'},
#define OPT_HIDE 1
      {"hide", no_argument, 0, OPT_HIDE},
      {"no-warnings", no_argument, 0, 'w'},
      {"output", required_argument, 0, 'o'},
#define OPT_UNHIDE 2
      {"unhide", no_argument, 0, OPT_UNHIDE},
      {"verbose", no_argument, 0, 'v'},
    };

    int opt_index = 0;
    int c = getopt_long(argc, argv, "ho:vw", long_opts, &opt_index);

    if (c == -1)
      break;

    switch (c) {
    case 'o':
      out_file = optarg;
      break;
    case 'v':
      ++verbose;
      break;
    case 'h':
      usage(me);
      break;
    case OPT_HIDE:
      hide = 1;
      break;
    case OPT_UNHIDE:
      hide = 0;
      break;
    case 'w':
      warn = 0;
      break;
    default:
      abort();
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "error: ELF file not specified in command line...\n");
    return 0;
  }

  const char *file = argv[optind++];

  if (optind >= argc) {
    fprintf(stderr, "error: no symbols specified in command line...\n");
    return 0;
  }

  size_t nsyms = argc - optind;
  const char **syms = malloc(sizeof(const char *) * nsyms);
  memcpy(syms, argv + optind, sizeof(const char *) * nsyms);

  if (!out_file)
    out_file = file;

  hide_symbols(file, out_file, syms, nsyms, hide);

  free(syms);

  return 0;
}
