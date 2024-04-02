#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_UPDATES 4444

/* Struct a shared variable to store result */
struct shared_data {
    int value;  
};

/* Global shared variable */
struct shared_data *counter;
/* Mutex lock */
pthread_mutex_t mutex;

/* Thread1 function */
void *thread1() {
    int i = 0
    while (i < MAX_UPDATES) {
        if (pthread_mutex_trylock(&mutex) == 0) {
            /* Do your Job!*/
           
            pthread_mutex_unlock(&mutex);
        }
    }
    
    printf("I'm thread1, I did ....\n");
    return NULL;
}


int main() {
    pthread_t tid[2];
    int rc;

    /* Allocate memory for shared data */
    counter = (struct shared_data *) malloc(sizeof(struct shared_data));
    counter->value = 0;

    /* Initialize mutex lock */
    if ((pthread_mutex_init(&mutex, NULL))) {
        printf("Error occured when initialize mutex lock.");
        exit(0);
    }

    pthread_attr_t attr;
    if ((pthread_attr_init(&attr))) {
        printf("Error occured when initialize pthread_attr_t.");
        exit(0);
    }

    /* Create thread1 */
    if ((rc = pthread_create(&tid[0], &attr, thread1, NULL))) {
        fprintf(stderr, "ERROR: pthread_create, rc: %d\n", rc);
        exit(0);
    }

    /* Wait for threads to finish */
    pthread_join(tid[0], NULL);

 
    /* Clean up */
    pthread_mutex_destroy(&mutex);
    free(counter);
    pthread_exit(NULL);

    return 0;
}
