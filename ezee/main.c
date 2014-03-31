#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ezee.h"

int get_args(int argc, char** argv) {
    uint16_t flags, i;
    for(i = 2; i < argc; i++) {
        if(strcmp(argv[i], "-i")) {
            flags += flag_destroy_iat;
        } else if(strcmp(argv[i], "-d")) {
            flags += flag_anti_debug;
        } else if(strcmp(argv[i], "-t")) {
            flags += flag_time_trap;
        }
    }
    return flags;
}

int main(int argc, char** argv) {
    get_args(argc, argv);
    
    if(argc < 1) {
        printf("Usage: ez executable.exe [-i -d -t] \n"
               "-i : destroy import address table\n"
               "-d : add anti debug traps\n"
               "-t : add execution time trace traps\n\n");
    }
    
    _PE target = load_file(argv[1]);
    if(target->load_error != NULL) {
        printf(target->load_error);
    }
    
    apply_crypt(target);
    
    save_file(target, argv[1]);
    release_file(target);
    return (EXIT_SUCCESS);
}

