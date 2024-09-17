#include "elf.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

struct ELFHeader {
  uint8_t e_magic[4];
  uint8_t e_class;
  uint8_t e_data;
  uint8_t e_version;
  uint8_t e_osabi;
  uint8_t e_abiversion;
  uint8_t e_pad[7];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version2;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
};
struct ELFProgramHeader {
  uint32_t p_type;
  uint32_t p_flags;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
};

void generateExecutableELF(char *filename) {

  int bytesWritten = 0;
  FILE *f = fopen(filename, "w");
  struct ELFHeader header;
  header.e_magic[0] = 0x7f;
  header.e_magic[1] = 'E';
  header.e_magic[2] = 'L';
  header.e_magic[3] = 'F';
  header.e_class = 0x02;
  header.e_data = 0x01;
  header.e_version = 0x01;
  header.e_osabi = 0x00;
  header.e_abiversion = 0x00;
  header.e_pad[0] = 0x00;
  header.e_pad[1] = 0x00;
  header.e_pad[2] = 0x00;
  header.e_pad[3] = 0x00;
  header.e_pad[4] = 0x00;
  header.e_pad[5] = 0x00;
  header.e_pad[6] = 0x00;
  header.e_type = 0x02;
  header.e_machine = 0x3E;
  header.e_version2 = 0x01;
  header.e_entry = 0x0000000000400000 + sizeof(struct ELFHeader) +
                   sizeof(struct ELFProgramHeader); //???
  header.e_phoff = 0x40;
  header.e_shoff = 0x0;
  header.e_flags = 0x00;
  header.e_ehsize = 0x40;
  header.e_phentsize = 0x38;
  header.e_phnum = 0x01; // ???
  header.e_shentsize = 0x40;
  header.e_shnum = 0x00;    // ???
  header.e_shstrndx = 0x00; // ???
  fwrite(&header, sizeof(header), 1, f);
  bytesWritten += sizeof(header);

  struct ELFProgramHeader ph;
  ph.p_type = 0x01;
  ph.p_flags = 0x5; // r-x
  ph.p_offset = 0x0;
  ph.p_vaddr = 0x0000000000400000;
  ph.p_paddr = 0x0000000000400000;
  ph.p_filesz = 0x80;
  ph.p_memsz = 0x80;
  ph.p_align = 0x1;
  fwrite(&ph, sizeof(ph), 1, f);
  bytesWritten += sizeof(ph);

  // exit(1)
  // mov rax, 0x3c
  // mov rdi, 1
  // syscall
  fwrite("\x48\xc7\xc0\x3c\x00\x00\x00\x48\xc7\xc7\x01\x00\x00\x00\x0f\x05", 16,
         1, f);

  fclose(f);

  chmod(filename, 0755);
}
