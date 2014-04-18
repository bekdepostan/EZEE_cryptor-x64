#ifndef UTL_H
#define	UTL_H

#ifdef	__cplusplus
extern "C" {
#endif

boolean crush_sections(_PE target, _PE lib_ez);
unsigned long pad_p_to_n(long p, long n);
void create_stub_space(_PE target);
void inject_stub(_PE target, _PE lib_ez);
void pe_fixup(_PE target);

#ifdef	__cplusplus
}
#endif

#endif	/* UTL_H */

