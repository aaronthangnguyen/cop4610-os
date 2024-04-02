/* ASSIGNMENT 3
 * Name: Aaron Nguyen
 * PID: 6403474
 */

/*
 * Shared Counter with Mutex
 * - Initialize and create threads
 * - Safely modify shared data using mutex
 * - Synchonize thread execution
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_UPDATES 2000000

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
  int bonus_ok = 0;
  int bonus_times = 0;
  int updates = 0;

  // Remainder section
  while (updates < MAX_UPDATES) {
    // Entry section
    if (pthread_mutex_lock(&mutex) == 0) {
      // Critical section
      bonus_ok = MAX_UPDATES - updates >= 100 && counter->value % 100 == 0;
      if (bonus_ok) {
        counter->value += 100;
      } else {
        counter->value += 1;
      }
      // Exit section
      pthread_mutex_unlock(&mutex);
    }

    // Remainder section
    if (bonus_ok) {
      bonus_times += 1;
      updates += 100;
    } else
      updates += 1;
  }

  // Remainder section
  printf("I'm thread1, I did %d updates and I got the bonus for %d times, "
         "counter = %d\n",
         updates, bonus_times, counter->value);

  return NULL;
}

/* Thread2 function */
void *thread2() {
  int updates = 0;

  while (updates < MAX_UPDATES) {
    if (pthread_mutex_lock(&mutex) == 0) {
      counter->value += 1;
      pthread_mutex_unlock(&mutex);
    }
    updates += 1;
  }

  printf("I'm thread2, I did %d updates, counter = %d\n", updates,
         counter->value);

  return NULL;
}

int main() {
  pthread_t tid[2];
  int rc;

  /* Allocate memory for shared data */
  counter = (struct shared_data *)malloc(sizeof(struct shared_data));
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

  /* Create thread2 */
  if ((rc = pthread_create(&tid[1], &attr, thread2, NULL))) {
    fprintf(stderr, "ERROR: pthread_create, rc: %d\n", rc);
    exit(0);
  }

  /* Wait for threads to finish */
  pthread_join(tid[0], NULL);
  pthread_join(tid[1], NULL);

  if (pthread_mutex_trylock(&mutex) == 0) {
    printf("from parent counter = %d\n", counter->value);
    pthread_mutex_unlock(&mutex);
  }

  /* Clean up */
  pthread_mutex_destroy(&mutex);
  free(counter);
  pthread_exit(NULL);

  return 0;
}
