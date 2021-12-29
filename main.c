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
pthread_mutex_t mutex;
int KIOSK[10];
int kioskIndex;

struct Boarding{
    int passenger[M*N*P];
    int currentPassenger;
};
struct VIPchannel{
    int passengerGoing[M*N*P];
    int passengerWaiting[M*N*P];
    int PassengerComingBack[M*N*P];
    int going;
    int waiting;
    int comingBack;
};
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

struct Airport{
    struct Kiosk kiosks[M];
    struct Boarding boarding;
    struct VIPchannel viPchannel;
};
struct Airport airport;
struct passenger passengerArray[NUM_THREADS];
//struct Kiosk kiosks[M];

int isKioskFull(int kioskIndex,int beltIndex){
    if(airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger==P){
        return 1;
    } else{
        return 0;
    }
}

void *Process(void *threadarg) {
    int passID,vip;
    struct passenger *passengerStatus;
    passengerStatus=(struct passenger_status *)threadarg;
    passID=passengerStatus->id;
    vip=passengerStatus->isVIP;
    //printf("id=%d \nvip=%d \n",passID,vip);
    pthread_mutex_lock(&mutex);
    int hasEntered=0;
    if(vip==0) {
        while (hasEntered == 0) {
            int kioskIndex = rand() % M;
            int beltIndex = rand() % N;
            //printf("kiosk idx %d belt idx %d\n",kioskIndex,beltIndex);
            int isFull = isKioskFull(kioskIndex, beltIndex);
            if (isFull == 1) {
                printf("Passenger %d Kiosk %d Belt %d is full\n", passID, kioskIndex, beltIndex);
            } else {
                printf("Passenger %d is going through Kiosk %d Belt %d\n", passID, kioskIndex, beltIndex);
                airport.kiosks[kioskIndex].belts[beltIndex].passengers[airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger] = passID;
                airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger++;
                hasEntered = 1;
            }
        }

    }
    pthread_mutex_unlock(&mutex);
    //printf("%d\n",index);

    pthread_exit(NULL);

}
int main (int argc, char *argv[]) {
    pthread_t passengers[NUM_THREADS];
    int rc;
    pthread_mutex_init(&mutex,NULL);
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
            airport.kiosks[i].belts[j].currentPassenger=0;
            airport.kiosks[i].belts[j].maximumPassenger=P;
            for(int k=0;k<5;k++){
                airport.kiosks[i].belts[j].passengers[k]=-1;
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
        //printf("Passenger generated:%d\n",t);
        rc= pthread_create(&passengers[t],NULL,Process,(void *)&passengerArray[t]);
        if(rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    pthread_exit(NULL);
    return  0;
}