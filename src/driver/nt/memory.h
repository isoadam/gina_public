#pragma once

#include "../nt.h"

typedef struct _MMPTE_HARDWARE
{
  ULONGLONG Valid : 1;
  ULONGLONG Writable : 1;
  ULONGLONG Owner : 1;
  ULONGLONG WriteThrough : 1;
  ULONGLONG CacheDisable : 1;
  ULONGLONG Accessed : 1;
  ULONGLONG Dirty : 1;
  ULONGLONG LargePage : 1;
  ULONGLONG Global : 1;
  ULONGLONG CopyOnWrite : 1;
  ULONGLONG Prototype : 1;
  ULONGLONG Write : 1;
  ULONGLONG PageFrameNumber : 40;
  ULONGLONG Reserved : 11;
  ULONGLONG NoExecute : 1;
} MMPTE_HARDWARE, *PMMPTE_HARDWARE;

typedef union _MMPTE
{
  ULONG_PTR Long;
  MMPTE_HARDWARE Hardware;
} MMPTE, *PMMPTE;
