#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main() {
    int fdIn[2]; // going out from child to parent
    char buffer[] = "HELLO, PARENT PROCESS";
    char input[]  = "                     ";

    if (pipe(fdIn) == -1) {
        perror("pipe fdIn");
        return -1;
    }

    pid_t childPid = fork();
    if (childPid == 0) {
        printf("[+] child process\n");

        // close the read end of the pipe..
        // only child will be writing.
        close(fdIn[0]);

        // dup the fd with stdin.
        dup2(fdIn[1], 1);

        // throw something into the wire...
        printf("%s", buffer);

        // close the dupped fd;
        close(fdIn[1]);
    } else {
        printf("[+] parent process\n");

        // close the write end of the pipe.
        // only child will only be writing.
        close(fdIn[1]);

        // write something.
        read(fdIn[0], input, sizeof(input));
        printf("    <- %s\n", input);

        // close the fd when done.
        close(fdIn[0]);

        wait(NULL);
    }

    return 0;
}
