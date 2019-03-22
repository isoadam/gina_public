#include "driver.h"

using namespace gina;

bool driver::open()
{
  UNICODE_STRING object_name;
  OBJECT_ATTRIBUTES object_attributes;
  IO_STATUS_BLOCK io_status_block;

  RtlInitUnicodeString(&object_name, const_cast<wchar_t*>(device_path));
  InitializeObjectAttributes(&object_attributes, &object_name, OBJ_CASE_INSENSITIVE, NULL, NULL);

  HANDLE handle;
  if (NT_SUCCESS(NtOpenFile(&handle, FILE_GENERIC_READ | FILE_GENERIC_WRITE, &object_attributes, &io_status_block, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_NON_DIRECTORY_FILE)))
  {
    driver::handle = nt::handle(handle);
    return true;
  }

  return false;
}

void driver::close()
{
  handle.reset();
}

bool driver::opened()
{
  return handle != nullptr && handle.get() != INVALID_HANDLE_VALUE;
}

bool driver::is_present()
{
  return ioctl_present();
}

std::uintptr_t driver::get_image_base(unsigned long process_id)
{
  ioctl_image_base_parameters parameters = {};
  parameters.process_id = process_id;

  ioctl_image_base(parameters);

  return parameters.image_base;
}

std::uintptr_t driver::get_peb_address(unsigned long process_id)
{
  ioctl_peb_parameters parameters = {};
  parameters.process_id = process_id;

  ioctl_peb(parameters);

  return parameters.peb;
}

bool driver::read(unsigned long process_id, std::uintptr_t address, void* buffer, std::size_t size)
{
	ioctl_copy_memory_parameters parameters = {};
  parameters.process_id = process_id;
  parameters.buffer = reinterpret_cast<std::uintptr_t>(buffer);
  parameters.address = address;
  parameters.size = size;
  parameters.write = false;

  return ioctl_copy_memory(parameters);
}

bool driver::write(unsigned long process_id, std::uintptr_t address, void* buffer, std::size_t size)
{
	ioctl_copy_memory_parameters parameters = {};
  parameters.process_id = process_id;
  parameters.buffer = reinterpret_cast<std::uintptr_t>(buffer);
  parameters.address = address;
  parameters.size = size;
  parameters.write = true;

  return ioctl_copy_memory(parameters);
}

std::uintptr_t driver::allocate(unsigned long process_id, std::uintptr_t base_address, std::size_t size, unsigned long type, unsigned long protect)
{
  ioctl_allocate_memory_parameters parameters = {};
  parameters.process_id = process_id;
  parameters.base_address = base_address;
  parameters.size = size;
  parameters.type = type;
  parameters.protect = protect;

  if (ioctl_allocate_memory(parameters))
  {
    return parameters.base_address;
  }

  return 0;
}

bool driver::free(unsigned long process_id, std::uintptr_t base_address, std::size_t size, unsigned long type)
{
  ioctl_free_memory_parameters parameters = {};
  parameters.process_id = process_id;
  parameters.base_address = base_address;
  parameters.size = size;
  parameters.type = type;

  return ioctl_free_memory(parameters);
}

bool driver::remove_nx(unsigned long process_id, std::uintptr_t base_address, std::size_t size)
{
  ioctl_remove_nx_parameters parameters = {};
  parameters.process_id = process_id;
  parameters.base_address = base_address;
  parameters.size = size;

  return ioctl_remove_nx(parameters);
}

bool driver::ioctl_present()
{
  return DeviceIoControl(handle.get(), ioctl_code_present, NULL, 0, NULL, 0, NULL, NULL);
}

bool driver::ioctl_unload()
{
  return DeviceIoControl(handle.get(), ioctl_code_unload, NULL, 0, NULL, 0, NULL, NULL);
}

bool driver::ioctl_clear_unloaded(ioctl_code_clear_unloaded_parameters& parameters)
{
  return DeviceIoControl(handle.get(), ioctl_code_clear_unloaded, &parameters, sizeof(parameters), NULL, 0, NULL, NULL);
}

bool driver::ioctl_image_base(ioctl_image_base_parameters& parameters)
{
  return DeviceIoControl(handle.get(), ioctl_code_image_base, &parameters, sizeof(parameters), &parameters, sizeof(parameters), NULL, NULL);
}

bool driver::ioctl_peb(ioctl_peb_parameters& parameters)
{
  return DeviceIoControl(handle.get(), ioctl_code_peb, &parameters, sizeof(parameters), &parameters, sizeof(parameters), NULL, NULL);
}

bool driver::ioctl_copy_memory(ioctl_copy_memory_parameters& parameters)
{
  return DeviceIoControl(handle.get(), ioctl_code_copy_memory, &parameters, sizeof(parameters), NULL, 0, NULL, NULL);
}

bool driver::ioctl_allocate_memory(ioctl_allocate_memory_parameters& parameters)
{
  return DeviceIoControl(handle.get(), ioctl_code_allocate_memory, &parameters, sizeof(parameters), &parameters, sizeof(parameters), NULL, NULL);
}

bool driver::ioctl_free_memory(ioctl_free_memory_parameters& parameters)
{
  return DeviceIoControl(handle.get(), ioctl_code_free_memory, &parameters, sizeof(parameters), &parameters, sizeof(parameters), NULL, NULL);
}

bool driver::ioctl_remove_nx(ioctl_remove_nx_parameters& parameters)
{
  return DeviceIoControl(handle.get(), ioctl_code_remove_nx, &parameters, sizeof(parameters), &parameters, sizeof(parameters), NULL, NULL);
}
