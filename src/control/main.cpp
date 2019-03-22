#include "gina/driver.h"

int main(int argc, char* argv[])
{
  if (!gina::driver::open())
  {
    throw std::runtime_error("failed to open handle to device");
  }

  if (!gina::driver::is_present())
  {
    throw std::runtime_error("failed to communicate with patched device");
  }

  return 0;
}
