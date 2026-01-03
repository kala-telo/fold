#include <stdio.h>
#include <stdint.h>
#include <endian.h>
#include <stdlib.h>

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
    uint64_t program_header_off;
    uint64_t section_header_off;
    uint32_t e_flags;
} Header;

uint8_t read8(FILE *f) {
    uint8_t b;
    fread(&b, 1, 1, f);
    return b;
}
static inline uint16_t read16l(FILE *f) {
    uint16_t b;
    fread(&b, 2, 1, f);
    return le16toh(b);
}
static inline uint16_t read16b(FILE *f) {
    uint16_t b;
    fread(&b, 2, 1, f);
    return be16toh(b);
}
static inline uint16_t read16(FILE *f, Endianess e) {
    switch (e) {
    case ENDIANESS_BIG:
        return read16b(f);
    case ENDIANESS_LITTLE:
        return read16l(f);
    }
    abort();
    return -1;
}
static inline uint32_t read32l(FILE *f) {
    uint32_t b;
    fread(&b, 4, 1, f);
    return le32toh(b);
}
static inline uint32_t read32b(FILE *f) {
    uint32_t b;
    fread(&b, 4, 1, f);
    return be32toh(b);
}
static inline uint32_t read32(FILE *f, Endianess e) {
    switch (e) {
    case ENDIANESS_BIG:
        return read32b(f);
    case ENDIANESS_LITTLE:
        return read32l(f);
    }
    abort();
    return -1;
}
static inline uint64_t read64l(FILE *f) {
    uint64_t b;
    fread(&b, 8, 1, f);
    return le64toh(b);
}
static inline uint64_t read64b(FILE *f) {
    uint64_t b;
    fread(&b, 8, 1, f);
    return be64toh(b);
}
static inline uint64_t read64(FILE *f, Endianess e) {
    switch (e) {
    case ENDIANESS_BIG:
        return read64b(f);
    case ENDIANESS_LITTLE:
        return read64l(f);
    }
    abort();
    return -1;
}

static inline uint64_t read32or64(FILE *f, Width w, Endianess e) {
    return (w == WIDTH_32 ? read32(f, e) : read64(f, e));
}

void log_header(Header header) {
    char *abi_names[] = {
        [ABI_SystemV]       = "SystemV",
        [ABI_HPUX]          = "HPUX",
        [ABI_NetBSD]        = "NetBSD",
        [ABI_Linux]         = "Linux",
        [ABI_Hurd]          = "Hurd",
        [ABI_Solaris]       = "Solaris",
        [ABI_AIX]           = "AIX",
        [ABI_IRIX]          = "IRIX",
        [ABI_FreeBSD]       = "FreeBSD",
        [ABI_Tru64]         = "Tru64",
        [ABI_NovellModesto] = "NovellModesto",
        [ABI_OpenBSD]       = "OpenBSD",
        [ABI_OpenVMS]       = "OpenVMS",
        [ABI_NonStop]       = "NonStop",
        [ABI_AROS]          = "AROS",
        [ABI_FenixOS]       = "FenixOS",
        [ABI_NuxiCloud]     = "NuxiCloud",
        [ABI_OpenVOS]       = "OpenVOS",
    };
    char *etype_names[] = {
        [ET_NONE]   = "ET_NONE", [ET_REL]    = "ET_REL",
        [ET_EXEC]   = "ET_EXEC", [ET_DYN]    = "ET_DYN",
        [ET_CORE]   = "ET_CORE", [ET_LOOS]   = "ET_LOOS",
        [ET_HIOS]   = "ET_HIOS", [ET_LOPROC] = "ET_LOPROC",
        [ET_HIPROC] = "ET_HIPROC",
    };
    char *isa_names[] = {
        [ISA_NONE]  = "none",    [ISA_X86]   = "x86",
        [ISA_PPC]   = "PowerPC", [ISA_PPC64] = "PowerPC 64",
        [ISA_AMD64] = "AMD64",
    };
    printf("VERSION: %d\n", header.version);
    printf("CLASS: %d\n", header.width == WIDTH_32 ? 32 : 64);
    printf("DATA: %s\n", header.endianess == ENDIANESS_BIG ? "big" : "little");
    printf("OSABI: %s\n", abi_names[header.abi]);
    printf("ABIVERSION: %d\n", header.abi_version);
    printf("e_type: %s\n", etype_names[header.file_type]);
    printf("isa: %s\n", isa_names[header.isa]);
    printf("e_version: %d\n", header.e_version);
    printf("entry_addr: %lx\n", header.entry_addr);
    printf("e_phoff: %lx\n", header.program_header_off);
    printf("e_shoff: %lx\n", header.section_header_off);
    printf("e_flags: %d\n", header.e_flags);
}

int main() {
    FILE *f = fopen("test.o", "rb");
    uint32_t magic = read32b(f);
    if (magic != 0x7f454c46) {
        fprintf(stderr, "Invalid magic (%x)\n", magic);
    }
    Header hdr = {0};
    hdr.width = read8(f);
    hdr.endianess = read8(f);
    hdr.version = read8(f);
    hdr.abi = read8(f);
    hdr.abi_version = read8(f);
    for (int i = 0; i < 7; i++)
        read8(f);
    Endianess e = hdr.endianess;
    hdr.file_type = read16(f, e);
    hdr.isa = read16(f, e);
    hdr.e_version = read32(f, e);
    Width w = hdr.width;
    hdr.entry_addr = read32or64(f, w, e);
    hdr.program_header_off = read32or64(f, w, e);
    hdr.section_header_off = read32or64(f, w, e);
    hdr.e_flags = read32(f, e);
    uint16_t header_size = read16(f, e);
    uint16_t program_header_size = read16(f, e);
    uint16_t prog_header_entries_cnt = read16(f, e);
    uint16_t section_header_size = read16(f, e);
    uint16_t section_header_entries_cnt = read16(f, e);
    fclose(f);
    log_header(hdr);
}
