#include <stdio.h>
#include <unistd.h>

int main() {
    // Get the number of logical CPU cores currently online
    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Your system can run %ld hardware threads in parallel.\n", cores);
    return 0;
}
