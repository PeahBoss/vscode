#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "sync_primitives.h"

static void test_mutex_init_destroy(void **state) {
    (void)state;
    
    mutex_t m;
    mutex_init(&m);
    mutex_destroy(&m);

    assert_true(1);
}

static void test_mutex_lock_unlock(void **state) {
    (void)state;
    
    mutex_t m;
    mutex_init(&m);

    mutex_lock(&m);
    mutex_unlock(&m);
    
    mutex_destroy(&m);
}

static mutex_t test_mutex;
static int shared_counter = 0;
static const int NUM_THREADS = 10;
static const int ITERATIONS = 100;

void* increment_thread(void *arg) {
    (void)arg;
    
    for (int i = 0; i < ITERATIONS; i++) {
        mutex_lock(&test_mutex);
        shared_counter++;
        mutex_unlock(&test_mutex);
    }
    
    return NULL;
}

static void test_mutex_thread_safety(void **state) {
    (void)state;
    
    mutex_init(&test_mutex);
    shared_counter = 0;
    
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment_thread, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    assert_int_equal(shared_counter, NUM_THREADS * ITERATIONS);
    
    mutex_destroy(&test_mutex);
}


static void test_rwlock_init_destroy(void **state) {
    (void)state;
    
    rwlock_t rw;
    rwlock_init(&rw);
    rwlock_destroy(&rw);
    
    assert_true(1);
}

static void test_rwlock_single_reader_writer(void **state) {
    (void)state;
    
    rwlock_t rw;
    rwlock_init(&rw);

    rwlock_rdlock(&rw);
    rwlock_unlock(&rw);

    rwlock_wrlock(&rw);
    rwlock_unlock(&rw);
    
    rwlock_destroy(&rw);
}

static rwlock_t test_rwlock;
static int reader_count = 0;
static int writer_active = 0;
static int data_value = 0;

void* reader_test_thread(void *arg) {
    int thread_id = *(int*)arg;
    
    rwlock_rdlock(&test_rwlock);

    assert_int_equal(writer_active, 0); 
    reader_count++;

    usleep(10000);
    int temp = data_value;
    (void)temp; 
    
    reader_count--;
    rwlock_unlock(&test_rwlock);
    
    printf("Reader %d completed\n", thread_id);
    return NULL;
}

void* writer_test_thread(void *arg) {
    int thread_id = *(int*)arg;
    
    rwlock_wrlock(&test_rwlock);

    assert_int_equal(reader_count, 0); 
    assert_int_equal(writer_active, 0); 
    writer_active = 1;

    usleep(20000);
    data_value += 10;
    
    writer_active = 0;
    rwlock_unlock(&test_rwlock);
    
    printf("Writer %d completed\n", thread_id);
    return NULL;
}

static void test_rwlock_multiple_readers(void **state) {
    (void)state;
    
    rwlock_init(&test_rwlock);
    reader_count = 0;
    data_value = 0;
    
    pthread_t readers[3];
    int reader_ids[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++) {
        pthread_create(&readers[i], NULL, reader_test_thread, &reader_ids[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        pthread_join(readers[i], NULL);
    }
    
    rwlock_destroy(&test_rwlock);
}

static void test_rwlock_writer_exclusivity(void **state) {
    (void)state;
    
    rwlock_init(&test_rwlock);
    reader_count = 0;
    writer_active = 0;
    data_value = 0;
    
    pthread_t writer;
    int writer_id = 1;

    pthread_create(&writer, NULL, writer_test_thread, &writer_id);
    pthread_join(writer, NULL);
    
    assert_int_equal(data_value, 10);
    
    rwlock_destroy(&test_rwlock);
}


int main(void) {
    const struct CMUnitTest tests[] = {

        cmocka_unit_test(test_mutex_init_destroy),
        cmocka_unit_test(test_mutex_lock_unlock),
        cmocka_unit_test(test_mutex_thread_safety),

        cmocka_unit_test(test_rwlock_init_destroy),
        cmocka_unit_test(test_rwlock_single_reader_writer),
        cmocka_unit_test(test_rwlock_multiple_readers),
        cmocka_unit_test(test_rwlock_writer_exclusivity),
    };
    
    return cmocka_run_group_tests(tests, NULL, NULL);
}