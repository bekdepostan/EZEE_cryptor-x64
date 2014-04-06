// The ep_stub serves to load the lib-ezpak into memory and execute stage 2 functions.
long ep_stub() {
    __asm("jmp do_size_calc");
    __asm(".int 0x111333FF"); // begin copy flag for stub writer 
    ep_stub_begin:
    
    __asm("jmp over_ez_sig");
    __asm(".ascii \"EZPK\"");
    __asm("over_ez_sig:");
    __asm("jmpq *(0x0000000011111111)");
    __asm("nop");
    
    ep_stub_end:
    __asm(".int 0xFF333111"); // end copy flag for stub writer 
    __asm("do_size_calc:");
    return &&ep_stub_end - &&ep_stub_begin; // calculate size of dll loader stub
}
