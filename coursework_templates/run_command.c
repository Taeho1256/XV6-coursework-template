#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

void execute_line(char** args) {

  // Create a child process
  int pid = fork();

  if (pid < 0) {
    fprintf(2, "Error: fork failed\n");
  }
  else if (pid == 0) {
    exec(args[0], args);
    fprintf(2, "Error: exec failed\n");
    exit(1);
  }
  else {
    wait(0);
  }
}

int main(int argc, char *argv[]) {

  // Two pointers: to a command and to a null terminator.
  char *args[2] = {0};

  // Fixed-size storage for reading in a line of input
  char buf[512], *b;

  // Point b to the first character
  b = buf;

  fprintf(2, "Please enter command and press enter (type 'q' to exit):\n");

  while (read(0, b, 1) > 0) {

    if (*b == '\n') {
      *b = '\0';

      if (strcmp(buf, "q") == 0) {
        exit(0);
      }

      if (buf[0] != '\0') {
        args[0] = buf;
        args[1] = 0;
        execute_line(args);
      }

      b = buf;

      fprintf(2, "Please enter command and press enter (type 'q' to exit):\n");
    }
    else {
      if (b < buf + sizeof(buf) - 1) {
        b++;
      }
    }
  }

  exit(0);
}