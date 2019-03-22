#include "driver.h"

#include "nt.h"
#include "gina.h"
#include "utilities.h"
#include "dispatch.h"
#include <algorithm>

#ifdef max
#undef max
#endif

DRIVER_DISPATCH* null_original_irp_device_control = {};

// mov rax, 0i64
// jmp rax
const unsigned char jmp_buffer[] = { 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0 };

NTSTATUS DriverEntry(DRIVER_OBJECT* driver_object, UNICODE_STRING* registry_path)
{
  UNREFERENCED_PARAMETER(registry_path);

  driver_object->DriverUnload = unload;

  return initialise();
}

NTSTATUS initialise()
{
  MiGetPteAddress = utilities::find_pattern<decltype(MiGetPteAddress)>(nt::kernel_base_address(), nt::kernel_base_size(), "\x48\xC1\xE9\x09\x48\xB8\xF8\xFF\xFF\xFF\x7F\x00\x00\x00\x48\x23\xC8\x48\xB8\x00\x00\x00\x00\x00\x00\x00\x00\x48\x03\xC1\xC3", "xxxxxxxxxxxxxxxxxxx????????xxxx");
  if (MiGetPteAddress == nullptr)
  {
    return STATUS_UNSUCCESSFUL;
  }

  auto null_driver_object = nt::reference_driver(L"\\Driver\\Null");
  if (null_driver_object == nullptr)
  {
    return STATUS_UNSUCCESSFUL;
  }

  PIMAGE_SECTION_HEADER null_discardable_section_header = {};

  const auto null_nt_headers = RtlImageNtHeader(null_driver_object->DriverStart);
  const auto first_section = IMAGE_FIRST_SECTION(null_nt_headers);

  for (PIMAGE_SECTION_HEADER section = first_section; section < first_section + null_nt_headers->FileHeader.NumberOfSections; section++)
  {
    // assume INIT section is RWX - implement assert
    if (section->Characteristics & 0x02000000 && section->Misc.VirtualSize > 0 && section->Misc.VirtualSize <= PAGE_SIZE) // IMAGE_SCN_MEM_DISCARDABLE
    {
      null_discardable_section_header = section;
      break;
    }
  }

  if (null_discardable_section_header == nullptr)
  {
    return STATUS_UNSUCCESSFUL;
  }

  const auto discardable_section = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(null_driver_object->DriverStart) + null_discardable_section_header->VirtualAddress);
  auto discardable_section_pte = MiGetPteAddress(discardable_section);

  const auto discardable_allocated_buffer = ExAllocatePool(NonPagedPool, ROUND_TO_PAGES(null_discardable_section_header->Misc.VirtualSize)); // not currently handling more than 1 page anyhow
  auto discardable_allocated_buffer_pte = MiGetPteAddress(discardable_allocated_buffer);

  if (discardable_section_pte == nullptr || discardable_allocated_buffer_pte == nullptr)
  {
    return STATUS_UNSUCCESSFUL;
  }

  // copy pte
  *discardable_section_pte = *discardable_allocated_buffer_pte;

  // create hook
  memcpy(discardable_section, jmp_buffer, sizeof(jmp_buffer));
  *reinterpret_cast<void**>(&reinterpret_cast<unsigned char*>(discardable_section)[2]) = &device_control;
c
  // replace dispatch function
  null_original_irp_device_control = null_driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL];
  null_driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = reinterpret_cast<DRIVER_DISPATCH*>(discardable_section);

  return STATUS_SUCCESS;
}

void unload(DRIVER_OBJECT* driver_object)
{
  if (driver_object != nullptr && null_original_irp_device_control != nullptr)
  {
    if (auto null_driver_object = nt::reference_driver(L"\\Driver\\Null"); null_driver_object.get() != nullptr)
    {
      null_driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = null_original_irp_device_control;
    }
  }
}

NTSTATUS device_control(DEVICE_OBJECT* device_object, IRP* irp)
{
  const auto io_stack_location = IoGetCurrentIrpStackLocation(irp);
  auto& buffer = irp->AssociatedIrp.SystemBuffer;
  auto& status = irp->IoStatus.Status;

  if (io_stack_location->MajorFunction == IRP_MJ_DEVICE_CONTROL)
  {
    const auto io_control_code = io_stack_location->Parameters.DeviceIoControl.IoControlCode;
    const auto buffer_length = std::max(io_stack_location->Parameters.DeviceIoControl.InputBufferLength, io_stack_location->Parameters.DeviceIoControl.OutputBufferLength);

    switch (io_control_code)
    {
    case gina::ioctl_code_present:
      status = STATUS_SUCCESS;

      break;

    case gina::ioctl_code_unload:
      // unhook
      // set success/complete irp
      // jmp to asm to modify registers/stack to call ExFreePool but return to parent

      status = STATUS_UNSUCCESSFUL;

      break;

    case gina::ioctl_code_clear_unloaded:
      status = STATUS_UNSUCCESSFUL;

      break;

    case gina::ioctl_code_image_base:
      if (buffer_length >= sizeof(gina::ioctl_image_base_parameters))
      {
        status = dispatch_image_base(static_cast<gina::ioctl_image_base_parameters*>(buffer));
        irp->IoStatus.Information = sizeof(gina::ioctl_image_base_parameters);
      }
      else
      {
        status = STATUS_INFO_LENGTH_MISMATCH;
      }

      break;

    case gina::ioctl_code_peb:
      if (buffer_length >= sizeof(gina::ioctl_peb_parameters))
      {
        status = dispatch_peb(static_cast<gina::ioctl_peb_parameters*>(buffer));
        irp->IoStatus.Information = sizeof(gina::ioctl_peb_parameters);
      }
      else
      {
        status = STATUS_INFO_LENGTH_MISMATCH;
      }

      break;

    case gina::ioctl_code_copy_memory:
      if (buffer_length >= sizeof(gina::ioctl_copy_memory_parameters))
      {
        status = dispatch_copy_memory(static_cast<gina::ioctl_copy_memory_parameters*>(buffer));
        irp->IoStatus.Information = sizeof(gina::ioctl_copy_memory_parameters);
      }
      else
      {
        status = STATUS_INFO_LENGTH_MISMATCH;
      }

      break;

    case gina::ioctl_code_allocate_memory:
      if (buffer_length >= sizeof(gina::ioctl_allocate_memory_parameters))
      {
        status = dispatch_allocate_memory(static_cast<gina::ioctl_allocate_memory_parameters*>(buffer));
        irp->IoStatus.Information = sizeof(gina::ioctl_allocate_memory_parameters);
      }
      else
      {
        status = STATUS_INFO_LENGTH_MISMATCH;
      }

      break;

    case gina::ioctl_code_free_memory:
      if (buffer_length >= sizeof(gina::ioctl_free_memory_parameters))
      {
        status = dispatch_free_memory(static_cast<gina::ioctl_free_memory_parameters*>(buffer));
        irp->IoStatus.Information = sizeof(gina::ioctl_free_memory_parameters);
      }
      else
      {
        status = STATUS_INFO_LENGTH_MISMATCH;
      }

      break;

    case gina::ioctl_code_remove_nx:
      if (buffer_length >= sizeof(gina::ioctl_remove_nx_parameters))
      {
        status = dispatch_remove_nx(static_cast<gina::ioctl_remove_nx_parameters*>(buffer));
        irp->IoStatus.Information = sizeof(gina::ioctl_remove_nx_parameters);
      }
      else
      {
        status = STATUS_INFO_LENGTH_MISMATCH;
      }

      break;

    default:
      return null_original_irp_device_control(device_object, irp);
    }
  }
  else
  {
    status = STATUS_UNSUCCESSFUL;
  }

  IoCompleteRequest(irp, IO_NO_INCREMENT);

  return status;
}
