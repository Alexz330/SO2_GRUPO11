#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>

int calls = 0;
int c_write = 0;
int c_read = 0;
int c_seek = 0;

void stat(int pid1, int pid2, int monitor, int fd) {
    printf("\nFinalizando ejecución...\n");
    char buff[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buff, sizeof(buff))) > 0) {
        char *ptr = buff;
        char *action;
        while ((action = strtok(ptr, "\n")) != NULL) {
            ptr = NULL;
            if (strstr(action, "read") != NULL) {
                c_read++;
            } else if (strstr(action, "lseek") != NULL) {
                c_seek++;
            } else if (strstr(action, "write") != NULL) {
                c_write++;
            }
        }
    }
    calls = c_read + c_seek + c_write;

    printf("-------------------------------------------------\n");
    printf("Llamadas al sistema de los procesos hijo: %d\n", calls);
    printf("Llamadas write: %d\n", c_write);
    printf("Llamadas read: %d\n", c_read);
    printf("Llamadas seek: %d\n", c_seek);
    printf("-------------------------------------------------\n");

    close(fd);
    kill(pid1, SIGKILL);
    kill(pid2, SIGKILL);
    kill(monitor, SIGKILL);
    exit(0);
}
