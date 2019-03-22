#pragma once

#include <cstddef>
#include <cstdint>

namespace gina
{
  constexpr unsigned long gina_ioctl_code(int function)
  {
    return CTL_CODE(FILE_DEVICE_UNKNOWN, function, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS);
  }

  constexpr auto ioctl_code_present = gina_ioctl_code(0x800);
  constexpr auto ioctl_code_unload = gina_ioctl_code(0x801);
  constexpr auto ioctl_code_clear_unloaded = gina_ioctl_code(0x802);
  constexpr auto ioctl_code_image_base = gina_ioctl_code(0x803);
  constexpr auto ioctl_code_peb = gina_ioctl_code(0x804);
  constexpr auto ioctl_code_copy_memory = gina_ioctl_code(0x805);
  constexpr auto ioctl_code_allocate_memory = gina_ioctl_code(0x806);
  constexpr auto ioctl_code_free_memory = gina_ioctl_code(0x807);
  constexpr auto ioctl_code_remove_nx = gina_ioctl_code(0x808);

  struct ioctl_code_clear_unloaded_parameters
  {

  };

  struct ioctl_image_base_parameters
  {
    unsigned long process_id;
    std::uintptr_t image_base;
  };

  struct ioctl_peb_parameters
  {
    unsigned long process_id;
    std::uintptr_t peb;
  };

  struct ioctl_copy_memory_parameters
  {
    unsigned long process_id;
    std::uintptr_t buffer;
    std::uintptr_t address;
    std::size_t size;
    bool write;
  };

  struct ioctl_allocate_memory_parameters
  {
    unsigned long process_id;
    std::uintptr_t base_address;
    std::size_t size;
    unsigned long type;
    unsigned long protect;
  };

  struct ioctl_free_memory_parameters
  {
    unsigned long process_id;
    std::uintptr_t base_address;
    std::size_t size;
    unsigned long type;
  };

  struct ioctl_remove_nx_parameters
  {
    unsigned long process_id;
    std::uintptr_t base_address;
    std::size_t size;
  };
}
