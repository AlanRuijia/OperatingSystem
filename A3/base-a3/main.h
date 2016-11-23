#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include "sync.h"


long long c = 0;
int numThreads;
int numIterations;
int workOutsideCS;
int workInsideCS;
int testID;
pthread_mutex_t count_mutex;
pthread_spinlock_t count_spin_lock;
my_spinlock_t count_my_spin_lock;
my_mutex_t count_my_mutex;
my_queuelock_t count_my_queuelock;


unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

