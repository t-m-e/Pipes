#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main() {
    
    char pipeInName[100] = { 0 };
    char pipeOutName[100] = { 0 };

    snprintf(pipeInName, 100, "/tmp/pipeIn%d", getpid());
    snprintf(pipeOutName, 100, "/tmp/pipeOut%d", getpid());

    if (mkfifo(pipeInName, 0666) == -1) {
        perror("[-] mkfifo pipeIn");
        return -1;
    }

    if (mkfifo(pipeOutName, 0666) == -1) {
        perror("[-] mkfifo pipeOut");
        return -1;
    }

    pid_t child = fork();
    if (child == 0) {
        printf("[+] child process\n");
        
        int fdIn = open(pipeInName, O_RDONLY);
        int fdOut = open(pipeOutName, O_WRONLY);

        dup2(fdIn, 0);
        dup2(fdOut, 1);

        char input[100] = { 0 };
        char* prompt[] = {
            "HELLO, PARENT\n",
            "GOODBYE, PARENT\n"
        };

        fgets(input, 100, stdin);
        fprintf(stderr, "    C <- %s", input);
        memset(input, 0, 100);

        printf("%s", prompt[0]);
        fprintf(stderr, "    C -> %s", prompt[0]);

        fgets(input, 100, stdin);
        fprintf(stderr, "    C <- %s", input);
        memset(input, 0, 100);

        printf("%s", prompt[1]);
        fprintf(stderr, "    C -> %s", prompt[1]);

        close(fdIn);
        close(fdOut);
        return 0;
    } else {
        printf("[+] parent process\n");

        int fdIn = open(pipeInName, O_WRONLY);
        int fdOut = open(pipeOutName, O_RDONLY);

        char input[100] = { 0 };
        char* prompt[] = {
            "HELLO, CHILD\n", 
            "GOODBYE, CHILD\n"
        };

        write(fdIn, prompt[0], strlen(prompt[0]));
        printf("    P -> %s", prompt[0]);

        read(fdOut, input, 100);
        printf("    P <- %s", input);
        memset(input, 0, 100);

        write(fdIn, prompt[1], strlen(prompt[1]));
        printf("    P -> %s", prompt[1]);

        read(fdOut, input, 100);
        printf("    P <- %s", input);
        memset(input, 0, 100);

        close(fdIn);
        close(fdOut);
        wait(NULL);
    }

    char command[103] = { 0 };
    char command2[103] = { 0 };

    snprintf(command, 103, "rm %s", pipeInName);
    snprintf(command2, 103, "rm %s", pipeOutName);

    system(command);
    system(command2);

    return 0;
}
