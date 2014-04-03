#include <stdio.h>
#include "ezee.h"

boolean crush_sections(_PE target) {
    long section_raw = 0;
    long section_virt = 0;
    PIMAGE_SECTION_HEADER section;
    for(int i = 0; i < target->nt_h->FileHeader.NumberOfSections; i++) {
        section = target->s_h + (sizeof(IMAGE_SECTION_HEADER) * i);
        
    }
    
}