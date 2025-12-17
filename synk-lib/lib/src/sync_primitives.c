#include "sync_primitives.h"
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>


static int futex(uint32_t *uaddr, int futex_op, uint32_t val) {
    return syscall(SYS_futex, uaddr, futex_op, val, NULL, NULL, 0);
}



void mutex_init(mutex_t *m) {
    atomic_store(&m->state, 0);
}

void mutex_lock(mutex_t *m) {
    uint32_t expected;
    
    while (1) {
       
        expected = 0;
        if (atomic_compare_exchange_strong(&m->state, &expected, 1)) {
            return;
        }
    
        uint32_t current_state = atomic_load(&m->state);
        futex((uint32_t*)&m->state, FUTEX_WAIT, current_state);
    }
}

void mutex_unlock(mutex_t *m) {
    atomic_store(&m->state, 0);
    
    futex((uint32_t*)&m->state, FUTEX_WAKE, 1);
}

void mutex_destroy(mutex_t *m) {
    (void)m; 
}


void rwlock_init(rwlock_t *rw) {
    atomic_store(&rw->state, 0);
}

void rwlock_rdlock(rwlock_t *rw) {
    uint32_t old_state, new_state;
    
    while (1) {
        old_state = atomic_load(&rw->state);
        
        if ((old_state & WRITER_BIT) == 0) {
            new_state = old_state + 2; 
            
            if (atomic_compare_exchange_weak(&rw->state, &old_state, new_state)) {
                return;
            }
        }
        
        uint32_t current_state = atomic_load(&rw->state);
        futex((uint32_t*)&rw->state, FUTEX_WAIT, current_state);
    }
}

void rwlock_wrlock(rwlock_t *rw) {
    uint32_t old_state, new_state;
    
    while (1) {
        old_state = atomic_load(&rw->state);
        
        if (old_state == 0) {
            new_state = WRITER_BIT;
            
            if (atomic_compare_exchange_weak(&rw->state, &old_state, new_state)) {
                return;
            }
        }
        
        uint32_t current_state = atomic_load(&rw->state);
        futex((uint32_t*)&rw->state, FUTEX_WAIT, current_state);
    }
}

void rwlock_unlock(rwlock_t *rw) {
    uint32_t old_state = atomic_load(&rw->state);
    uint32_t new_state;
    
    if (old_state & WRITER_BIT) {
        new_state = 0;
    } else {
        new_state = old_state - 2;
        if (new_state > old_state) { 
            new_state = 0;
        }
    }
    
    atomic_store(&rw->state, new_state);
    
    futex((uint32_t*)&rw->state, FUTEX_WAKE, INT32_MAX);
}

void rwlock_destroy(rwlock_t *rw) {
    (void)rw; 
}