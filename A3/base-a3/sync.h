/*
 * NAME, etc.
 *
 * sync.h
 */

#ifndef _m_SYNC_H_
#define _m_SYNC_H_

#include "atomic_ops.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <limits.h>

#define MIN_DELAY 10
#define MAX_DELAY 40960

struct my_mutex_struct {
    pid_t lock_thread;
    volatile unsigned long lock_value;
};




typedef struct my_mutex_struct my_mutex_t;


int my_mutex_init(my_mutex_t *lock);
int my_mutex_unlock(my_mutex_t *lock);
int my_mutex_destroy(my_mutex_t *lock);

int my_mutex_lock(my_mutex_t *lock);
int my_mutex_trylock(my_mutex_t *lock);



/*Spinlock Starts here*/

struct my_spinlock_struct {
    pid_t lock_thread;
    volatile unsigned long lock_value;
};

typedef struct my_spinlock_struct my_spinlock_t;

int my_spinlock_init(my_spinlock_t *lock);
int my_spinlock_destroy(my_spinlock_t *lock);
int my_spinlock_unlock(my_spinlock_t *lock);

int my_spinlock_lockTAS(my_spinlock_t *lock);
int my_spinlock_lockTTAS(my_spinlock_t *lock);
int my_spinlock_trylock(my_spinlock_t *lock);


/*queuelock Starts here*/

struct my_queuelock_struct {
    pid_t lock_thread;
    volatile unsigned long now_serving, next_ticket;
};

typedef struct my_queuelock_struct my_queuelock_t;

int my_queuelock_init(my_queuelock_t *lock);
int my_queuelock_destroy(my_queuelock_t *lock);
int my_queuelock_unlock(my_queuelock_t *lock);

int my_queuelock_lock(my_queuelock_t *lock);
int my_queuelock_trylock(my_queuelock_t *lock);


#endif
