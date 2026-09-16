#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Function to be executed by the child thread
void* thread_func(void* arg) {
    char* message = (char*)arg;
    printf("Child thread: Received message = %s\n", message);
    
    for (int i=0; i<10; i++) {
        printf("Child thread running... %d\n", i);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t thread;
    char* msg = "Hello from main";

    // Create the child thread
    if (pthread_create(&thread, NULL, thread_func, (void*)msg) != 0) {
        perror("Failed to create thread");
        return 1;
    }

    // Both main and child threads are running simultaneously here
    for (int i = 0; i < 10; i++) {
        printf("Main thread running... step %d\n", i);
        sleep(1); 
    }

    // Wait for child thread to finish before exiting the program
    pthread_join(thread, NULL);

    printf("Main thread: Waiting for child thread to finish...\n");

    // Wait for the child thread to terminate
    pthread_join(thread, NULL);

    printf("Main thread: Finished\n");
    return 0;
}
