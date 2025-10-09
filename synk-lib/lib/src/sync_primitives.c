#include "sync_primitives.h"
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

// Вспомогательная функция для системного вызова futex
// Исправляем тип параметра - убираем _Atomic
static int futex(uint32_t *uaddr, int futex_op, uint32_t val) {
    return syscall(SYS_futex, uaddr, futex_op, val, NULL, NULL, 0);
}

//  МЬЮТЕКС 

void mutex_init(mutex_t *m) {
    atomic_store(&m->state, 0);
}

void mutex_lock(mutex_t *m) {
    uint32_t expected;
    
    while (1) {
        // Пытаемся атомарно установить состояние в 1 (занят)
        expected = 0;
        if (atomic_compare_exchange_strong(&m->state, &expected, 1)) {
            // Успешно захватили мьютекс
            return;
        }
        
        // Мьютекс занят, сообщаем ядру что мы ждем
        // Используем обычный указатель вместо atomic для futex
        uint32_t current_state = atomic_load(&m->state);
        futex((uint32_t*)&m->state, FUTEX_WAIT, current_state);
        // После пробуждения цикл продолжается и мы снова пытаемся захватить
    }
}

void mutex_unlock(mutex_t *m) {
    // Атомарно освобождаем мьютекс
    atomic_store(&m->state, 0);
    
    // Будим одного ожидающего
    futex((uint32_t*)&m->state, FUTEX_WAKE, 1);
}

void mutex_destroy(mutex_t *m) {
    (void)m; // Подавление предупреждения о неиспользуемой переменной
}

//  RW-БЛОКИРОВКА 

void rwlock_init(rwlock_t *rw) {
    atomic_store(&rw->state, 0);
}

void rwlock_rdlock(rwlock_t *rw) {
    uint32_t old_state, new_state;
    
    while (1) {
        old_state = atomic_load(&rw->state);
        
        // Проверяем, нет ли активного писателя
        if ((old_state & WRITER_BIT) == 0) {
            // Нет писателя, пытаемся увеличить счетчик читателей
            new_state = old_state + 2; // +2 потому что младший бит занят под флаг писателя
            
            if (atomic_compare_exchange_weak(&rw->state, &old_state, new_state)) {
                // Успешно захватили блокировку для чтения
                return;
            }
        }
        
        // Есть писатель или не удалось атомарно обновить состояние - ждем
        uint32_t current_state = atomic_load(&rw->state);
        futex((uint32_t*)&rw->state, FUTEX_WAIT, current_state);
    }
}

void rwlock_wrlock(rwlock_t *rw) {
    uint32_t old_state, new_state;
    
    while (1) {
        old_state = atomic_load(&rw->state);
        
        // Проверяем, свободна ли блокировка (нет читателей и писателей)
        if (old_state == 0) {
            // Свободна, пытаемся установить флаг писателя
            new_state = WRITER_BIT;
            
            if (atomic_compare_exchange_weak(&rw->state, &old_state, new_state)) {
                // Успешно захватили блокировку для записи
                return;
            }
        }
        
        // Блокировка занята - ждем
        uint32_t current_state = atomic_load(&rw->state);
        futex((uint32_t*)&rw->state, FUTEX_WAIT, current_state);
    }
}

void rwlock_unlock(rwlock_t *rw) {
    uint32_t old_state = atomic_load(&rw->state);
    uint32_t new_state;
    
    if (old_state & WRITER_BIT) {
        // Разблокировка писателя - просто сбрасываем флаг
        new_state = 0;
    } else {
        // Разблокировка читателя - уменьшаем счетчик
        new_state = old_state - 2;
        // Проверяем на underflow (на всякий случай)
        if (new_state > old_state) { // произошло underflow
            new_state = 0;
        }
    }
    
    // Атомарно обновляем состояние
    atomic_store(&rw->state, new_state);
    
    // Будим всех ожидающих (и читателей и писателей)
    futex((uint32_t*)&rw->state, FUTEX_WAKE, INT32_MAX);
}

void rwlock_destroy(rwlock_t *rw) {
    (void)rw; // Подавление предупреждения о неиспользуемой переменной
}