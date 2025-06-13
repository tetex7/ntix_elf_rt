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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>

#include "ntix_elf_header.h"
#include "ntix_utils.h"
#include "ntix_elf_file_c.h"

#include <windows.h>
#include <ntdef.h>
#include <winternl.h>

void cleanup_FILE(FILE** file)
{
    if (*file)
    {
        fclose(*file);
        *file = NULL;
    }
}

void cleanup_HANDLE(HANDLE** handle)
{
    if (*handle)
    {
        CloseHandle(*handle);
        *handle = NULL;
    }
}

NtCreateProcessEx_t NtCreateProcessEx = NULL;
NtAllocateVirtualMemory_t NtAllocateVirtualMemory = NULL;
NtCreateThreadEx_t NtCreateThreadEx = NULL;

bool MCL = true;

void sig(int si)
{
    MCL = false;
}

HANDLE rtVmPorc = NULL;
HANDLE rtVmMainThread = NULL;


void* map_elf(HANDLE vmProc, ntix_elf_header_t* header, FILE* elfFile)
{
    fseek(elfFile, header->e_phoff, SEEK_SET);

    for (uint16_t i = 0; i < header->e_phnum; i++)
    {
        ntix_elf_program_header_t ph;
        fread(&ph, sizeof(ph), 1, elfFile);

        if (ph.p_type != NTIX_PT_LOAD)
            continue;

        // Allocate memory in the target process
        LPVOID remote_addr = (LPVOID)ph.p_vaddr;
        SIZE_T mem_size = ph.p_memsz;
        NTSTATUS status = NtAllocateVirtualMemory(
            vmProc,
            &remote_addr,
            0,
            &mem_size,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_EXECUTE_READWRITE // Temp full access, can fine-tune later
        );

        if (status != 0)
        {
            fprintf(stderr, "Failed to allocate memory in target process: 0x%08lX\n", status);
            continue;
        }

        // Read segment from file
        uint8_t* segment_data = malloc(ph.p_filesz);
        if (!segment_data)
        {
            fprintf(stderr, "Out of memory\n");
            continue;
        }

        fseek(elfFile, ph.p_offset, SEEK_SET);
        fread(segment_data, 1, ph.p_filesz, elfFile);

        // Write into remote process
        SIZE_T written = 0;
        if (!WriteProcessMemory(vmProc, remote_addr, segment_data, ph.p_filesz, &written))
        {
            fprintf(stderr, "WriteProcessMemory failed: %lu\n", GetLastError());
        }

        free(segment_data);

        // Optionally: zero the remaining part if memsz > filesz
        if (ph.p_memsz > ph.p_filesz)
        {
            SIZE_T padding_size = ph.p_memsz - ph.p_filesz;
            uint8_t* zero_buf = calloc(1, padding_size);
            WriteProcessMemory(vmProc, (uint8_t*)remote_addr + ph.p_filesz, zero_buf, padding_size, &written);
            free(zero_buf);
        }
    }

    // Now you could use SetThreadContext or NtCreateThreadEx to jump to `header->e_entry`
    return (void*)header->e_entry;
}




int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        ntix_error("Usage: elf_rt.exe <file>\n");
        return 1;
    }
    signal(SIGINT, sig);

    NtCreateProcessEx =
    (NtCreateProcessEx_t)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"),
        "NtCreateProcessEx"
    );

    NtAllocateVirtualMemory =
    (NtAllocateVirtualMemory_t)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"),
        "NtAllocateVirtualMemory"
    );

    NtCreateThreadEx =
    (NtCreateThreadEx_t)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"),
        "NtCreateThreadEx"
    );

    const char* file = argv[1];

    ntix_elf_file_c* elf_file = ntix_elf_file_new(file);

    if (ntix_elf_file_isValidNtixElfFile(elf_file))
    {
        printf("How did you do that we're not even done yet\n");
    }


    OBJECT_ATTRIBUTES attr;
    InitializeObjectAttributes(&attr, NULL, 0, NULL, NULL);

    HANDLE hParent = GetCurrentProcess();

    NTSTATUS status = NtCreateProcessEx(
        &rtVmPorc,
        PROCESS_ALL_ACCESS,
        &attr,
        hParent,
        0,
        NULL,
        NULL,
        NULL,
        FALSE
    );

    if (status != 0)
    {
        printf("NtCreateProcessEx failed with status 0x%lX\n", status);
        return 66;
    }

    printf("Blank zombie process created! Handle: %p\n", rtVmPorc);

    //void* funcCall = map_elf(rtVmPorc, &elf_header, elf_file);

    /*NTSTATUS rstatus =  NtCreateThreadEx(
        &rtVmMainThread,
        THREAD_ALL_ACCESS,
        NULL,                              // ObjectAttributes
        rtVmPorc,                          // Handle to the process you're injecting into
        (LPTHREAD_START_ROUTINE)funcCall,  // Entry point inside the target
        NULL,                              // Optional param
        FALSE,                             // Create suspended?
        0,                                 // ZeroBits
        0x10000,                           // StackSize
        0x100000,                          // MaxStackSize
        NULL                               // Attribute list
    );

    if (rstatus != 0)
    {
        printf("NtCreateThreadEx failed with status 0x%lX\n", status);
        return 67;
    }*/

    TerminateProcess(rtVmPorc, 0);
    //CloseHandle(rtVmMainThread);
    CloseHandle(rtVmPorc);

}