#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <editline/readline.h>
#include <editline/history.h>

#define BUFFER_SIZE 8192

static char command[BUFFER_SIZE];

int main(int argc, char** argv) {

  puts(">> Defunct v0.2");

  while (1) {
    char* input_line = readline(": ");
    printf("%s\n", input_line);
    free(input_line);
  }

  return 0;
}
