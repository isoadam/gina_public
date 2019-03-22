#pragma once

#include "../nt.h"
#include <memory>
#include <type_traits>

namespace nt
{
  template <typename T, void(*closer)(T*)>
  struct handle_deleter
  {
    void operator()(T* handle)
    {
      closer(handle);
    }
  };

  template <typename T, void(*closer)(T*)>
  using generic_handle = std::unique_ptr<T, handle_deleter<T, closer>>;

  using handle = generic_handle<std::remove_pointer<HANDLE>::type, reinterpret_cast<void(*)(HANDLE)>(CloseHandle)>;
  using sc_handle = generic_handle<std::remove_pointer<SC_HANDLE>::type, reinterpret_cast<void(*)(SC_HANDLE)>(CloseServiceHandle)>;
}
