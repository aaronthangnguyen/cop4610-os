#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

sem_t access_mutex;
sem_t readers_mutex;
sem_t order_mutex;
int shared_counter = 0;
unsigned int readers = 0;

void *reader(void *args) {
  long id = (long)args;
  sem_wait(&order_mutex);
  sem_wait(&readers_mutex);
  if (readers == 0)
    sem_wait(&access_mutex);
  readers++;
  sem_post(&order_mutex);
  sem_post(&readers_mutex);

  // Simulate reading the shared_counter 2,000,000 times
  int local_copy = shared_counter;

  sem_wait(&readers_mutex);
  readers--;
  if (readers == 0)
    sem_post(&access_mutex);
  sem_post(&readers_mutex);

  printf("Reader %ld read the value: %d\n", id, local_copy);
  return NULL;
}

void *writer(void *args) {
  sem_wait(&order_mutex);
  sem_wait(&access_mutex);
  sem_post(&order_mutex);

  for (int i = 0; i < 25000; ++i) {
    shared_counter++;
  }
  printf("Writer done, final value: %d\n", shared_counter);

  sem_post(&access_mutex);
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <number of readers>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int num_readers = atoi(argv[1]);
  if (num_readers < 1 || num_readers > 12) {
    fprintf(stderr, "Number of readers must be between 1 and 12.\n");
    return EXIT_FAILURE;
  }

  sem_init(&access_mutex, 0, 1);
  sem_init(&readers_mutex, 0, 1);
  sem_init(&order_mutex, 0, 1);

  pthread_t writer_thread;
  pthread_t reader_threads[num_readers];

  pthread_create(&writer_thread, NULL, writer, NULL);
  for (long i = 0; i < num_readers; i++) {
    pthread_create(&reader_threads[i], NULL, reader, (void *)i);
  }

  pthread_join(writer_thread, NULL);
  for (int i = 0; i < num_readers; i++) {
    pthread_join(reader_threads[i], NULL);
  }

  sem_destroy(&access_mutex);
  sem_destroy(&readers_mutex);
  sem_destroy(&order_mutex);

  return EXIT_SUCCESS;
}
