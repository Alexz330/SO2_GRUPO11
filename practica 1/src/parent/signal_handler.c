#include <signal.h>
#include <signal.h>

volatile sig_atomic_t  sigint_received = 0;

void ctrlc_handler(int signal) {
    sigint_received = 1;
}