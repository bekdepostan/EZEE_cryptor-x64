#include "ezee.h"

// The ep_stub serves to load the lib-ezpak into memory and execute stage 2 functions.
long ep_stub() {
    __asm("jmp do_size_calc");
    __asm(".int 0x111333FF"); // begin copy flag for stub writer 
    ep_stub_begin:
    
    __asm("jmp over_ez_sig");
    __asm(".ascii \"EZPK\"");
    __asm("over_ez_sig:");
    __asm("jmp *(0x0000000011111111)");
    __asm("nop");
    
    ep_stub_end:
    __asm(".int 0xFF333111"); // end copy flag for stub writer 
    __asm("do_size_calc:");
    return &&ep_stub_end - &&ep_stub_begin; // calculate size of dll loader stub
}

long iat_size() {
    long sizeOfId = sizeof(IMAGE_IMPORT_DESCRIPTOR);
    
    // Really? why isn't this KERNEL64 on 64-bit systems! X|
    // Guess change is hard.
    long sizeOfDLLSz = strlen("KERNEL32.DLL") + 1;
    long sizeOfImp1 = strlen("LoadLibraryA") + 1;
    
    // Also... why no GetProcAddress(A/W) ??? 
    long sizeOfImp2 = strlen("GetProcAddress") + 1;
    
    return sizeOfId + sizeOfDLLSz + sizeOfImp1 + sizeOfImp2;
}