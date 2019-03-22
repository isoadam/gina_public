#pragma once

#include "nt.h"
#include "gina.h"

NTSTATUS dispatch_image_base(gina::ioctl_image_base_parameters* parameters);
NTSTATUS dispatch_peb(gina::ioctl_peb_parameters* parameters);
NTSTATUS dispatch_copy_memory(gina::ioctl_copy_memory_parameters* parameters);
NTSTATUS dispatch_allocate_memory(gina::ioctl_allocate_memory_parameters* parameters);
NTSTATUS dispatch_free_memory(gina::ioctl_free_memory_parameters* parameters);
NTSTATUS dispatch_remove_nx(gina::ioctl_remove_nx_parameters* parameters);
