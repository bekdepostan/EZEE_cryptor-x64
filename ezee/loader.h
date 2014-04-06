#ifndef EZ_PE_LOADER_H
#define	EZ_PE_LOADER_H

#ifdef	__cplusplus
extern "C" {
#endif

_PE load_file(char *filename); 
void release_file(_PE file);
char* save_file(_PE towrite, char *filename);
_PE reload_in_mem(_PE pe_file);

#ifdef	__cplusplus
}
#endif

#endif	/* EZ_PE_LOADER_H */

