#include "nt.h"

#include <intrin.h>

void* nt::kernel_base_address()
{
  return *reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(PsLoadedModuleList) + 0x30);
}

std::size_t nt::kernel_base_size()
{
  return *reinterpret_cast<std::size_t*>(reinterpret_cast<std::uintptr_t>(PsLoadedModuleList) + 0x40);
}

std::uintptr_t nt::get_process_cr3(PEPROCESS process)
{
  KAPC_STATE kapc_state;
  KeStackAttachProcess(process, &kapc_state);

  const auto cr3 = __readcr3();

  KeUnstackDetachProcess(&kapc_state);

  return cr3;
}

// unexported
PMMPTE(NTAPI *MiGetPteAddress)(PVOID VirtualAddress) = nullptr;
