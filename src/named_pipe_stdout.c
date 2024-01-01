#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
    char buffer[] = "HELLO, PARENT PROCESS\n";
    char input[100] = { 0 };

    char pipeOutName[100] = { 0 };
    snprintf(pipeOutName, 100, "/tmp/.pipeOut%d", getpid());
    if (mkfifo(pipeOutName, 0666) == -1) {
        perror("[-] mkfifo");
        return -1;
    }

    pid_t childPid = fork();
    if (childPid == 0) {
        printf("[+] child process\n");
        int fdOut = open(pipeOutName, O_WRONLY);

        dup2(fdOut, 1);

        fprintf(stderr, "    C -> %s", buffer);
        printf("%s", buffer);

        close(fdOut);
        return 0;
    } else {
        printf("[+] parent process\n");

        int fdOut = open(pipeOutName, O_RDONLY);
        read(fdOut, input, 100);
        printf("    P <- %s", input);

        close(fdOut);

        wait(NULL);
    }

    // remove the pipe from the pipeOutName path
    char command[103] = { 0 };
    snprintf(command, 103, "rm %s", pipeOutName);
    system(command);
    return 0;
}
