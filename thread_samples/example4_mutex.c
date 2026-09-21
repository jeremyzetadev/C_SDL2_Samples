#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int shared_counter = 0;

pthread_mutex_t mutex;

void *increment_shared_counter(void *arg){
    char* msg_arg = (char*)arg;
    for(int i=0; i<100000; i++){
        pthread_mutex_lock(&mutex);
        shared_counter++;
        printf("%s: %i\n", msg_arg, i);
        pthread_mutex_unlock(&mutex);
    }
}

int main(){
    pthread_t t1, t2;

    if(pthread_mutex_init(&mutex, NULL)!=0){
        printf("mutex init failed\n");
        return 1;
    }

    char* msg_t1 = malloc(12 * sizeof(char)); 
    msg_t1 = "thread1";
    char* msg_t2 = malloc(12 * sizeof(char)); 
    msg_t2 = "thread2";

    pthread_create(&t1, NULL, increment_shared_counter, msg_t1);
    pthread_create(&t2, NULL, increment_shared_counter, msg_t2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);
}
