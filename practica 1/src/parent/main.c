#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "monitor.h"
#include "stat.h"
#include "signal_handler.h"

int main() {
    signal(SIGINT, ctrlc_handler);

    int fd = open("syscalls.log", O_RDWR | O_CREAT | O_TRUNC, 0777);
    int fdch = open("practica1.txt", O_CREAT | O_TRUNC, 0777);
    close(fdch);

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(1);
    }
    if (pid1 == 0) {
        char *arg_Ptr[4] = {"child.c", "Hola!", "Soy el proceso hijo!", NULL};
        execv("/home/alexis/Escritorio/sopes_2/practica _1/src/child/main.bin", arg_Ptr);
    } else {
        pid_t pid2 = fork();
        if (pid2 == -1) {
            perror("fork");
            exit(1);
        }
        if (pid2 == 0) {
            char *arg_Ptr[4] = {"child.c", "Hola!", "Soy el proceso hijo!", NULL};
            execv("/home/alexis/Escritorio/sopes_2/practica _1/src/child/main.bin", arg_Ptr);
        } else {
            pid_t monitor = fork();
            if (monitor == 0) {
                monitor_syscalls(pid1, pid2);
            }

            int status;
            waitpid(pid2, &status, 0);
            waitpid(pid2, &status, 0);
            waitpid(monitor, &status, 0);
            stat(pid1, pid2, monitor, fd);
        }
    }

    return 0;
}
