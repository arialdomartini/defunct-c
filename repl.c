#include <stdio.h>
#define BUFFER_SIZE 8192

static char input[BUFFER_SIZE];

int main(int argc, char** argv) {

  puts(">> Defunct v0.1");

  while (1) {
    fputs(": ", stdout);
    fgets(input, BUFFER_SIZE, stdin);
    printf("%s", input);
  }

  return 0;
}
