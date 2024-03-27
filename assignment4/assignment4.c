#include <errno.h>
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

// Error checking semaphore operations
#define HANDLE_SEM_ERROR(en, msg)                                              \
  do {                                                                         \
    errno = en;                                                                \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

void initialize_semaphores() {
  if (sem_init(&access_mutex, 0, 1) != 0)
    HANDLE_SEM_ERROR(errno, "sem_init access_mutex");
  if (sem_init(&readers_mutex, 0, 1) != 0)
    HANDLE_SEM_ERROR(errno, "sem_init readers_mutex");
  if (sem_init(&order_mutex, 0, 1) != 0)
    HANDLE_SEM_ERROR(errno, "sem_init order_mutex");
}

void *reader(void *arg) {
  long id = (long)arg;

  if (sem_wait(&order_mutex) != 0)
    HANDLE_SEM_ERROR(errno, "sem_wait order_mutex");
  if (sem_wait(&readers_mutex) != 0)
    HANDLE_SEM_ERROR(errno, "sem_wait readers_mutex");
  if (readers == 0)
    if (sem_wait(&access_mutex) != 0)
      HANDLE_SEM_ERROR(errno, "sem_wait access_mutex");
  readers++;
  if (sem_post(&order_mutex) != 0)
    HANDLE_SEM_ERROR(errno, "sem_post order_mutex");
  if (sem_post(&readers_mutex) != 0)
    HANDLE_SEM_ERROR(errno, "sem_post readers_mutex");

  // Simulating reading
  int local_copy;
  for (int i = 0; i < 2000000; ++i) {
    local_copy = shared_counter;
  }
  printf("Reader %ld reads shared counter value: %d\n", id, local_copy);

  if (sem_wait(&readers_mutex) != 0)
    HANDLE_SEM_ERROR(errno, "sem_wait readers_mutex");
  readers--;
  if (readers == 0)
    if (sem_post(&access_mutex) != 0)
      HANDLE_SEM_ERROR(errno, "sem_post access_mutex");
  if (sem_post(&readers_mutex) != 0)
    HANDLE_SEM_ERROR(errno, "sem_post readers_mutex");

  return NULL;
}

void *writer(void *arg) {
  if (sem_wait(&order_mutex) != 0)
    HANDLE_SEM_ERROR(errno, "sem_wait order_mutex");
  if (sem_wait(&access_mutex) != 0)
    HANDLE_SEM_ERROR(errno, "sem_wait access_mutex");
  if (sem_post(&order_mutex) != 0)
    HANDLE_SEM_ERROR(errno, "sem_post order_mutex");

  // Simulating writing
  for (int i = 0; i < 25000; ++i) {
    shared_counter++;
  }
  printf("Writer done, final value: %d\n", shared_counter);

  if (sem_post(&access_mutex) != 0)
    HANDLE_SEM_ERROR(errno, "sem_post access_mutex");

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <number of readers>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int num_readers = atoi(argv[1]);
  if (num_readers <= 0 || num_readers > 12) {
    fprintf(stderr, "Number of readers must be between 1 and 12.\n");
    exit(EXIT_FAILURE);
  }

  initialize_semaphores();

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

  return 0;
}
