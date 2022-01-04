/*
 * Copyright 2022 Yury Gribov
 * 
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

#include "common.h"
#include "hider.h"

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include <elf.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

void hide_symbols(const char *file, const char *out_file,
                  const char **hidden_syms, size_t num_hidden_syms) {
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
  char *start = mmap(0, file_size, inplace ? PROT_READ | PROT_WRITE : PROT_READ,
                     MAP_PRIVATE, fd, 0);
  if (start == NULL || start == MAP_FAILED) {
    fprintf(stderr, PREFIX "mmap for %s failed: %s", file, strerror (errno));
    exit(1);
  }

  // Read header

  const Elf64_Ehdr *ehdr = (const Elf64_Ehdr *)start;

  // Find .dynsym

  const Elf64_Shdr *shdr = (const Elf64_Shdr *)(start + ehdr->e_shoff);
  Elf64_Half shnum = ehdr->e_shnum;

  Elf64_Sym *dynsym = NULL;
  const char *dynstr = NULL;
  Elf64_Xword sym_count = 0;

  for (Elf64_Half i = 0; i < shnum; ++i) {
    if (shdr[i].sh_type != SHT_DYNSYM)
      continue;

    if (dynsym) {
      fprintf(stderr, PREFIX "multiple .dynsym sections in %s\n", file);
      exit(1);
    }

    dynstr = start + shdr[shdr[i].sh_link].sh_offset;

    dynsym = (Elf64_Sym *)(start + shdr[i].sh_offset);
    sym_count = shdr[i].sh_size / shdr[i].sh_entsize;
    assert(shdr[i].sh_entsize == sizeof(Elf64_Sym));
  }

  if (!dynsym) {
    fprintf(stderr, PREFIX "failed to locate .dynsym section in %s\n", file);
    exit(1);
  }

  // Locate target symbols and change visibility

  for (Elf64_Xword i = 0; i < sym_count; ++i) {
    Elf64_Sym *sym = &dynsym[i];
    const char *name = &dynstr[sym->st_name];
    for (size_t j = 0; j < num_hidden_syms; ++j) {
      if (0 == strcmp(hidden_syms[j], name)) {
        sym->st_other &= ~0x3;
        sym->st_other |= STV_HIDDEN;
        sym->st_info = ELF64_ST_INFO(STB_LOCAL, ELF64_ST_TYPE(sym->st_info));
        break;
      }
    }
  }

  // Clean up

  if (out_file) {
    int out_fd = open(out_file, O_WRONLY);
    ssize_t written = write(out_fd, start, file_size);
    if (written < 0 || written < file_size) {
      fprintf(stderr, PREFIX "failed to write output file %s\n", out_file);
      exit(1);
    }
    close(out_fd);
  } else if (0 != msync(start, file_size, MS_SYNC)) {
    fprintf(stderr, PREFIX "failed to synch file %s\n", file);
    exit(1);
  }

  if(0 != munmap(start, file_size)) {
    fprintf(stderr, PREFIX "failed to unmap file %s\n", file);
    exit(1);
  }
  close(fd);
}
