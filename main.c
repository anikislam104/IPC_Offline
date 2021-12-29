#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "semaphore.h"
#define NUM_THREADS	7
#define M 2
#define N 3
#define P 5
sem_t kioskEmpty;
sem_t kioskFull;
int KIOSK[10];
int kioskIndex;
struct passenger{
    int id;
    int isVIP;
};
struct Belt{
    int passengers[P];
    int currentPassenger;
    int maximumPassenger;

};
struct Kiosk{
    struct Belt belts[N];
};
struct passenger passengerArray[NUM_THREADS];
struct Kiosk kiosks[M];

void *Process(void *threadarg) {
    int passID,vip;
    struct passenger *passengerStatus;
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
    for(int i=0;i<M;i++){
        for(int j=0;j<N;j++){
            kiosks[i].belts[j].currentPassenger=0;
            kiosks[i].belts[j].maximumPassenger=P;
            for(int k=0;k<5;k++){
                kiosks[i].belts[j].passengers[k]=-1;
                //printf("Kiosk %d belt %d passenger number %d\n",i,j,k);
            }
        }
    }
    sem_init(&kioskEmpty,0,M);
    for(int t=0; t<NUM_THREADS; t++){
        passengerArray[t].id=t;
        if(t%2==0){
            passengerArray[t].isVIP=1;
        } else{
            passengerArray[t].isVIP=0;
        }
        printf("Passenger generated:%d\n",t);
        rc= pthread_create(&passengers[t],NULL,Process,(void *)&passengerArray[t]);
        if(rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    pthread_exit(NULL);
    return  0;
}