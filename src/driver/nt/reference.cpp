#include "reference.h"

nt::driver_reference nt::reference_driver(const wchar_t* object_path, unsigned long attributes, KPROCESSOR_MODE access_mode)
{
  UNICODE_STRING object_path_unicide;
  RtlInitUnicodeString(&object_path_unicide, object_path);

  DRIVER_OBJECT* object;
  if (NT_SUCCESS(ObReferenceObjectByName(&object_path_unicide, attributes, NULL, NULL, *IoDriverObjectType, access_mode, NULL, reinterpret_cast<void**>(&object))))
  {
    return nt::driver_reference(object);
  }

  return {};
}

nt::process_reference nt::reference_process(unsigned long process_id)
{
  PEPROCESS process;
  if (NT_SUCCESS(PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(process_id), &process)))
  {
    return nt::process_reference(process);
  }

  return {};
}
