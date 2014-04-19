#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define offsetof(type, member)  __builtin_offsetof (type, member)

int main(int argc, char** argv) {
    printf("relative offset of e_lfanew: %d\n", offsetof(IMAGE_DOS_HEADER, e_lfanew));
    printf("relative offset number of sections in file header: %d\n", offsetof(IMAGE_FILE_HEADER, NumberOfSections));
    printf("size of all NT headers: %d\n", sizeof(IMAGE_NT_HEADERS));
    printf("size of section header: %d\n", sizeof(IMAGE_SECTION_HEADER));
    printf("relative offset optional header in nt headers: %d\n", offsetof(IMAGE_NT_HEADERS, OptionalHeader));
    printf("relative offset EP in optional header: %d\n", offsetof(IMAGE_OPTIONAL_HEADER64, AddressOfEntryPoint));
    return (EXIT_SUCCESS);
}

