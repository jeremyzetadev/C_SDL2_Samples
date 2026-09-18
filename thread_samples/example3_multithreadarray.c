#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// Function to be executed by the child thread
void* thread_func(void* arg) {
    char* message = (char*)arg;
    printf("[CREATE] Child thread number: %s\n", message);
    
    for (int i=0; i<10; i++) {
        printf("Child thread %s: running... %d\n", message, i);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t thread[7];

    for(int i=0; i<7; i++){
        // Create the child thread
        char* msg = malloc(12 * sizeof(char)); 
        sprintf(msg, "%d", i);

        if (pthread_create(&thread[i], NULL, thread_func, (void*)msg) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Both main and child threads are running simultaneously here
    for (int i = 0; i < 10; i++) {
        printf("Main thread running... step %d\n", i);
        sleep(1); 
    }

    printf("Main thread: Waiting for child threads to finish...\n");

    for(int j=0; j<7; j++){
        // Wait for the child thread to terminate
        pthread_join(thread[j], NULL);
    }

    printf("Main thread: Finished\n");
    return 0;
}
