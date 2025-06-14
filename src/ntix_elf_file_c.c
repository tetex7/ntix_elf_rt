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

#include <string.h>
#include <stdlib.h>

#include "ntix_elf_file_c.h"

ntix_elf_file_c* ntix_elf_file_new(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file)
        return NULL;

    uint8_t magic[4] = {0};
    if (fread(&magic, 1, sizeof(magic), file) != sizeof(magic) ||
        magic[0] != 0x7F ||
        magic[1] != 'E' ||
        magic[2] != 'L' ||
        magic[3] != 'F')
    {
        fclose(file);
        return NTIX_ELF_FILE_NOT_ELF_FILE;
    }

    fseek(file, 0, SEEK_SET);

    ntix_elf_file_c* self = malloc(sizeof(ntix_elf_file_c));

    if (!self)
    {
        fclose(file);
        return NULL;
    }

    memset(self, 0, sizeof(ntix_elf_file_c));

    self->raw_elf_file = file;

    if (fread(&self->header, 1, sizeof(ntix_elf_header_t), file) != sizeof(ntix_elf_header_t))
    {
        ntix_elf_file_destroy(self);
        return NULL;
    }

    self->program_header_len = self->header.e_phnum;
    size_t alloc_size = self->program_header_len * sizeof(ntix_elf_program_header_t);

    self->program_header_array = malloc(alloc_size);
    if (!self->program_header_array)
    {
        ntix_elf_file_destroy(self);
        return NULL;
    }

    if (fseek(file, (long)self->header.e_phoff, SEEK_SET) != 0)
    {
        ntix_elf_file_destroy(self);
        return NULL;
    }

    for (size_t i = 0; i < self->header.e_phnum; i++)
    {
        if (fread(&self->program_header_array[i], sizeof(ntix_elf_program_header_t), 1, file) != 1)
        {
            ntix_elf_file_destroy(self);
            return NULL;
        }
    }

    return self;
}

const ntix_elf_header_t* ntix_elf_file_getElfHeader(const ntix_elf_file_c* self)
{
    return &self->header;
}

const ntix_elf_program_header_t* ntix_elf_file_getProgramHeaderArray(const ntix_elf_file_c* self)
{
    return self->program_header_array;
}

size_t ntix_elf_file_getProgramHeaderArrayLen(const ntix_elf_file_c* self)
{
    return self->program_header_len;
}

FILE* ntix_elf_file_getRawFile(const ntix_elf_file_c* self)
{
    return self->raw_elf_file;
}

bool ntix_elf_file_isValidNtixElfFile(const ntix_elf_file_c* self)
{
    return self->header.e_ident.fields.osabi == NTIX_ELFOSABI_NTIX;
}

void ntix_elf_file_destroy(ntix_elf_file_c* self)
{
    if (!self) return;
    if (self->raw_elf_file)
        fclose(self->raw_elf_file);
    if (self->program_header_array)
        free(self->program_header_array);
    free(self);
}

void ntix_elf_file_destroy_cleanup(ntix_elf_file_c** self)
{
    if (!self) return;
    if (!*self) return;
    ntix_elf_file_destroy(*self);
    *self = NULL;
}
