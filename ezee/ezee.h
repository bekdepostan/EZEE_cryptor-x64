/* 
 * File:   ezee.h
 * Author: Darius
 *
 * Created on March 23, 2014, 4:12 PM
 */


#ifndef EZEE_H
#define	EZEE_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    #include <windows.h>
    #include <stdint.h>

    #ifndef _PTR
    typedef void* _PTR;
    #endif

    enum {
        flag_destroy_iat = 1,
        flag_anti_debug = 10,
        flag_time_trap = 100     
    };

    typedef struct PE {
       _PTR file;
       uint64_t raw_size;
       
       // inject_ptr is empty until crush_sections finishes section size calculations
       uint64_t inject_ptr_raw; 
       
       PIMAGE_DOS_HEADER dos_h;
       PIMAGE_NT_HEADERS64 nt_h;
       PIMAGE_SECTION_HEADER s_h;
       char* load_error;
    } *_PE, PE;
    
    #include "loader.h"
    #include "ep_stub.h"
    #include "utl.h"

#ifdef	__cplusplus
}
#endif

#endif	/* EZEE_H */

