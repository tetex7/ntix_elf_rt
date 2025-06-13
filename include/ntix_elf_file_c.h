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
// Created by tete on 06/13/2025.
//
#pragma once
#ifndef NTIX_ELF_FILE_C_H
#define NTIX_ELF_FILE_C_H
#include <stdio.h>
#include <stdbool.h>
#include "ntix_elf_header.h"

#define NTIX_ELF_FILE_NOT_ELF_FILE ((void*)-2)

typedef struct ntix_elf_file_s
{
    ntix_elf_header_t header;
    ntix_elf_program_header_t* program_header_array;
    size_t program_header_len;
    FILE* raw_elf_file;
} ntix_elf_file_c;

ntix_elf_file_c* ntix_elf_file_new(const char* path);

const ntix_elf_header_t* ntix_elf_file_getElfHeader(const ntix_elf_file_c* self);
const ntix_elf_program_header_t* ntix_elf_file_getProgramHeaderArray(const ntix_elf_file_c* self);
size_t ntix_elf_file_getProgramHeaderArrayLen(const ntix_elf_file_c* self);
bool ntix_elf_file_isValidNtixElfFile(const ntix_elf_file_c* self);

void ntix_elf_file_destroy(ntix_elf_file_c* self);

#endif //NTIX_ELF_FILE_C_H
