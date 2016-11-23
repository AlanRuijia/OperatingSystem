/*
 * NAME, etc.
 *
 * sync.c
 *
 */

#define _REENTRANT

#include "sync.h"


/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock){
    if (lock == NULL){
        printf("Lock is NULL");
        return -1;
    }
    lock -> lock_value = 0;
    lock -> lock_thread = -1;
    return 0;
}

int my_spinlock_destroy(my_spinlock_t *lock){
    if (lock == NULL){
        printf("Lock is NULL");
        return -1;
    }
    lock -> lock_value = 2;
    lock -> lock_thread = -1;
    return 0;
}

int my_spinlock_unlock(my_spinlock_t *lock){ 
    if (lock == NULL){
        printf("Lock is NULL");
        return -1;
    }
    lock -> lock_value = 0;
    lock -> lock_thread = -1;
    return 0;
}

int my_spinlock_lockTAS(my_spinlock_t *lock){
    unsigned long val;
    if (lock->lock_thread == syscall(SYS_gettid)){
        // printf("Thread with lock tries to acuire lock");
        return 0;
    }
    while ((val = tas(&lock -> lock_value)) != 0)
        ;
    if (val == 2){
        printf("Destroyed Uninitialized Spinlock\n");
        return -1;
    }
    if (val != 0){
        printf("Uninitialized Spinlock\n");
        return -1;
    }
    lock -> lock_thread = syscall(SYS_gettid);
    return 0;
}


int my_spinlock_lockTTAS(my_spinlock_t *lock){
    unsigned long val;
    if (lock->lock_thread == syscall(SYS_gettid)){
        // printf("Thread with lock tries to acuire lock");
        return 0;
    }
    while (1){
        while (lock -> lock_value == 1)
            ;
        if ((val = tas(&lock -> lock_value)) == 0)
            return 0;
        if (val == 2){
            printf("Destroyed Uninitialized Spinlock\n");
            return -1;
        }
    }
}

int my_spinlock_trylock(my_spinlock_t *lock){
    unsigned long val;
    if (lock->lock_thread == syscall(SYS_gettid)){
        // printf("Thread with lock tries to acuire lock");
        return 0;
    }
    if ((val = tas(&lock -> lock_value)) == 0)
        return 0;
    if (val == 2){
        printf("Destroyed Uninitialized Spinlock\n");
        return -1;
    }
    if (val == 1){
        printf("Spinlock Busy\n");
        return -2;
    }
    printf("Spinlock trylock error\n");
    return -3;
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *lock){
    if (lock == NULL){
        printf("Lock is NULL");
        return -1;
    }
    lock -> lock_value = 0;
    lock -> lock_thread = -1;
    return 0;
}

int my_mutex_destroy(my_mutex_t *lock){
    if (lock == NULL){
        printf("Lock is NULL");
        return -1;
    }
    lock -> lock_value = 2;
    lock -> lock_thread = -1;
    return 0;
}

int my_mutex_unlock(my_mutex_t *lock){
    if (lock == NULL){
        printf("Lock is NULL");
        return -1;
    }
    lock -> lock_value = 0;
    lock -> lock_thread = -1;
    return 0;
}

int my_mutex_lock(my_mutex_t *lock){
    unsigned long val;
    long delay = MIN_DELAY;
    
    // useconds_t req = 0;
    struct timespec req, rem;
    if (lock -> lock_thread == syscall(SYS_gettid)){
        // printf("Thread with lock tries to acuire lock");
        return 0;
    }
    req.tv_sec = 0;
    req.tv_nsec = 0;
    while (1){
        while (lock -> lock_value == 1)
            ;
        if ((val = tas(&lock -> lock_value)) == 0)
            return 0;
        if (val == 2){
            printf("Destroyed Uninitialized Spinlock\n");
            return -1;
        }
        req.tv_nsec = rand() % delay;
        nanosleep(&req, &rem);
        // req = rand() % delay;
        // usleep(req);
        if (delay < MAX_DELAY)
            delay *= 2;
    }
}

int my_mutex_trylock(my_mutex_t *lock){
    unsigned long val;
    if (lock->lock_thread == syscall(SYS_gettid)){
        // printf("Thread with lock tries to acuire lock");
        return 0;
    }
    if ((val = tas(&lock -> lock_value)) == 0)
        return 0;
    if (val == 2){
        printf("Destroyed Uninitialized Mutex lock\n");
        return -1;
    }
    if (val == 1){
        printf("Mutex lock Busy\n");
        return -2;
    }
    printf("Mutex lock trylock error\n");
    return -3;
}

/*
 * Queue Lock
 */

int my_queuelock_init(my_queuelock_t *lock){
    if (lock == NULL){
        printf("Lock is NULL");
        return -1;
    }
    lock -> now_serving = 0;
    lock -> next_ticket = 0;
    lock -> lock_thread = -1;
    return 0;
}

int my_queuelock_destroy(my_queuelock_t *lock){
    if (lock == NULL){
        printf("Lock is NULL");
        return -1;
    }
    lock -> now_serving = -1;
    lock -> next_ticket = -1;
    lock -> lock_thread = -1;

    return 0;
}

int my_queuelock_unlock(my_queuelock_t *lock){
    if (lock == NULL){
        printf("Lock is NULL");
        return -1;
    }
    if (lock -> now_serving == ULONG_MAX){
        lock -> now_serving = 0;
        return 0;
    }
    lock -> now_serving++;
    return 0;
}

int my_queuelock_lock(my_queuelock_t *lock){ 
    unsigned long my_ticket;
    printf("entering lock");
    if (lock -> lock_thread == syscall(SYS_gettid))
        return 0;
    my_ticket = anf(lock -> next_ticket, 1);
    if (lock -> next_ticket == ULONG_MAX)
        lock -> next_ticket = 0;
    while(lock -> now_serving != my_ticket) 
        ;
    lock -> lock_thread = syscall(SYS_gettid);
    return 0;

}

int my_queuelock_trylock(my_queuelock_t *lock){
    if (lock -> lock_thread == syscall(SYS_gettid))
        return 0;
    if (lock -> next_ticket == lock -> now_serving){
        lock -> lock_thread = syscall(SYS_gettid);
        return 0;
    }
    return -1;
}

