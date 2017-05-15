#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termio.h>
#include <stdlib.h>
#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 500 // one rows length
#define MAX_LINE 24

int getch(void) // this function just waiting for input and then takes the char we pressed. No "enter" needed
{
  struct termios oldattr, newattr;
  int ch;
  tcgetattr(STDIN_FILENO, &oldattr);
  newattr=oldattr;
  newattr.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
  ch=getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

  return ch;
}

int main(int argc, char const *argv[]) {

  //                File reader variables
  FILE *fp;
  char const *filename = argv[1];
  char *line= NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen(filename, "r");
  if (fp == NULL){
    exit(EXIT_FAILURE);
  }
  //   ------------------------

  if (argc > 2) { //     More than 2 arguments  (Main if)
    if (!strcmp(argv[2],"=")) { //         if there is pipe symbol

      char msg[MAX_LINE][BUFFER_SIZE]; // message in pipe
      char input;
      char strRead[15]; // pipe's read end in string form
      int endofdoc = 0; // Document finished ? 1 = document finished


      do {
        int fd[2];//        Pipe
        pid_t pid;
        // Pipe Creation
        if (pipe(fd) < 0) {
          fprintf(stderr,"Pipe Failed");
          return 1;
        }// ----------------------------

        pid = fork();           // Create Child

        if (pid < 0)       /* fork failed           */
        {
                printf("ERROR: forking child process failed\n");
                exit(1);
        }

        if (pid > 0)                /* for the parent process      */
        {

          int counter;
          for (counter = 0; counter < MAX_LINE ; counter++) {
            if (read = getline(&line, &len, fp) != -1) {
              strcpy(msg[counter],line); //                         filling the message
            }
            else{
              endofdoc = 1;
              break;
            }

          }

          close(fd[READ_END]);
          write(fd[WRITE_END],&msg,counter*BUFFER_SIZE);
          close(fd[WRITE_END]);

          wait(NULL); // waiting the child process

          if (endofdoc == 1) {
            printf("\n -----END OF DOCUMENT-----\n");
          }
          else{
            printf("\n-----PRESS KEY TO CONTINUE-----");
          }
          input = getch();
        }

        else if (pid == 0)           /* for the child process:         */
        {
          close(fd[WRITE_END]);
          sprintf(strRead,"%d", fd[READ_END]); // pipe's read end to string
          char *secondArgs[3] = {argv[3],strRead,NULL}; // Argv[] for child process
          if (execv(argv[3], secondArgs)  < 0) // execution of myMore
          {
            printf("ERROR: exec failed\n");
            exit(1);
          }
        }
      }while (input != 'q' && endofdoc == 0);

      free(line);
      exit(EXIT_SUCCESS);
    }

  }
  else { // simple myCat

      while ((read = getline(&line, &len, fp)) != -1) {
        printf("%s", line);
    }

      free(line);
      exit(EXIT_SUCCESS);

  }
  return 0;
}
