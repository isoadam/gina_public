#pragma once

#include "nt.h"

#include <cstddef>
#include <cstdint>

namespace utilities
{
  template <typename T = void*>
  T find_pattern(void* start, std::size_t length, const char* pattern, const char* mask)
  {
    const auto data = static_cast<const char*>(start);
    const auto pattern_length = strlen(mask);

    for (std::size_t i = 0; i <= length - pattern_length; i++)
    {
      bool accumulative_found = true;

      for (std::size_t j = 0; j < pattern_length; j++)
      {
        if (!MmIsAddressValid(reinterpret_cast<void*>(reinterpret_cast<std::intptr_t>(data) + i + j)))
        {
          accumulative_found = false;
          break;
        }

        if (data[i + j] != pattern[j] && mask[j] != '?')
        {
          accumulative_found = false;
          break;
        }
      }

      if (accumulative_found)
      {
        return reinterpret_cast<T>(reinterpret_cast<std::intptr_t>(data) + i);
      }
    }

    return nullptr;
  }
}
