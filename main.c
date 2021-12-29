#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "semaphore.h"
#define NUM_THREADS	2
sem_t kioskEmpty;
sem_t kioskFull;
struct passenger_status{
    int id;
    int isVIP;
};

struct passenger_status passengerStatusArray[NUM_THREADS];

void *Process(void *threadid) {


}
int main (int argc, char *argv[]) {
    pthread_t passengers[NUM_THREADS];
    int rc;
    int M;
    printf("Input M:");
    scanf("%d",M);
    printf("\n");
    int KIOSK[M];
    sem_init(&kioskEmpty,0,M);
    for(int t=0; t<NUM_THREADS; t++){
        passengerStatusArray[t].id=t;
        if(t%2==0){
            passengerStatusArray->isVIP=1;
        } else{
            passengerStatusArray->isVIP=0;
        }
        printf("Passenger generated:%d\n",t);
        rc= pthread_create(&passengers[t],NULL,Process,(void *)&passengerStatusArray[t]);
        if(rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    pthread_exit(NULL);
    return  0;
}