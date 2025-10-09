#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "sync_primitives.h"

#define NUM_THREADS 5
#define NUM_ITERATIONS 3

//  переменные для демонстрации
mutex_t counter_mutex;
rwlock_t data_rwlock;
int shared_counter = 0;
int shared_data = 0;

void* worker_mutex(void* arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        mutex_lock(&counter_mutex);
        
        // Критическая секция
        int temp = shared_counter;
        printf("Thread %d: read counter = %d\n", thread_id, temp);
        usleep(100000); // Имитация работы
        shared_counter = temp + 1;
        printf("Thread %d: updated counter to %d\n", thread_id, shared_counter);
        
        mutex_unlock(&counter_mutex);
        
        usleep(50000); // Работа вне критической секции
    }
    
    return NULL;
}

void* reader_thread(void* arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        rwlock_rdlock(&data_rwlock);
        
        // Чтение данных
        printf("Reader %d: data = %d\n", thread_id, shared_data);
        usleep(200000); // Имитация чтения
        
        rwlock_unlock(&data_rwlock);
        
        usleep(100000);
    }
    
    return NULL;
}

void* writer_thread(void* arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        rwlock_wrlock(&data_rwlock);
        
        // Запись данных
        printf("Writer %d: updating data from %d", thread_id, shared_data);
        shared_data += 10;
        printf(" to %d\n", shared_data);
        usleep(300000); // Имитация записи
        
        rwlock_unlock(&data_rwlock);
        
        usleep(200000);
    }
    
    return NULL;
}

void demo_mutex() {
    printf("=== DEMO: Mutex ===\n");
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    mutex_init(&counter_mutex);
    shared_counter = 0;
    
    // Создаем потоки
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, worker_mutex, &thread_ids[i]);
    }
    
    // Ждем завершения всех потоков
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Final counter value: %d (expected: %d)\n", 
           shared_counter, NUM_THREADS * NUM_ITERATIONS);
    
    mutex_destroy(&counter_mutex);
}

void demo_rwlock() {
    printf("\n=== DEMO: RW-Lock ===\n");
    pthread_t readers[3], writers[2];
    int reader_ids[3] = {1, 2, 3};
    int writer_ids[2] = {1, 2};
    
    rwlock_init(&data_rwlock);
    shared_data = 0;
    
    // Создаем потоки читателей
    for (int i = 0; i < 3; i++) {
        pthread_create(&readers[i], NULL, reader_thread, &reader_ids[i]);
    }
    
    // Создаем потоки писателей
    for (int i = 0; i < 2; i++) {
        pthread_create(&writers[i], NULL, writer_thread, &writer_ids[i]);
    }
    
    // Ждем завершения читателей
    for (int i = 0; i < 3; i++) {
        pthread_join(readers[i], NULL);
    }
    
    // Ждем завершения писателей
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }
    
    printf("Final data value: %d\n", shared_data);
    
    rwlock_destroy(&data_rwlock);
}

int main() {
    printf("Starting synchronization primitives demo...\n\n");
    
    demo_mutex();
    demo_rwlock();
    
    printf("\nDemo completed successfully!\n");
    return 0;
}