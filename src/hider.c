/*
 * Copyright 2022 Yury Gribov
 * 
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

#include "common.h"
#include "hider.h"

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <elf.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

void hide_symbols(const char *file, const char *out_file,
                  const char **hidden_syms, size_t num_hidden_syms,
                  int hide_or_unhide) {
  const int inplace = !out_file;

  int fd = open(file, inplace ? O_RDWR : O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, PREFIX "failed to open %s: %s\n", file, strerror(errno));
    exit(1);
  }

  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(stderr, PREFIX "failed to determine size of %s: %s\n", file, strerror(errno));
    exit(1);
  }

  const off_t file_size = st.st_size;
  char *start = malloc(file_size);
  ssize_t nread = read(fd, start, file_size);
  if (nread < 0 || nread != file_size) {
    fprintf(stderr, PREFIX "failed to read contents of %s\n", file);
    exit(1);
  }

  close(fd);

  // Read header

  const Elf64_Ehdr *ehdr = (const Elf64_Ehdr *)start;

  switch (ehdr->e_type) {
    case ET_REL:
      break;
    case ET_DYN:
    case ET_EXEC:
      if (!hide_or_unhide)
        fprintf(stderr, PREFIX "warning: --unhide will likely not work for already linked files");
      break;
    default:
      fprintf(stderr, PREFIX "bad ELF type in %s: only ET_{REL,DYN,EXEC} are supported", file);
      exit(1);
  }
  const int is_reloc = ehdr->e_type == ET_REL;

  // Find symtab

  const Elf64_Shdr *shdr = (const Elf64_Shdr *)(start + ehdr->e_shoff);
  Elf64_Half shnum = ehdr->e_shnum;

  Elf64_Sym *symtab = NULL;
  const char *strtab = NULL;
  Elf64_Xword sym_count = 0;

  for (Elf64_Half i = 0; i < shnum; ++i) {
    if (shdr[i].sh_type != (is_reloc ? SHT_SYMTAB : SHT_DYNSYM))
      continue;

    if (symtab) {
      fprintf(stderr, PREFIX "multiple .dynsym/.symtab sections in %s\n", file);
      exit(1);
    }

    strtab = start + shdr[shdr[i].sh_link].sh_offset;

    symtab = (Elf64_Sym *)(start + shdr[i].sh_offset);
    sym_count = shdr[i].sh_size / shdr[i].sh_entsize;
    assert(shdr[i].sh_entsize == sizeof(Elf64_Sym));
  }

  if (!symtab) {
    fprintf(stderr, PREFIX "failed to locate .dynsym/.symtab section in %s\n", file);
    exit(1);
  }

  // Locate target symbols and change visibility

  for (Elf64_Xword i = 0; i < sym_count; ++i) {
    Elf64_Sym *sym = &symtab[i];
    const char *name = &strtab[sym->st_name];
    for (size_t j = 0; j < num_hidden_syms; ++j) {
      if (0 == strcmp(hidden_syms[j], name)) {
        sym->st_other &= ~0x3;
        sym->st_other |= hide_or_unhide ? STV_HIDDEN : STV_DEFAULT;
        // sym->st_info = ELF64_ST_INFO(STB_LOCAL, ELF64_ST_TYPE(sym->st_info)); TODO: do we need this ?!
        break;
      }
    }
  }

  // Write results

  int out_fd = open(out_file, O_WRONLY);

  ssize_t written = write(out_fd, start, file_size);
  if (written < 0 || written < file_size) {
    fprintf(stderr, PREFIX "failed to write output file %s\n", out_file);
    exit(1);
  }

  close(out_fd);
}
