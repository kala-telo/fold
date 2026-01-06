#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "headers.h"
#include "read_helpers.h"

#define da_append(xs, x)                                                       \
    do {                                                                       \
        if ((xs).len + 1 > (xs).cap) {                                         \
            if ((xs).cap != 0) {                                               \
                (xs).cap *= 2;                                                 \
            } else {                                                           \
                (xs).cap = 4;                                                  \
            }                                                                  \
            (xs).data = realloc((xs).data, sizeof(*(xs).data) * (xs).cap);     \
            assert((xs).data != NULL);                                         \
        }                                                                      \
        (xs).data[(xs).len++] = (x);                                           \
    } while (0)

// FIXME: This thing 100% has endianess isssues, those are to be fixed later
typedef struct {
    uint64_t address;
    struct {
        enum : uint32_t {
            //                                Field 	Calculation
            R_X86_64_NONE = 0,             // none      none
            R_X86_64_64 = 1,               // word64    S + A
            R_X86_64_PC32 = 2,             // word32    S + A - P
            R_X86_64_GOT32 = 3,            // word32    G + A
            R_X86_64_PLT32 = 4,            // word32    L + A - P
            R_X86_64_COPY = 5,             // none      none
            R_X86_64_GLOB_DAT = 6,         // wordclass S
            R_X86_64_JUMP_SLOT = 7,        // wordclass S
            R_X86_64_RELATIVE = 8,         // wordclass B + A
            R_X86_64_GOTPCREL = 9,         // word32    G + GOT + A - P
            R_X86_64_32 = 10,              // word32    S + A
            R_X86_64_32S = 11,             // word32    S + A
            R_X86_64_16 = 12,              // word16    S + A
            R_X86_64_PC16 = 13,            // word16    S + A - P
            R_X86_64_8 = 14,               // word8     S + A
            R_X86_64_PC8 = 15,             // word8     S + A - P
            R_X86_64_DTPMOD64 = 16,        // word64
            R_X86_64_DTPOFF64 = 17,        // word64
            R_X86_64_TPOFF64 = 18,         // word64
            R_X86_64_TLSGD = 19,           // word32
            R_X86_64_TLSLD = 20,           // word32
            R_X86_64_DTPOFF32 = 21,        // word32
            R_X86_64_GOTTPOFF = 22,        // word32
            R_X86_64_TPOFF32 = 23,         // word32
            R_X86_64_PC64 = 24,            // word64    S + A - P
            R_X86_64_GOTOFF64 = 25,        // word64    S + A - GOT
            R_X86_64_GOTPC32 = 26,         // word32    GOT + A - P
            R_X86_64_SIZE32 = 32,          // word32    Z + A
            R_X86_64_SIZE64 = 33,          // word64    Z + A
            R_X86_64_GOTPC32_TLSDESC = 34, // word32
            R_X86_64_TLSDESC_CALL = 35,    // none
            R_X86_64_TLSDESC = 36,         // word64×2
            R_X86_64_IRELATIVE = 37,       // wordclass indirect (B + A)
            R_X86_64_RELATIVE64 = 38,      // word64    B + A
            R_X86_64_GOTPCRELX = 41,       // word32    G + GOT + A - P
            R_X86_64_REX_GOTPCRELX = 42,   // word32    G + GOT + A - P
        } type;
        uint32_t sym;
    } info;
    int64_t addend;
} Rela64;

// FIXME: This thing 100% has endianess isssues, those are to be fixed later
typedef struct {
    uint32_t name;
    uint8_t info;
    uint8_t other;
    uint16_t shndx;
    uint64_t value;
    uint64_t size;
} Symtab64;

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
    printf("e_flags: %d\n", header.e_flags);
}

void log_section_header(SectionHeader const *const sections,
                        SectionHeader const *const hdr, size_t names) {
    char *type_names[] = {
        [SHT_NULL] = "SHT_NULL",
        [SHT_PROGBITS] = "SHT_PROGBITS",
        [SHT_SYMTAB] = "SHT_SYMTAB",
        [SHT_STRTAB] = "SHT_STRTAB",
        [SHT_RELA] = "SHT_RELA",
        [SHT_HASH] = "SHT_HASH",
        [SHT_DYNAMIC] = "SHT_DYNAMIC",
        [SHT_NOTE] = "SHT_NOTE",
        [SHT_NOBITS] = "SHT_NOBITS",
        [SHT_REL] = "SHT_REL",
        [SHT_SHLIB] = "SHT_SHLIB",
        [SHT_DYNSYM] = "SHT_DYNSYM",
        [SHT_INIT_ARRAY] = "SHT_INIT_ARRAY",
        [SHT_FINI_ARRAY] = "SHT_FINI_ARRAY",
        [SHT_PREINIT_ARRAY] = "SHT_PREINIT_ARRAY",
        [SHT_GROUP] = "SHT_GROUP",
        [SHT_SYMTAB_SHNDX] = "SHT_SYMTAB_SHNDX",
        [SHT_NUM] = "SHT_NUM",
    };
    printf("|Name: %s\n", &sections[names].data[hdr->name]);
    printf("|Type: %s\n", type_names[hdr->type]);
    printf("|Flags: %x\n", hdr->flags);
    printf("|Address: %lx\n", hdr->address);
    printf("|Offset: %ld\n", hdr->offset);
    printf("|Size: %ld\n", hdr->size);
    printf("|Link: %d\n", hdr->link);
    printf("|Info: %d\n", hdr->info);
    printf("|Address align: %ld\n", hdr->address_align);
    printf("|Entry size: %ld\n", hdr->entry_size);
    switch (hdr->type) {
    case SHT_RELA: {
        const char *const x86_names[] = {
            [R_X86_64_NONE] = "R_X86_64_NONE",
            [R_X86_64_64] = "R_X86_64_64",
            [R_X86_64_PC32] = "R_X86_64_PC32",
            [R_X86_64_GOT32] = "R_X86_64_GOT32",
            [R_X86_64_PLT32] = "R_X86_64_PLT32",
            [R_X86_64_COPY] = "R_X86_64_COPY",
            [R_X86_64_GLOB_DAT] = "R_X86_64_GLOB_DAT",
            [R_X86_64_JUMP_SLOT] = "R_X86_64_JUMP_SLOT",
            [R_X86_64_RELATIVE] = "R_X86_64_RELATIVE",
            [R_X86_64_GOTPCREL] = "R_X86_64_GOTPCREL",
            [R_X86_64_32] = "R_X86_64_32",
            [R_X86_64_32S] = "R_X86_64_32S",
            [R_X86_64_16] = "R_X86_64_16",
            [R_X86_64_PC16] = "R_X86_64_PC16",
            [R_X86_64_8] = "R_X86_64_8",
            [R_X86_64_PC8] = "R_X86_64_PC8",
            [R_X86_64_DTPMOD64] = "R_X86_64_DTPMOD64",
            [R_X86_64_DTPOFF64] = "R_X86_64_DTPOFF64",
            [R_X86_64_TPOFF64] = "R_X86_64_TPOFF64",
            [R_X86_64_TLSGD] = "R_X86_64_TLSGD",
            [R_X86_64_TLSLD] = "R_X86_64_TLSLD",
            [R_X86_64_DTPOFF32] = "R_X86_64_DTPOFF32",
            [R_X86_64_GOTTPOFF] = "R_X86_64_GOTTPOFF",
            [R_X86_64_TPOFF32] = "R_X86_64_TPOFF32",
            [R_X86_64_PC64] = "R_X86_64_PC64",
            [R_X86_64_GOTOFF64] = "R_X86_64_GOTOFF64",
            [R_X86_64_GOTPC32] = "R_X86_64_GOTPC32",
            [R_X86_64_SIZE32] = "R_X86_64_SIZE32",
            [R_X86_64_SIZE64] = "R_X86_64_SIZE64",
            [R_X86_64_GOTPC32_TLSDESC] = "R_X86_64_GOTPC32_TLSDESC",
            [R_X86_64_TLSDESC_CALL] = "R_X86_64_TLSDESC_CALL",
            [R_X86_64_TLSDESC] = "R_X86_64_TLSDESC",
            [R_X86_64_IRELATIVE] = "R_X86_64_IRELATIVE",
            [R_X86_64_RELATIVE64] = "R_X86_64_RELATIVE64",
            [R_X86_64_GOTPCRELX] = "R_X86_64_GOTPCRELX",
            [R_X86_64_REX_GOTPCRELX] = "R_X86_64_REX_GOTPCRELX",
        };
        Rela64 *relas = (void*)hdr->data;
        assert(hdr->entry_size == sizeof(Rela64));
        for (size_t i = 0; i < hdr->size/hdr->entry_size; i++) {
            Rela64 *rela = &relas[i];
            SectionHeader symnames = sections[hdr->link];
            Symtab64 symtab = ((Symtab64*)symnames.data)[rela->info.sym];
            printf("|- Rela `%s' [%d]: %s at %lx (%ld)\n",
                   &sections[symnames.link].data[symtab.name],
                   rela->info.sym,
                   x86_names[rela->info.type],
                   rela->address,
                   rela->addend);
        }
    } break;
    case SHT_SYMTAB: {
        Symtab64 *symbols = (Symtab64*)hdr->data;
        for (size_t i = 0; i < hdr->size/hdr->entry_size; i++) {
            Symtab64 sym = symbols[i];
            printf("|- Symtab: name: `%s'; info: %d; other: %d; shndx: %d; "
                   "value: %ld; size: %ld\n",
                   &sections[hdr->link].data[sym.name], sym.info, sym.other,
                   sym.shndx, sym.value, sym.size);
        }
    } break;
    default:
        break;
    }
    printf("|---------------\n");
}

SectionHeader read_section_header(FILE *f, Width w, Endianess e) {
    SectionHeader hdr = {0};
    hdr.name = read32(f, e);
    hdr.type = read32(f, e);
    hdr.flags = read32or64(f, w, e);
    hdr.address = read32or64(f, w, e);
    hdr.offset = read32or64(f, w, e);
    hdr.size = read32or64(f, w, e);
    hdr.link = read32(f, e);
    hdr.info = read32(f, e);
    hdr.address_align = read32or64(f, w, e);
    hdr.entry_size = read32or64(f, w, e);
    if (hdr.size) {
        hdr.data = malloc(hdr.size);
        fseek(f, hdr.offset, SEEK_SET);
        fread(hdr.data, hdr.size, 1, f);
    }
    return hdr;
}

Header read_header(FILE *f) {
    uint32_t magic = read32b(f);
    if (magic != 0x7f454c46) {
        fprintf(stderr, "Invalid magic (%x)\n", magic);
        exit(1);
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
    hdr.header_size = read16(f, e);
    hdr.program_header_size = read16(f, e);
    hdr.prog_header_entries_cnt = read16(f, e);
    if (hdr.program_header_size != 0)
        abort();
    hdr.section_header_size = read16(f, e);
    hdr.section_header_entries_cnt = read16(f, e);
    hdr.section_name_idx = read16(f, e);
    return hdr;
}

int main() {
    FILE *f = fopen("test.o", "rb");
    Header hdr = read_header(f);
    Endianess e = hdr.endianess;
    Width w = hdr.width;
    fseek(f, hdr.section_header_off + hdr.section_name_idx * hdr.section_header_size,
          SEEK_SET);
    SectionHeader names = read_section_header(f, w, e);
    fseek(f, hdr.section_header_off, SEEK_SET);
    struct {
        SectionHeader* data;
        size_t len, cap;
    } sections = {0};
    for (size_t i = 0; i < hdr.section_header_entries_cnt; i++) {
        long start = ftell(f);
        SectionHeader sec_hdr = read_section_header(f, w, e);
        fseek(f, start + hdr.section_header_size, SEEK_SET);
        da_append(sections, sec_hdr);
    }
    for (size_t i = 0; i < sections.len; i++) {
        SectionHeader sec_hdr = sections.data[i];
        log_section_header(sections.data, &sec_hdr, hdr.section_name_idx);
    }
    fclose(f);
    log_header(hdr);
}
