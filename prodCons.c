#include "stdio.h"
#include "pthread.h"
#include "semaphore.h"
#include "unistd.h"
#include "stdlib.h"

sem_t empty;
sem_t full;
pthread_mutex_t mutex;


