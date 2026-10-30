#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

void execute_left_side(char** args, int *p) {

  // Create a child process for executing the
  // left-side command.
  int f = fork();
  if (f < 0) {
    fprintf(2, "run_command: fork failed\n");
    exit(1);
  } else if (f == 0) {
    // Redirect standard output to the pipe.
    close(1);
    dup(p[1]);

    // Close unused pipe descriptors.
    close(p[0]);
    close(p[1]);

    // Execute the left-side command.
    exec(args[0], args);
    fprintf(2, "run_command: exec failed\n");
    exit(1);
  }
  // Parent continues in main.
}

void execute_right_side(char** args, int *p) {

  // Create a child process for executing the
  // right-side command.
  int f = fork();
  if (f < 0) {
    fprintf(2, "run_command: fork failed\n");
    exit(1);
  } else if (f == 0) {
    // Redirect standard input to the pipe.
    close(0);
    dup(p[0]);

    // Close unused pipe descriptors.
    close(p[0]);
    close(p[1]);

    // Execute the right-side command.
    exec(args[0], args);
    fprintf(2, "run_command: exec failed\n");
    exit(1);
  } else {
    // Wait for both child processes.
    wait(0);
    wait(0);
  }
}

int main(int argc, char *argv[]) {

  // Array of pointers for storing commands and null terminators.
  char *args[6] = {0};
  args[2] = 0;
  args[5] = 0;

  // Pipe for communication between commands.
  int p[2];

  // Storage for reading input.
  char buf[512], *b;
  args[0] = buf;

  // Point b to the first character.
  b = buf;

  while (1) {
    b = buf;

    // Reset argument pointers.
    args[1] = 0;
    args[4] = 0;

    // Create a new pipe.
    if (pipe(p) < 0) {
      fprintf(2, "run_command: pipe failed\n");
      exit(1);
    }

    fprintf(2, "\nPlease enter the first command and press enter (type 'q' to exit):\n");

    while (read(0, b, 1) > 0) {
      if (*b == '\n') {
        *b = '\0';

        if (buf[0] == 'q' && buf[1] == '\0') {
          close(p[0]);
          close(p[1]);
          exit(0);
        }
        else {
          // Execute left-side command.
          execute_left_side(args, p);

          // Parent closes write-end of pipe.
          close(p[1]);
          break;
        }
      } else if (*b == ' ') {
        *b = '\0';
        b++;
        args[1] = b;
      } else {
        b++;
      }
    }

    fprintf(2, "\nPlease enter the second command and press enter:\n");

    b++;
    args[3] = b;

    while (read(0, b, 1) > 0) {
      if (*b == '\n') {
        *b = '\0';

        // Execute right-side command.
        execute_right_side(&args[3], p);

        // Close remaining read-end.
        close(p[0]);
        break;
      } else if (*b == ' ') {
        *b = '\0';
        b++;
        args[4] = b;
      } else {
        b++;
      }
    }
  }

  exit(0);
}