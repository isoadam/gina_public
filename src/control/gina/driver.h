#pragma once

#include "../nt.h"
#include "../../driver/gina.h"
#include <string>

namespace gina
{
  namespace driver
  {
    constexpr const auto device_path = L"\\Device\\Null";

    static nt::handle handle = {};

    bool open();
    void close();
    bool opened();

    bool is_present();
    std::uintptr_t get_image_base(unsigned long process_id);
    std::uintptr_t get_peb_address(unsigned long process_id);
    bool read(unsigned long process_id, std::uintptr_t address, void* buffer, std::size_t size);
    bool write(unsigned long process_id, std::uintptr_t address, void* buffer, std::size_t size);
    std::uintptr_t allocate(unsigned long process_id, std::uintptr_t base_address, std::size_t size, unsigned long type, unsigned long protect);
    bool free(unsigned long process_id, std::uintptr_t base_address, std::size_t size, unsigned long type);
    bool remove_nx(unsigned long process_id, std::uintptr_t base_address, std::size_t size);

    template <typename T>
    T read(unsigned long process_id, std::uintptr_t address)
    {
      T buffer;
      if (read(process_id, address, &buffer, sizeof(T)))
      {
        return buffer;
      }

      return {};
    }

    template <typename T>
    void write(unsigned long process_id, std::uintptr_t address, const T& buffer)
    {
      write(process_id, address, &buffer, sizeof(T));
    }

    inline bool ioctl_present();
    inline bool ioctl_unload();
    inline bool ioctl_clear_unloaded(ioctl_code_clear_unloaded_parameters& parameters);
    inline bool ioctl_image_base(ioctl_image_base_parameters& parameters);
    inline bool ioctl_peb(ioctl_peb_parameters& paramaters);
    inline bool ioctl_copy_memory(ioctl_copy_memory_parameters& parameters);
    inline bool ioctl_allocate_memory(ioctl_allocate_memory_parameters& parameters);
    inline bool ioctl_free_memory(ioctl_free_memory_parameters& parameters);
    inline bool ioctl_remove_nx(ioctl_remove_nx_parameters& parameters);
  }
}
