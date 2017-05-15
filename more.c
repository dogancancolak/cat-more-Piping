#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include <unistd.h>
#define MAX_LINE 24
#define BUFFER_SIZE 500

int main(int argc, char const *argv[]) {


  int readend = atoi(argv[1]);

  char msg[MAX_LINE][BUFFER_SIZE];

  read(readend,&msg,MAX_LINE*BUFFER_SIZE);

  int i;
  for (i = 0; i < MAX_LINE; i++) {
    if (strlen(msg[i])<1) {
      break;
    }
    printf("%s",msg[i]);
  }

  close(readend);
  return 0;
}
