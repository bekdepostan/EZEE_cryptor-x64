#include <stdio.h>
#include "ezee.h"

_PE load_file(char *filename) {
    // Allocate resources
    _PE pe_file = (_PE)malloc(sizeof(PE));
    memset((_PTR)pe_file, 0, sizeof(PE));
    
    // Check path
    FILE* f_handle = fopen(filename, "rb");
    if(f_handle == NULL) {
        pe_file->load_error = "Failed to open file, check filename and privilege.\n";
        return pe_file;
    }
    
    // Read the file
    fseek(f_handle, 0, SEEK_END);
    pe_file->raw_size = ftell(f_handle);
    rewind(f_handle);
    
    pe_file->file = (_PTR)malloc(pe_file->raw_size);
    memset((_PTR)pe_file->file, 0, pe_file->raw_size);
    
    fread(pe_file->file, pe_file->raw_size, 1, f_handle);
    fclose (f_handle);
    
    // Validate the format and load DOS header
    pe_file->dos_h = pe_file->file;
    if(pe_file->dos_h->e_magic != 0x5A4D) {
        pe_file->load_error = "Failed to load file, no valid DOS headers found.\n";
        return pe_file;
    }
    
    // Validate the format and load NT header
    pe_file->nt_h = pe_file->file + pe_file->dos_h->e_lfanew;
    if(pe_file->nt_h->Signature != 0x4550) {
        pe_file->load_error = "Failed to load file, no valid NT headers found.\n";
        return pe_file;
    }
    
    if(pe_file->nt_h->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        pe_file->load_error = "Failed to load file, no valid 64-bit headers found.\n";
        return pe_file;
    }
    
    pe_file->s_h = IMAGE_FIRST_SECTION(pe_file);
    return pe_file;
}


char* save_file(_PE towrite, char *filename) {
    char* out_append = "_packed.exe";
    char* out_filename = malloc(strlen(filename) + strlen(out_append) + 1);
    
    strcpy(out_filename, filename);
    strcat(out_filename, out_append);
    
    FILE* f_handle = fopen(out_filename, "wb+");
    
    if(f_handle == NULL) {
        return "Failed to create new executable on disk, check privilege.\n";
    }
    
    fwrite(towrite->file, towrite->raw_size, 1, f_handle);
    fclose(f_handle);
    
    free(out_filename);
    return "";
}

void release_file(_PE file) {
    free(file->file);
    free(file);
}
