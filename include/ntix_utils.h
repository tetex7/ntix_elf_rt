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
#ifndef NTIX_UTILS_H
#define NTIX_UTILS_H
#include <stdio.h>

#include <windows.h>
#include <ntdef.h>
#include <winternl.h>
#include <stdint.h>
#include <stdbool.h>


#define NTIX_CLEANUP(func_call) __attribute__((cleanup(func_call)))

typedef NTSTATUS (NTAPI *NtCreateProcessEx_t)(
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ParentProcess,
    IN ULONG Flags,
    IN HANDLE SectionHandle OPTIONAL,
    IN HANDLE DebugPort OPTIONAL,
    IN HANDLE ExceptionPort OPTIONAL,
    IN BOOLEAN InJob
);

typedef NTSTATUS (NTAPI *NtAllocateVirtualMemory_t)(
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN ULONG_PTR ZeroBits,
    IN OUT PSIZE_T RegionSize,
    IN ULONG AllocationType,
    IN ULONG Protect
);

typedef NTSTATUS (NTAPI *NtCreateThreadEx_t)(
    OUT PHANDLE ThreadHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ProcessHandle,
    IN PVOID StartRoutine,
    IN PVOID Argument OPTIONAL,
    IN ULONG CreateFlags,
    IN SIZE_T ZeroBits,
    IN SIZE_T StackSize,
    IN SIZE_T MaximumStackSize,
    IN PVOID AttributeList OPTIONAL
);

extern NtCreateProcessEx_t NtCreateProcessEx;
extern NtAllocateVirtualMemory_t NtAllocateVirtualMemory;
extern NtCreateThreadEx_t NtCreateThreadEx;

#define ntix_error(msg, ...) fprintf(stderr, msg, ##__VA_ARGS__);

#endif //NTIX_UTILS_H
