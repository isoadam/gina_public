#pragma once

#include "nt.h"

extern "C" NTSTATUS DriverEntry(DRIVER_OBJECT* driver_object, UNICODE_STRING* registry_path);
extern "C" NTSTATUS initialise();
void unload(DRIVER_OBJECT* driver_object);
NTSTATUS device_control(DEVICE_OBJECT* device_object, IRP* irp);
