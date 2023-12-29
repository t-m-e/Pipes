/* +========+                        +=======+
 * | PARENT |>> STDOUT >>>>> STDIN >>| CHILD |
 * +========+    WRITE        READ   +=======+
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main() {
    int fdOut[2]; // going out from parent into child
    char buffer[] = "HELLO, CHILD PROCESS";
    char input[]  = "                    ";

    if (pipe(fdOut) == -1) {
        perror("pipe fdOut");
        return -1;
    }

    pid_t childPid = fork();
    if (childPid == 0) {
        printf("[+] child process\n");

        // close the write end of the pipe.
        // only the parent will be writing.
        close(fdOut[1]);

        // dup the fd with stdin.
        dup2(fdOut[0], 0);

        // grab something from the wire...
        fgets(input, sizeof(input), stdin);
        printf("    <- %s\n", input);

        // close the dupped fd;
        close(fdOut[0]);
    } else {
        printf("[+] parent process\n");

        // close the read end of the pipe.
        // this process will only be writing.
        close(fdOut[0]);

        // write something.
        write(fdOut[1], buffer, sizeof(buffer));
        printf("    -> %s\n", buffer);

        // close the fd when done.
        close(fdOut[1]);

        wait(NULL);
    }

    return 0;
}
