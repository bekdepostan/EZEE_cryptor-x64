#include "ezee.h"

// The ep_stub serves to load the lib-ezpak into memory and execute stage 2 functions.
unsigned long long ep_stub(int action) {
    
    // This skips the actual stub itself when code injector just wants information about it
    __asm("jmp do_calc");
    ep_stub_begin:
    
    // Relative positioning is a pain in the arse
    __asm("call get_eip");
    __asm("get_eip:");
    __asm("pop %rdi");
    __asm("addq $7, %rdi");
    
    // Store data we need to load DLL functions here
    // This is how we load functions without exposing them in the import directory
    __asm("jmp ez_dat_fin");
    __asm("begin_ez_dat:");
    __asm(".asciz \"Kernel32.dll\"");
    __asm(".asciz \"VirtualAlloc\"");
    __asm("ez_dat_fin:");
    
    // edi is pointer to ez_data - save it
    __asm("push %rdi");
    __asm("movq %rdi, %rcx");
    
    // rsi will point to thunks
    __asm("movq %rdi, %rsi");
    __asm("addq $stub_fin - begin_ez_dat, %rsi");
    __asm("addq $64, %rsi");
    __asm("callq *(%rsi)");
    
    // pass VirtualAlloc asciz and module handle for kernel
    __asm("movq %rax, %rcx");
    __asm("movq (%esp), %rdx");
    __asm("addq $13, %rdx");
    __asm("callq *8(%rsi)");
    
    // Find and size lib_ez
    __asm("movq %rdi, %rax");
    __asm("addq $stub_fin - begin_ez_dat, %rax");
    __asm("addq $133, %rsi");
    
    // Alloc some memory
    // Todo: new calling conventions
    __asm("push $0");
    __asm("push $0x5000");
    __asm("push $0x3000");
    __asm("push $0x3000");
    __asm("callq *(%rax)");    
    
    __asm("stub_fin:");

    ep_stub_end:
    
    __asm("do_calc:");
    if(action == EP_GET_SIZE) {
        return &&ep_stub_end - &&ep_stub_begin; // calculate size of dll loader stub
    } else {
        return (unsigned long long)&&ep_stub_begin;
    }
}

// TODO: why is this shit all camel case?? I was tired I guess
unsigned long iat_size() {
    long sizeOfId = (sizeof(IMAGE_IMPORT_DESCRIPTOR) * 2) + (sizeof(IMAGE_THUNK_DATA64) * 6);
    
    // Really? why isn't this KERNEL64 on 64-bit systems! X|
    // The + 3 is for null terminator and the hint word in IMAGE_IMPORT_BY_NAME
    long sizeOfDLLSz = strlen("KERNEL32.DLL") + 1;
    long sizeOfImp1Sz = strlen("LoadLibraryA") + 3;
    
    // Also... interesting no GetProcAddress(A/W)
    long sizeOfImp2Sz = strlen("GetProcAddress") + 3;
    
    return sizeOfId + sizeOfDLLSz + sizeOfImp1Sz + sizeOfImp2Sz;
}

// TODO -- use offsetof and structify this :0
void build_import_directory(_PTR dest_base, unsigned long base_rva) {
    DWORD image_import_by_names_base = base_rva + (sizeof(IMAGE_IMPORT_DESCRIPTOR) * 2) + (sizeof(IMAGE_THUNK_DATA64) * 6);
    
    PIMAGE_THUNK_DATA64 o_first_thunk = dest_base + (sizeof(IMAGE_IMPORT_DESCRIPTOR) * 2);
    o_first_thunk->u1.AddressOfData = image_import_by_names_base;
    o_first_thunk = (_PTR)o_first_thunk + sizeof(IMAGE_THUNK_DATA64);
    o_first_thunk->u1.AddressOfData = image_import_by_names_base + 3 + strlen("LoadLibraryA");
    
    PIMAGE_THUNK_DATA64 first_thunk = (_PTR)o_first_thunk + (sizeof(IMAGE_THUNK_DATA64) * 2);
    first_thunk->u1.AddressOfData = image_import_by_names_base;
    first_thunk = (_PTR)first_thunk + sizeof(IMAGE_THUNK_DATA64);
    first_thunk->u1.AddressOfData = image_import_by_names_base + 3 + strlen("LoadLibraryA");
    
    // These are my IMAGE_IMPORT_BY_NAME structures -- there is no way to represent in a struct
    _PTR import_by_name_ll = (_PTR)first_thunk + (sizeof(IMAGE_THUNK_DATA64) * 2);
    *((WORD*)import_by_name_ll) = 0;
    strcpy(import_by_name_ll + 2, "LoadLibraryA");
    
    _PTR import_by_name_gpa = import_by_name_ll + strlen("LoadLibraryA") + 3;
    *((WORD*)import_by_name_gpa) = 0;
    strcpy(import_by_name_gpa + 2, "GetProcAddress");
    
    _PTR import_k64 = import_by_name_gpa + strlen("GetProcAddress") + 3; 
    strcpy(import_k64, "KERNEL32.DLL");
    
    PIMAGE_IMPORT_DESCRIPTOR kernel_desc = dest_base;
    kernel_desc->OriginalFirstThunk = base_rva + (sizeof(IMAGE_IMPORT_DESCRIPTOR) * 2);
    kernel_desc->FirstThunk = kernel_desc->OriginalFirstThunk + (sizeof(IMAGE_THUNK_DATA64) * 3);
    kernel_desc->Name = base_rva + (import_k64 - dest_base);
    kernel_desc->ForwarderChain = -1;
    kernel_desc->TimeDateStamp = -1;
}