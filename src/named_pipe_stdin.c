#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
    char buffer[] = "HELLO, CHILD PROCESS\n";
    char input[100] = { 0 };
    int fdIn;
    char pipeInName[100] = { 0 };
    snprintf(pipeInName, 100, "/tmp/.pipeIn%d", getpid());
    
    if (mkfifo(pipeInName, 0666) == -1) {
        perror("[-] mkfifo");
        return -1;
    }

    pid_t childPid = fork();
    if (childPid == 0) {
        printf("[+] child process\n");
        fdIn = open(pipeInName, O_RDONLY);

        dup2(fdIn, 0);

        fgets(input, 100, stdin);
        printf("    C <- %s", input);

        close(fdIn);
        return 0;
    } else {
        printf("[+] parent process\n");

        fdIn = open(pipeInName, O_WRONLY);
        write(fdIn, buffer, sizeof(buffer));
        printf("    P -> %s", buffer);

        close(fdIn);

        wait(NULL);
    }
        
    // remove the pipe from the pipeName path
    char command[103] = { 0 };
    snprintf(command, 103, "rm %s", pipeInName);
    system(command);
    return 0;
}
