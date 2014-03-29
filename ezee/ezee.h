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

    enum {
        flag_destroy_iat = 1,
        flag_anti_debug = 10,
        flag_time_trap = 100     
    };

    typedef struct PE {
       _PTR file;
       uint64_t raw_size;
       PIMAGE_DOS_HEADER dos_h;
       PIMAGE_NT_HEADERS64 nt_h;
       char* load_error;
    } *_PE, PE;
    
    #include "ez-pe-loader.h"

#ifdef	__cplusplus
}
#endif

#endif	/* EZEE_H */

