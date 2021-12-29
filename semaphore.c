//
// Created by pantha on ২৯/১২/২১.
//
#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"
#include "unistd.h"

sem_t bin_sem;
pthread_mutex_t mtx;

void *threadFunction(void *arg){
    int x;
    char msg2[10];
    int loop=0;
    while (loop<4){
        //pthread_mutex_lock(&mtx);
        printf("Thread waiting...");
        sem_wait(&bin_sem);
        printf("hi i am the new thread waiting inside critical..\n");
        scanf("%s",msg2);
        printf("You entered in Thread2:%s\n",msg2);
        sem_post(&bin_sem);
        //pthread_mutex_unlock(&mtx);
        //loop++;
    }
}

int main(){
    pthread_t pthread;
    pthread_attr_t attr;
    char msg[10];
    int x;
    sem_init(&bin_sem,0,1);
    pthread_mutex_init(&mtx,NULL);
    pthread_attr_init(&attr);

    pthread_attr_setschedpolicy(&attr,SCHED_RR);

    pthread_create(&pthread,&attr,threadFunction,NULL);
    int loop=0;
    while (loop<4){
        //pthread_mutex_lock(&mtx);
        printf("Main waiting...\n");
        sem_wait(&bin_sem);
        printf("hi i am the main thread waiting inside critical..\n");
        scanf("%s",msg);
        printf("You entered in Main:%s\n",msg);
        sem_post(&bin_sem);
        //pthread_mutex_unlock(&mtx);
        //loop++;
    }
    sleep(1);
}