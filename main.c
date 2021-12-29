#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "semaphore.h"
#define NUM_THREADS	16
#define M 2
#define N 3
#define P 5
#define w 4
#define x 4
#define y 4
#define z 4
sem_t kioskFull;
pthread_mutex_t kiosk;
pthread_mutex_t belt;

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
    sem_t kioskFull;

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

void showBelt(int kioskIndex,int beltIndex){
    for(int i=0;i<airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger;i++){
        printf("%d\n",airport.kiosks[kioskIndex].belts[beltIndex].passengers[i]);
    }
}

void passBelt(int kioskIndex,int beltIndex,int passID){
    for(int i=0;i<airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger;i++){
        if(airport.kiosks[kioskIndex].belts[beltIndex].passengers[i]==passID){
            for(int j=i;j<airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger-1;j++){
                airport.kiosks[kioskIndex].belts[beltIndex].passengers[j]=airport.kiosks[kioskIndex].belts[beltIndex].passengers[j+1];
            }
            airport.kiosks[kioskIndex].belts[beltIndex].passengers[airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger-1]=-1;
            airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger--;
            break;
        }
    }
}


void *Process(void *threadarg) {
    int passID,vip;
    struct passenger *passengerStatus;
    passengerStatus=(struct passenger_status *)threadarg;
    passID=passengerStatus->id;
    vip=passengerStatus->isVIP;
    //printf("id=%d \nvip=%d \n",passID,vip);
    //pthread_mutex_lock(&mutex);
    int hasEntered=0;
    int kioskIndex,beltIndex;
    if(vip==0) {
//        while (hasEntered == 0) {
            kioskIndex = rand() % M;
            beltIndex = rand() % N;
//            //printf("kiosk idx %d belt idx %d\n",kioskIndex,beltIndex);
//            int isFull = isKioskFull(kioskIndex, beltIndex);
//            if (isFull == 1) {
//                printf("Passenger %d Kiosk %d Belt %d is full\n", passID, kioskIndex, beltIndex);
//            } else {
//                printf("Passenger %d has started self check-in Kiosk %d Belt %d\n", passID, kioskIndex, beltIndex);
//                airport.kiosks[kioskIndex].belts[beltIndex].passengers[airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger] = passID;
//                airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger++;
//                showBelt(kioskIndex,beltIndex);
//                sleep(w);
//                hasEntered = 1;
//            }
//        }

        sem_wait(&airport.kiosks[kioskIndex].belts[beltIndex].kioskFull);
        //sem_post(&airport.kiosks[kioskIndex].belts[beltIndex].kioskFull);
        //printf("hello");
        printf("Passenger %d has started self check-in Kiosk %d Belt %d\n", passID, kioskIndex, beltIndex);
        pthread_mutex_lock(&kiosk);
        airport.kiosks[kioskIndex].belts[beltIndex].passengers[airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger] = passID;
        airport.kiosks[kioskIndex].belts[beltIndex].currentPassenger++;
        showBelt(kioskIndex,beltIndex);
        pthread_mutex_unlock(&kiosk);



        sleep(w);
        pthread_mutex_lock(&belt);
        printf("Passenger %d is in the security check of Kiosk %d Belt %d\n", passID, kioskIndex, beltIndex);
        sleep(x);
        passBelt(kioskIndex,beltIndex,passID);
        sem_post(&airport.kiosks[kioskIndex].belts[beltIndex].kioskFull);
        printf("Passenger %d is going out from Kiosk %d Belt %d\n", passID, kioskIndex, beltIndex);
        showBelt(kioskIndex,beltIndex);
        pthread_mutex_unlock(&belt);

    }
    //pthread_mutex_unlock(&mutex);
    //printf("%d\n",index);

    pthread_exit(NULL);

}
int main (int argc, char *argv[]) {
    pthread_t passengers[NUM_THREADS];
    int rc;
    pthread_mutex_init(&kiosk,NULL);
    pthread_mutex_init(&belt,NULL);
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
            sem_init(&airport.kiosks[i].belts[j].kioskFull,0,P);
            for(int k=0;k<5;k++){
                airport.kiosks[i].belts[j].passengers[k]=-1;
                //printf("Kiosk %d belt %d passenger number %d\n",i,j,k);
            }
        }
    }
    sem_init(&kioskFull,0,0);
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