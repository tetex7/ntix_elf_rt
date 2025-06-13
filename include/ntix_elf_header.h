/*
 * Copyright (C) 2025  Tetex7
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

//
// Created by tete on 06/06/2025.
//
#pragma once

#ifndef NTIX_ELF_HEADER_H
#define NTIX_ELF_HEADER_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdint.h>
#define NTIX_ELFOSABI_NTIX       0x42
#define NTIX_EI_NIDENT 16
typedef struct __attribute__((packed)) ntix_elf_header_s
{
    union
    {
        uint8_t raw[NTIX_EI_NIDENT]; // Raw identifier array

        struct
        {
            uint8_t magic[4];     // 0x7F, 'E', 'L', 'F'
            uint8_t eclass;        // 1 = 32-bit, 2 = 64-bit
            uint8_t data;         // 1 = little endian, 2 = big endian
            uint8_t version;      // ELF version (should be 1)
            uint8_t osabi;        // OS/ABI identification
            uint8_t abiversion;   // ABI version
            uint8_t pad[7];       // Unused padding bytes
        } fields;
    } e_ident;
    uint16_t e_type;              // Object file type
    uint16_t e_machine;           // Machine architecture
    uint32_t e_version;           // ELF version
    uint64_t e_entry;             // Entry point virtual address
    uint64_t e_phoff;             // Program header table file offset
    uint64_t e_shoff;             // Section header table file offset
    uint32_t e_flags;             // Processor-specific flags
    uint16_t e_ehsize;            // ELF header size
    uint16_t e_phentsize;         // Size of one program header entry
    uint16_t e_phnum;             // Number of program header entries
    uint16_t e_shentsize;         // Size of one section header entry
    uint16_t e_shnum;             // Number of section header entries
    uint16_t e_shstrndx;          // Section header string table index
} ntix_elf_header_t;


#define NTIX_PT_NULL    0
#define NTIX_PT_LOAD    1
#define NTIX_PT_DYNAMIC 2
#define NTIX_PT_INTERP  3
#define NTIX_PT_NOTE    4
#define NTIX_PT_SHLIB   5
#define NTIX_PT_PHDR    6

typedef struct __attribute__((packed)) ntix_elf_program_header_s {
    uint32_t p_type;     // Segment type
    uint32_t p_flags;    // Segment flags
    uint64_t p_offset;   // File offset
    uint64_t p_vaddr;    // Virtual address in memory
    uint64_t p_paddr;    // Physical address (ignore for NTIX)
    uint64_t p_filesz;   // Number of bytes in file
    uint64_t p_memsz;    // Number of bytes in memory
    uint64_t p_align;    // Alignment
} ntix_elf_program_header_t;

#ifdef  __cplusplus
}
#endif

#endif

