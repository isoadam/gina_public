#pragma once

#include "../nt.h"

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

typedef struct _IMAGE_FILE_HEADER
{
  USHORT Machine;
  USHORT NumberOfSections;
  ULONG TimeDateStamp;
  ULONG PointerToSymbolTable;
  ULONG NumberOfSymbols;
  USHORT SizeOfOptionalHeader;
  USHORT Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY
{
  ULONG VirtualAddress;
  ULONG Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_SECTION_HEADER
{
  UCHAR  Name[8];
  union
  {
    ULONG PhysicalAddress;
    ULONG VirtualSize;
  } Misc;
  ULONG VirtualAddress;
  ULONG SizeOfRawData;
  ULONG PointerToRawData;
  ULONG PointerToRelocations;
  ULONG PointerToLinenumbers;
  USHORT NumberOfRelocations;
  USHORT NumberOfLinenumbers;
  ULONG Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER64
{
  USHORT Magic;
  UCHAR MajorLinkerVersion;
  UCHAR MinorLinkerVersion;
  ULONG SizeOfCode;
  ULONG SizeOfInitializedData;
  ULONG SizeOfUninitializedData;
  ULONG AddressOfEntryPoint;
  ULONG BaseOfCode;
  ULONGLONG ImageBase;
  ULONG SectionAlignment;
  ULONG FileAlignment;
  USHORT MajorOperatingSystemVersion;
  USHORT MinorOperatingSystemVersion;
  USHORT MajorImageVersion;
  USHORT MinorImageVersion;
  USHORT MajorSubsystemVersion;
  USHORT MinorSubsystemVersion;
  ULONG Win32VersionValue;
  ULONG SizeOfImage;
  ULONG SizeOfHeaders;
  ULONG CheckSum;
  USHORT Subsystem;
  USHORT DllCharacteristics;
  ULONGLONG SizeOfStackReserve;
  ULONGLONG SizeOfStackCommit;
  ULONGLONG SizeOfHeapReserve;
  ULONGLONG SizeOfHeapCommit;
  ULONG LoaderFlags;
  ULONG NumberOfRvaAndSizes;
  struct _IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_NT_HEADERS64
{
  ULONG Signature;
  struct _IMAGE_FILE_HEADER FileHeader;
  struct _IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

//
typedef IMAGE_NT_HEADERS64 IMAGE_NT_HEADERS;

#define IMAGE_FIRST_SECTION( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((ULONG_PTR)(ntheader) +                                            \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
     ((ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))
