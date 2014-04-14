#include "ezee.h"

// The ep_stub serves to load the lib-ezpak into memory and execute stage 2 functions.
unsigned long ep_stub(int action) {
    __asm("jmp do_calc");
    ep_stub_begin:
    
    __asm("jmp over_ez_sig");
    __asm(".ascii \"EZPK\"");
    __asm("over_ez_sig:");
    __asm("jmp *(0x0000000011111111)");
    __asm("nop");
    
    ep_stub_end:
    __asm("do_calc:");
    if(action == EP_GET_SIZE) {
        return &&ep_stub_end - &&ep_stub_begin; // calculate size of dll loader stub
    } else {
        return (unsigned long)&&ep_stub_begin;
    }
}

// TODO: why is this shit all camel case?? I was tired I guess
unsigned long iat_size() {
    long sizeOfId = (sizeof(IMAGE_IMPORT_DESCRIPTOR) * 2) + (sizeof(IMAGE_THUNK_DATA64) * 4);
    
    // Really? why isn't this KERNEL64 on 64-bit systems! X|
    // The + 3 is for null terminator and the hint word in IMAGE_IMPORT_BY_NAME
    long sizeOfDLLSz = strlen("KERNEL32.DLL") + 1;
    long sizeOfImp1Sz = strlen("LoadLibraryA") + 3;
    
    // Also... interesting no GetProcAddress(A/W)
    long sizeOfImp2Sz = strlen("GetProcAddress") + 3;
    
    return sizeOfId + sizeOfDLLSz + sizeOfImp1Sz + sizeOfImp2Sz;
}

void build_iat(_PTR dest_base, unsigned long base_rva) {
    DWORD image_import_by_names_base = base_rva + (sizeof(IMAGE_IMPORT_DESCRIPTOR) * 2) + (sizeof(IMAGE_THUNK_DATA64) * 4);
    
    PIMAGE_THUNK_DATA64 o_first_thunk = dest_base + (sizeof(IMAGE_IMPORT_DESCRIPTOR) * 2);
    o_first_thunk->u1.AddressOfData = image_import_by_names_base + 2;
    o_first_thunk++;
    o_first_thunk->u1.AddressOfData = image_import_by_names_base + 4 + strlen("LoadLibraryA");
    
    PIMAGE_THUNK_DATA64 first_thunk = o_first_thunk + sizeof(IMAGE_THUNK_DATA64);
    first_thunk->u1.AddressOfData = image_import_by_names_base + 2;
    first_thunk++;
    first_thunk->u1.AddressOfData = image_import_by_names_base + 4 + strlen("LoadLibraryA");
    
    // These are my IMAGE_IMPORT_BY_NAME structures -- there is no way to represent in a struct ???
    _PTR import_by_name_ll = first_thunk  + sizeof(IMAGE_THUNK_DATA64);
    *((WORD*)import_by_name_ll) = -1;
    strcpy(import_by_name_ll + 2, "LoadLibraryA");
    
    _PTR import_by_name_gpa = import_by_name_ll + strlen("LoadLibraryA") + 3;
    *((WORD*)import_by_name_gpa) = -1;
    strcpy(import_by_name_gpa + 2, "GetProcAddress");
    
    _PTR import_k64 = import_by_name_gpa + strlen("GetProcAddress") + 3; 
    strcpy(import_k64, "KERNEL32.DLL");
    
    PIMAGE_IMPORT_DESCRIPTOR kernel_desc = dest_base;
    kernel_desc->OriginalFirstThunk = base_rva + (sizeof(IMAGE_IMPORT_DESCRIPTOR) * 2);
    kernel_desc->FirstThunk = kernel_desc->OriginalFirstThunk + (sizeof(IMAGE_THUNK_DATA64) * 2);
    kernel_desc->Name = base_rva + (import_k64 - dest_base);
    kernel_desc->ForwarderChain = -1;
    kernel_desc->TimeDateStamp = -1;
}