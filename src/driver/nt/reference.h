#pragma once

#include "../nt.h"

#include <memory>

namespace nt
{
  template <typename T, void(*dereference)(T*)>
  struct object_reference_deleter
  {
    void operator()(T* object)
    {
      dereference(object);
    }
  };

  template <typename T, auto dereference = reinterpret_cast<void(*)(T*)>(ObfDereferenceObject)>
  using object_reference = std::unique_ptr<T, object_reference_deleter<T, dereference>>;

  using driver_reference = object_reference<DRIVER_OBJECT>;
  using process_reference = object_reference<std::remove_pointer<PEPROCESS>::type>;

  driver_reference reference_driver(const wchar_t* object_path, unsigned long attributes = 0, KPROCESSOR_MODE access_mode = KernelMode);
  process_reference reference_process(unsigned long process_id);
}
