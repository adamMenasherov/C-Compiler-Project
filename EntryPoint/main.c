#include "EntryPoint/FileHandling.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  argc--;
  argv++;

  if (argc < 1) {
    fprintf(stderr, "No files provided for compilation.\n");
    return 1;
  }
  startProcess(argc, argv);
  return 0;
}
