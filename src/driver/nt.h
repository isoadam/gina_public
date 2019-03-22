#pragma once

#define PHNT_MODE PHNT_MODE_KERNEL
#define PHNT_VERSION PHNT_THRESHOLD

#include <ntifs.h>
#include <phnt.h>

#include <cstddef>
#include <cstdint>

#include "nt/memory.h"
#include "nt/portable_executable.h"
#include "nt/reference.h"

namespace nt
{
  void* kernel_base_address();
  std::size_t kernel_base_size();

  std::uintptr_t get_process_cr3(PEPROCESS process);

  template <typename T = void*>
  auto get_system_routine(const wchar_t* routine_name) -> T
  {
    UNICODE_STRING routine_unicode_name;
    RtlInitUnicodeString(&routine_unicode_name, routine_name);

    return reinterpret_cast<T>(MmGetSystemRoutineAddress(&routine_unicode_name));
  }
}

// undocumented imports
extern "C" __declspec(dllimport) POBJECT_TYPE* IoDriverObjectType;
extern "C" __declspec(dllimport) PLIST_ENTRY PsLoadedModuleList;

extern "C" __declspec(dllimport) NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS FromProcess, PVOID FromAddress, PEPROCESS ToProcess, PVOID ToAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T NumberOfBytesCopied);
extern "C" __declspec(dllimport) NTSTATUS NTAPI ObReferenceObjectByName(PUNICODE_STRING ObjectName, ULONG Attributes, PACCESS_STATE AccessState, ACCESS_MASK DesiredAccess, POBJECT_TYPE ObjectType, KPROCESSOR_MODE AccessMode, PVOID ParseContext OPTIONAL, PVOID* Object);
extern "C" __declspec(dllimport) PVOID NTAPI PsGetProcessSectionBaseAddress(PEPROCESS Process);
extern "C" __declspec(dllimport) PIMAGE_NT_HEADERS NTAPI RtlImageNtHeader(PVOID Base);
extern "C" __declspec(dllimport) PPEB NTAPI PsGetProcessPeb(PEPROCESS Process);

// unexported
extern PMMPTE(NTAPI *MiGetPteAddress)(PVOID VirtualAddress);
