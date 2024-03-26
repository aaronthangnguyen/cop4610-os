#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_READERS 12
#define NUM_READS 2000000
#define NUM_WRITES 25000

// A semaphore for writer priority
sem_t writeAccess;
// Mutex and condition variable for reader control
pthread_mutex_t readWriteMutex;
pthread_cond_t canRead;
int activeReaders = 0;
int waitingWriters = 0;
int shared_count = 0;

void *writer(void *arg) {
  for (int i = 0; i < NUM_WRITES; i++) {
    // Wait for exclusive access to write
    sem_wait(&writeAccess);
    pthread_mutex_lock(&readWriteMutex);
    while (activeReaders > 0) {
      pthread_cond_wait(&canRead, &readWriteMutex);
    }
    // Write operation
    shared_count += 1;
    pthread_mutex_unlock(&readWriteMutex);
    sem_post(&writeAccess);
  }
  printf("Writer done\n");
  return NULL;
}

void *reader(void *arg) {
  int id = *((int *)arg);

  for (int i = 0; i < NUM_READS; i++) {
    pthread_mutex_lock(&readWriteMutex);
    while (waitingWriters > 0) {
      pthread_cond_wait(&canRead, &readWriteMutex);
    }
    activeReaders++;
    pthread_mutex_unlock(&readWriteMutex);

    // Simulated read operation
    // No actual reading of shared_count to avoid slowing down

    pthread_mutex_lock(&readWriteMutex);
    activeReaders--;
    if (activeReaders == 0 && waitingWriters > 0) {
      pthread_cond_signal(&canRead);
    }
    pthread_mutex_unlock(&readWriteMutex);
  }

  pthread_mutex_lock(&readWriteMutex);
  printf("I'm reader%d, counter = %d\n", id + 1, shared_count);
  pthread_mutex_unlock(&readWriteMutex);

  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_mutex_init(&readWriteMutex, NULL);
  pthread_cond_init(&canRead, NULL);
  sem_init(&writeAccess, 0, 1); // Initialize semaphore for writers

  if (argc < 2) {
    printf("Usage: %s <number of readers>\n", argv[0]);
    return 1;
  }

  int num_readers = atoi(argv[1]);
  if (num_readers <= 0 || num_readers > MAX_READERS) {
    printf("Number of readers must be between 1 and %d.\n", MAX_READERS);
    return 1;
  }

  pthread_t writers[1], readers[MAX_READERS];
  int ids[MAX_READERS];

  pthread_create(&writers[0], NULL, writer, NULL);

  for (int i = 0; i < num_readers; i++) {
    ids[i] = i;
    pthread_create(&readers[i], NULL, reader, &ids[i]);
  }

  pthread_join(writers[0], NULL);

  for (int i = 0; i < num_readers; i++) {
    pthread_join(readers[i], NULL);
  }

  pthread_mutex_destroy(&readWriteMutex);
  pthread_cond_destroy(&canRead);
  sem_destroy(&writeAccess);

  return 0;
}
