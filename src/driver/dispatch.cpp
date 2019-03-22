#include "dispatch.h"

NTSTATUS dispatch_image_base(gina::ioctl_image_base_parameters* parameters)
{
  auto process = nt::reference_process(parameters->process_id);
  if (process == nullptr)
  {
    return STATUS_NOT_FOUND;
  }

  parameters->image_base = reinterpret_cast<std::uintptr_t>(PsGetProcessSectionBaseAddress(process.get()));

  return STATUS_SUCCESS;
}

NTSTATUS dispatch_peb(gina::ioctl_peb_parameters* parameters)
{
  auto process = nt::reference_process(parameters->process_id);
  if (process == nullptr)
  {
    return STATUS_NOT_FOUND;
  }

  parameters->peb = reinterpret_cast<std::uintptr_t>(PsGetProcessPeb(process.get()));

  return STATUS_SUCCESS;
}

NTSTATUS dispatch_copy_memory(gina::ioctl_copy_memory_parameters* parameters)
{
  auto process = nt::reference_process(parameters->process_id);
  if (process == nullptr)
  {
    return STATUS_NOT_FOUND;
  }

  const auto buffer = reinterpret_cast<void*>(parameters->buffer);
  const auto address = reinterpret_cast<void*>(parameters->address);

  // beware: no size check on buffers occurs
  if (!MmIsAddressValid(buffer))
  {
    return STATUS_ACCESS_VIOLATION;
  }

  KAPC_STATE kapc_state;
  KeStackAttachProcess(process.get(), &kapc_state);

  if (!MmIsAddressValid(address))
  {
    KeUnstackDetachProcess(&kapc_state);
    return STATUS_ACCESS_VIOLATION;
  }

  KeUnstackDetachProcess(&kapc_state);

  std::size_t bytes_copied;
  if (parameters->write)
  {
    return MmCopyVirtualMemory(IoGetCurrentProcess(), buffer, process.get(), address, parameters->size, KernelMode, &bytes_copied);
  }
  else
  {
    return MmCopyVirtualMemory(process.get(), address, IoGetCurrentProcess(), buffer, parameters->size, KernelMode, &bytes_copied);
  }
}

NTSTATUS dispatch_allocate_memory(gina::ioctl_allocate_memory_parameters* parameters)
{
  auto process = nt::reference_process(parameters->process_id);
  if (process == nullptr)
  {
    return STATUS_NOT_FOUND;
  }

  KAPC_STATE kapc_state;
  KeStackAttachProcess(process.get(), &kapc_state);

  const auto status = ZwAllocateVirtualMemory(NtCurrentProcess(), reinterpret_cast<void**>(&parameters->base_address), 0, &parameters->size, parameters->type, parameters->protect);

  KeUnstackDetachProcess(&kapc_state);

  return status;
}

NTSTATUS dispatch_free_memory(gina::ioctl_free_memory_parameters* parameters)
{
  auto process = nt::reference_process(parameters->process_id);
  if (process == nullptr)
  {
    return STATUS_NOT_FOUND;
  }

  KAPC_STATE kapc_state;
  KeStackAttachProcess(process.get(), &kapc_state);

  const auto status = NtFreeVirtualMemory(NtCurrentProcess(), reinterpret_cast<void**>(&parameters->base_address), &parameters->size, parameters->type);

  KeUnstackDetachProcess(&kapc_state);

  return status;
}

NTSTATUS dispatch_remove_nx(gina::ioctl_remove_nx_parameters* parameters)
{
  auto process = nt::reference_process(parameters->process_id);
  if (process == nullptr)
  {
    return STATUS_NOT_FOUND;
  }

  if (MiGetPteAddress == nullptr)
  {
    return STATUS_UNSUCCESSFUL;
  }

  for (std::uintptr_t page = parameters->base_address; page < parameters->base_address + parameters->size; page += PAGE_SIZE)
  {
    if (auto pte = MiGetPteAddress(reinterpret_cast<void*>(page)); pte != nullptr)
    {
      if (pte->Hardware.Valid)
      {
        pte->Hardware.NoExecute = 0;
      }
    }
    else
    {
      return STATUS_ACCESS_VIOLATION;
    }
  }

  return STATUS_SUCCESS;
}
