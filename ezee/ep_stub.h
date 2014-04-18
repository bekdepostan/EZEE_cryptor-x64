/* 
 * File:   ep_stub.h
 * Author: Darius
 *
 * Created on April 6, 2014, 10:27 AM
 */

#ifndef EP_STUB_H
#define	EP_STUB_H

#ifdef	__cplusplus
extern "C" {
#endif

    #define EP_GET_SIZE 1
    #define EP_GET_PTR 0
    unsigned long long ep_stub(int action);
    unsigned long iat_size();
    void build_import_directory(_PTR dest_base, unsigned long base_rva);

#ifdef	__cplusplus
}
#endif

#endif	/* EP_STUB_H */

