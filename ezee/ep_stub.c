#include "ezee.h"

// The ep_stub serves to load the lib-ezpak into memory and execute stage 2 functions.
unsigned long long ep_stub(int action) {
    
    // This prevents stub from executing during information gathering
    __asm("jmp stub_fin");
    ep_stub_begin:
    
    // Relative positioning is a pain in the arse
    __asm("call get_eip");
    __asm("get_eip:");
    __asm("pop %rdi");
    __asm("addq $7, %rdi");
    
    // Store data we need to load DLL functions here
    // This is how a person might load functions without exposing them in the import directory
    __asm("jmp ez_dat_fin");
    __asm("begin_ez_dat:");
    __asm(".asciz \"Kernel32.dll\"");
    __asm(".asciz \"VirtualAlloc\"");
    __asm("ez_dat_fin:");
    __asm("movq %rdi, %rcx");
    
    // rsi will point to thunks
    __asm("movq %rdi, %rsi");
    __asm("addq $stub_fin - begin_ez_dat, %rsi");
    __asm("addq $64, %rsi");
    __asm("callq *(%rsi)");
    
    // pass asciz and module handle to get VirtualAlloc's address
    __asm("movq %rax, %rcx");
    __asm("movq %rdi, %rdx");
    __asm("addq $13, %rdx");
    __asm("callq *8(%rsi)");
    __asm("movq %rax, %rbp");
    
    // Find lib_ez
    __asm("movq %rdi, %rax");
    __asm("addq $stub_fin - begin_ez_dat, %rax");
    
    // Import directory size is 133, offset to e_lfanew is 60
    __asm("addq $133, %rax");
    __asm("mov %rax, %r9");
    __asm("addq $60, %r9");
    __asm("xor %rbx, %rbx");
    __asm("movzwl (%r9), %ebx");
    __asm("addq %rbx, %rax");
    
    // Get number of sections and seek last section
    // Size of nt headers is 264, size of section header is 40
    __asm("xor %rdx, %rdx");
    __asm("movzwl 6(%rax), %edx");
    __asm("push %rax");
    __asm("movq $40, %rax");
    __asm("mul %rdx");
    __asm("movq %rax, %rbx");
    __asm("pop %rax");
    __asm("addq %rax, %rbx");
    __asm("addq $264 - 40, %rbx");
    
    // Find file size of lib_ez and alloc memory
    __asm("xor %rcx, %rcx");
    __asm("xor %rdx, %rdx");
    __asm("movl 16(%rbx), %edx");
    __asm("addl 20(%rbx), %edx");
    __asm("movq %rdx, %rsi");
    __asm("movq $0x3000, %r8");
    __asm("movq $0x40, %r9");
    __asm("callq %rbp"); 
    
    // move DLL to allocated space
    __asm("movq %rax, %r9");
    __asm("movq %rdi, %rax");
    __asm("addq $stub_fin - begin_ez_dat, %rax");
    __asm("addq $133, %rax");
    
    // save ptr to relocated lib_ez --------------------------------------------
    __asm("push %rax");
    __asm("copy_more:");
    __asm("movzbl (%rax), %ebx");
    __asm("movb %bl, (%r9)");
    __asm("dec %rsi");
    __asm("inc %r9");
    __asm("inc %rax");
    __asm("cmp $0, %rsi");
    __asm("jg copy_more");
    __asm("pop %rax");
    
    // apply relocation deltas
    
    
    
    // resolve import directory
    
    
    // fuck this stupid DLL, it has TLS and an exception dir and shit
    // I don't know how to deal with that shit
    
    
    // call stage 2 initialization 
    
    
    
    // if we end up back here the target actually returned to us holy shit.
    // ... I guess just exit success here???
    __asm("mov $0, %rax"); 
    __asm("ret"); 
    __asm("stub_fin:");
    ep_stub_end:
    
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