#ifndef SYNC_PRIMITIVES_H
#define SYNC_PRIMITIVES_H

#include <stdint.h>
#include <stdatomic.h>

// Структура мьютекса
typedef struct {
    _Atomic uint32_t state;
} mutex_t;

// API мьютекса
void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);
void mutex_destroy(mutex_t *m);

// Структура RW-блокировки
//  бит 0 - флаг писателя (1 = писатель активен)
// биты 31-1 - счетчик читателей
typedef struct {
    _Atomic uint32_t state;
} rwlock_t;

// Константы для RW-блокировки
#define WRITER_BIT 0x1u
#define READER_MASK 0xFFFFFFFEu

// API RW-блокировки
void rwlock_init(rwlock_t *rw);
void rwlock_rdlock(rwlock_t *rw);
void rwlock_wrlock(rwlock_t *rw);
void rwlock_unlock(rwlock_t *rw);
void rwlock_destroy(rwlock_t *rw);

#endif 