#include <stdlib.h>
#include <stdio.h>

void monitor_syscalls(int pid1, int pid2) {
    char command[100];
    sprintf(command, "sudo stap trace.stp %d %d > syscalls.log", pid1, pid2);
    system(command);
}
