#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* ASSIGNMENT 4
 * Name: Aaron Nguyen
 * PID: 6403474
 */

/*
 * Reader-Writer Synchronization using Semaphores
 *
 * This program demonstrates a solution to the readers-writer problem using
 * POSIX threads (pthreads) and semaphores. It ensures fairness and prevents
 * starvation by managing access to a shared resource among multiple reader and
 * writer threads. Semaphores are used to synchronize access in a way that
 * multiple readers can access the shared resource simultaneously, but writers
 * gain exclusive access. An order semaphore ensures that access is granted
 * based on the order of arrival, to prevent starvation and ensure fairness
 * between readers and writers.
 *
 * Features:
 * - Initialize semaphores for access control and synchronization.
 * - Create writer and reader threads that simulate reading from and writing to
 * a shared resource.
 * - Use an order semaphore to maintain the order of thread arrival.
 * - Use a readers count protected by a mutex to allow multiple readers
 * concurrently while ensuring exclusive access for writers.
 * - Ensure the program is deadlock-free and starvation is minimized for both
 * readers and writers.
 *
 * Reference:
 * https://en.wikipedia.org/wiki/Readers%E2%80%93writers_problem#Third_readers%E2%80%93writers_problem
 */

// Semaphores to control read/write access
sem_t access_mutex;  // Controls access to the shared resource.
sem_t readers_mutex; // Protects the 'readers' count.
sem_t order_mutex;   // Ensures that access by order of arrival.

int shared_counter = 0;   // Shared resource
unsigned int readers = 0; // Count of active readers.

// Function to initialize semaphores
void initialize_semaphores() {
  sem_init(&access_mutex, 0,
           1); // Initialized to 1, allowing write access if no readers.
  sem_init(&readers_mutex, 0, 1);
  sem_init(&order_mutex, 0, 1);
}

// Reader thread function
void *reader(void *arg) {
  long id = (long)arg;

  // Ensure order of access to prevent starvation and ensure fairness.
  sem_wait(&order_mutex);
  // Protect 'readers' count update
  sem_wait(&readers_mutex);
  if (readers == 0)
    sem_wait(&access_mutex); // If first reader, lock resource from writers.
  readers++;
  // Release semaphores after successfully updating 'readers' count
  sem_post(&order_mutex);
  sem_post(&readers_mutex);

  // Simulated reading operation
  int local_copy = shared_counter; // Copy shared counter value (simulated read)

  // On finishing reading, check if this reader is the last to ensure writers
  // can proceed.
  sem_wait(&readers_mutex);
  readers--;
  if (readers == 0)
    sem_post(&access_mutex); // If last reader, allow writer access.
  sem_post(&readers_mutex);

  printf("Reader %ld reads shared counter value: %d\n", id + 1, local_copy);
  return NULL;
}

// Writer thread function
void *writer(void *arg) {
  // Writers also wait for their turn to ensure fairness
  sem_wait(&order_mutex);
  sem_wait(&access_mutex); // Request exclusive access
  sem_post(&order_mutex);

  // Simulate writing by incrementing the shared_counter
  for (int i = 0; i < 25000; ++i) {
    shared_counter++;
  }
  printf("Writer done, final value: %d\n", shared_counter);

  sem_post(&access_mutex); // Release exclusive access after writing
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
