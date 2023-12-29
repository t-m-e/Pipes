#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int fdIn[2];    // input from child -> parent
    int fdOut[2];   // output from parent -> child
    
    char buffer[100] = { 0 };
    char input[100]  = { 0 };

    if (pipe(fdIn) == -1) {
        perror("pipe fdIn");
        return -1;
    }

    if (pipe(fdOut) == -1) {
        perror("pipe fdOut");
        return -1;
    }

    pid_t childpid = fork();
    if (childpid == 0) {
        printf("[+] child process\n");

        char* childPrompts[] = {
            "CHILD -> PARENT: Hello, PARENT\n",
            "CHILD -> PARENT: Bye!\n"
        };

        // close the write end of the OUT pipe.
        close(fdOut[1]);
        // close the read end of the IN pipe.
        close(fdIn[0]);

        // dup the OUT pipe with STDIN.
        dup2(fdOut[0], 0);
        // dup the IN pipe with STDOUT.
        dup2(fdIn[1], 1);

        fgets(input, sizeof(input), stdin);
        fprintf(stderr, "    C <- %s", input);
        memset(input, 0, sizeof(input));

        fprintf(stderr, "    C -> %s", childPrompts[0]);
        printf("%s", childPrompts[0]);
       
        fgets(input, sizeof(input), stdin);
        fprintf(stderr, "    C <- %s", input);
        memset(input, 0, sizeof(input));

        fprintf(stderr, "    C -> %s", childPrompts[1]);
        printf("%s", childPrompts[1]);

        // close when done.
        close(fdOut[0]);
        close(fdIn[1]);
    } else {
        printf("[+] parent process\n");

        char* parentPrompts[] = {
            "PARENT -> CHILD: Hello, CHILD\n",
            "PARENT -> CHILD: Goodbye, CHILD\n"
        };

        // close the read end of the OUT pipe.
        close(fdOut[0]);
        // close the write end of the IN pipe.
        close(fdIn[1]);

        write(
            fdOut[1], 
            parentPrompts[0],
            strlen(parentPrompts[0])
        );
        printf("    P -> %s", parentPrompts[0]);

        read(fdIn[0], input, sizeof(input));
        printf("    P <- %s", input);
        memset(input, 0, sizeof(input));

        write(
            fdOut[1], 
            parentPrompts[1],
            strlen(parentPrompts[1])
        );
        printf("    P -> %s", parentPrompts[1]);

        read(fdIn[0], input, sizeof(input));
        printf("    P <- %s", input);
        memset(input, 0, sizeof(input));

        // close when done. 
        close(fdOut[1]);
        close(fdIn[0]);

        // wait for the child process.
        wait(NULL);
    }

    return 0;
}
