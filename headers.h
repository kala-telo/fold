#ifndef HEADERS_H
#define HEADERS_H

#include <stdint.h>

typedef enum {
    ENDIANESS_LITTLE = 1,
    ENDIANESS_BIG = 2,
} Endianess;
typedef enum {
    WIDTH_32 = 1,
    WIDTH_64 = 2,
} Width;

typedef struct {
    Width width;
    Endianess endianess;
    uint8_t version;
    enum {
        ABI_SystemV       = 0x00,
        ABI_HPUX          = 0x01,
        ABI_NetBSD        = 0x02,
        ABI_Linux         = 0x03,
        ABI_Hurd          = 0x04,
        ABI_Solaris       = 0x06,
        ABI_AIX           = 0x07,
        ABI_IRIX          = 0x08,
        ABI_FreeBSD       = 0x09,
        ABI_Tru64         = 0x0A,
        ABI_NovellModesto = 0x0B,
        ABI_OpenBSD       = 0x0C,
        ABI_OpenVMS       = 0x0D,
        ABI_NonStop       = 0x0E,
        ABI_AROS          = 0x0F,
        ABI_FenixOS       = 0x10,
        ABI_NuxiCloud     = 0x11,
        ABI_OpenVOS       = 0x12,
    } abi;
    uint8_t abi_version;
    enum {
        // Unknown
        ET_NONE = 0x0000,
        // Relocatable file
        ET_REL = 0x0001,
        // Executable file
        ET_EXEC = 0x0002,
        // Shared object
        ET_DYN = 0x0003,
        // Core file
        ET_CORE = 0x0004,
        // Reserved inclusive range
        ET_LOOS = 0xFE00,
        // Operating system specific
        ET_HIOS = 0xFEFF,
        // Reserved inclusive range
        ET_LOPROC = 0xFF00,
        // Processor specific
        ET_HIPROC = 0xFFFF,
    } file_type;
    enum {
        // TODO: populate further
        ISA_NONE  = 0x0000,
        ISA_X86   = 0x0003,
        ISA_PPC   = 0x0014,
        ISA_PPC64 = 0x0015,
        ISA_AMD64 = 0x003E,
    } isa;
    uint32_t e_version;
    uint64_t entry_addr;
    uint32_t e_flags;
    uint64_t program_header_off;
    uint64_t section_header_off;
    uint16_t header_size;
    uint16_t program_header_size;
    uint16_t prog_header_entries_cnt;
    uint16_t section_header_size;
    uint16_t section_header_entries_cnt;
    uint16_t section_name_idx;
} Header;

typedef struct {
    uint64_t name;
    enum {
        // Section header table entry unused
        SHT_NULL = 0x0,
        // Program data
        SHT_PROGBITS = 0x1,
        // Symbol table
        SHT_SYMTAB = 0x2,
        // String table
        SHT_STRTAB = 0x3,
        // Relocation entries with addends
        SHT_RELA = 0x4,
        // Symbol hash table
        SHT_HASH = 0x5,
        // Dynamic linking information
        SHT_DYNAMIC = 0x6,
        // Notes
        SHT_NOTE = 0x7,
        // Program space with no data (bss)
        SHT_NOBITS = 0x8,
        // Relocation entries, no addends
        SHT_REL = 0x9,
        // Reserved
        SHT_SHLIB = 0x0A,
        // Dynamic linker symbol table
        SHT_DYNSYM = 0x0B,
        // Array of constructors
        SHT_INIT_ARRAY = 0x0E,
        // Array of destructors
        SHT_FINI_ARRAY = 0x0F,
        // Array of pre-constructors
        SHT_PREINIT_ARRAY = 0x10,
        // Section group
        SHT_GROUP = 0x11,
        // Extended section indices
        SHT_SYMTAB_SHNDX = 0x12,
        // Number of defined types.
        SHT_NUM = 0x13,
    } type;
    enum {
        SHF_WRITE = 0x1,
        SHF_ALLOC = 0x2,
        SHF_EXECINSTR = 0x4,
        SHF_MERGE = 0x10,
        SHF_STRINGS = 0x20,
        SHF_INFO_LINK = 0x40,
        SHF_LINK_ORDER = 0x80,
        SHF_OS_NONCONFORMING = 0x100,
        SHF_GROUP = 0x200,
        SHF_TLS = 0x400,
        SHF_COMPRESSED = 0x800,
        SHF_MASKOS = 0x0ff00000,
        SHF_MASKPROC = 0xf0000000,
    } flags;
    uint64_t address;
    uint64_t offset;
    uint64_t size;
    uint32_t info;
    uint64_t address_align;
    uint64_t entry_size;
    // related section
    uint32_t link;
    // data read from the section
    char *data;
} SectionHeader;
#endif
