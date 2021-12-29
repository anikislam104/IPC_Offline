#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "semaphore.h"
#define NUM_THREADS	7
int M;
sem_t kioskEmpty;
sem_t kioskFull;
int KIOSK[10];
int kioskIndex;
struct passenger_status{
    int id;
    int isVIP;
};

struct passenger_status passengerStatusArray[NUM_THREADS];

void *Process(void *threadarg) {
    int passID,vip;
    struct passenger_status *passengerStatus;
    passengerStatus=(struct passenger_status *)threadarg;
    passID=passengerStatus->id;
    vip=passengerStatus->isVIP;
    printf("id=%d \nvip=%d \n",passID,vip);
    int index=rand()%10;
    //printf("%d\n",index);

    pthread_exit(NULL);

}
int main (int argc, char *argv[]) {
    pthread_t passengers[NUM_THREADS];
    int rc;
//    printf("Input M:");
//    scanf("%d\n",M);
//    printf("\n");
//    printf("%d\n",M);
//    for(int i=0;i<M;i++){
//        KIOSK[i]=-1;
//    }
//    printf("%d",KIOSK[M-2]);
    sem_init(&kioskEmpty,0,M);
    for(int t=0; t<NUM_THREADS; t++){
        passengerStatusArray[t].id=t;
        if(t%2==0){
            passengerStatusArray[t].isVIP=1;
        } else{
            passengerStatusArray[t].isVIP=0;
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