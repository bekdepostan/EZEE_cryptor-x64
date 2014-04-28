#ifndef MAIN_DLL_H
#define MAIN_DLL_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    void __stdcall __declspec(dllexport) ez_main(void* caller_memory);

    #ifdef __cplusplus
    }
    #endif

#endif 