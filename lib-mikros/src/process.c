#include "process.h"
#include "stdio.h"

extern int main(int argc, char* args[]);

static uint32_t getargs() {
    struct regstate state = {
      .eax = 4,
      .ebx = 0,
      .ecx = 0,
      .edx = 0,
      .esi = 0,
      .edi = 0
    };

    syscall(&state);

    return state.eax;
}

void _start() {
  char** args = (char**) getargs();

  int argc = 0;

  if(args != 0) {
      while(args[argc] != 0) {
          argc++;
      }
  }

  int result = main(argc, args);

  exit(result);
}

void yield() {
    struct regstate state = {
      .eax = 5,
      .ebx = 0,
      .ecx = 0,
      .edx = 0,
      .esi = 0,
      .edi = 0
    };

    syscall(&state);

    return;
}

void exit(int returncode) {
    struct regstate state = {
        .eax = 1,
        .ebx = returncode,
        .ecx = 0,
        .edx = 0,
        .esi = 0,
        .edi = 0
    };

    syscall(&state);

    while(1) {
      printf("error\n");
    }
}

int exec(char* path, char** args) {
    struct regstate state = {
        .eax = 3,
        .ebx = (uint32_t) path,
        .ecx = (uint32_t) args,
        .edx = 0,
        .esi = 0,
        .edi = 0
    };

    syscall(&state);

    return state.eax;
}

void texec(char* path, char** args) {
    uint32_t res = exec(path, args);

    if(res) {
        printexecerror(path, res);
    }
}

void printexecerror(char* path, uint32_t code) {
    switch(code) {
    case EXEC_CORRUPT_ELF:
        printf("%s: Corrupt elf", path);
        break;
    case EXEC_FILESYSTEM:
        printf("%s: Filesystem error", path);
        break;
    case EXEC_PERM_DENIED:
        printf("%s: Permission denied", path);
        break;
    case EXEC_FILE_NOT_FOUND:
        printf("%s: File not found", path);
        break;
    }
    printf("\n");
}
