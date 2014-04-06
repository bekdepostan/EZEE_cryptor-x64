#include <stdio.h>
#include "ezee.h"

long pad_p_to_n(long p, long n) {
    long i = 0;
    while(i < p) {
        i += n;
    }
    return i;
}

long size_of_lib_ezpak() {
    long dll_size  = 0;
    long stub_size = ep_stub();
    return dll_size + stub_size;
}

boolean crush_sections(_PE target) {
    long section_raw = 0;
    long section_virt = 0;
    int section_flags = 0xE40000E0;
    long ez_size = size_of_lib_ezpak();
    PIMAGE_SECTION_HEADER first_section = target->s_h;
    PIMAGE_SECTION_HEADER final_section = target->s_h;
    final_section = (void*)final_section + ((target->nt_h->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER)) - sizeof(IMAGE_SECTION_HEADER));
    
    if(IsBadReadPtr(final_section, sizeof(IMAGE_SECTION_HEADER)) || ez_size == 0) {
        return FALSE;
    }
    
    section_raw = final_section->SizeOfRawData + final_section->PointerToRawData;
    //section_raw = pad_p_to_n(final_section->SizeOfRawData + ez_size, 0x200);
    section_raw -= first_section->PointerToRawData;
    
    section_virt = final_section->VirtualAddress + final_section->Misc.VirtualSize;
    section_virt += ez_size;
    section_virt -= first_section->VirtualAddress;
    
    PIMAGE_SECTION_HEADER destroy_section = target->s_h;
    for(int i = 1; i < target->nt_h->FileHeader.NumberOfSections; i++) {
        destroy_section = (void*)first_section + (i * sizeof(IMAGE_SECTION_HEADER));
        memset(destroy_section, 'Z', sizeof(IMAGE_SECTION_HEADER));
    }
    
    // Build crushed section
    target->nt_h->FileHeader.NumberOfSections = 1;
    
    first_section->Characteristics = section_flags;
    first_section->Misc.VirtualSize = section_virt;
    memcpy(first_section->Name, ".ezee\0\0\0", 8);
    first_section->SizeOfRawData = section_raw;
}