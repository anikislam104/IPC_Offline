#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "semaphore.h"
#define NUM_THREADS	2
#define M 5
#define N 3
#define P 5
#define w 2
#define x 4
#define y 4
#define z 4
sem_t kioskFull;
pthread_mutex_t kiosk;
pthread_mutex_t belt;
pthread_mutex_t boarding;
int TIME=0;
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
    int time;
};
struct Belt{
    int passengers[P];
    int currentPassenger;
    int maximumPassenger;
    sem_t beltFull;

};
struct Kiosk{
    //struct Belt belts[N];
    int passengers[1];
    int currentPassenger;
    int maximumPassenger;
    sem_t kioskFull;
};

struct Airport{
    struct Kiosk kiosks[M];
    struct Belt belts[N];
    struct Boarding boarding;
    struct VIPchannel viPchannel;
};
struct Airport airport;
struct passenger passengerArray[NUM_THREADS];
//struct Kiosk kiosks[M];

int isKioskFull(int kioskIndex,int beltIndex){
    if(airport.kiosks[kioskIndex].currentPassenger==P){
        return 1;
    } else{
        return 0;
    }
}

void showKiosk(int kioskIndex){
    for(int i=0;i<airport.kiosks[kioskIndex].currentPassenger;i++){
        printf("%d\n",airport.kiosks[kioskIndex].passengers[i]);
    }
}

void showBelt(int beltIndex){
    for(int i=0;i<airport.belts[beltIndex].currentPassenger;i++){
        printf("%d\n",airport.belts[beltIndex].passengers[i]);
    }
}

void passKiosk(int kioskIndex,int passID){
    for(int i=0;i<airport.kiosks[kioskIndex].currentPassenger;i++){
        if(airport.kiosks[kioskIndex].passengers[i]==passID){
            for(int j=i;j<airport.kiosks[kioskIndex].currentPassenger-1;j++){
                airport.kiosks[kioskIndex].passengers[j]=airport.kiosks[kioskIndex].passengers[j+1];
            }
            airport.kiosks[kioskIndex].passengers[airport.kiosks[kioskIndex].currentPassenger-1]=-1;
            airport.kiosks[kioskIndex].currentPassenger--;
            break;
        }
    }
}

void passBelt(int beltIndex,int passID){
    for(int i=0;i<airport.belts[beltIndex].currentPassenger;i++){
        if(airport.belts[beltIndex].passengers[i]==passID){
            for(int j=i;j<airport.belts[beltIndex].currentPassenger-1;j++){
                airport.belts[beltIndex].passengers[j]=airport.belts[beltIndex].passengers[j+1];
            }
            airport.belts[beltIndex].passengers[airport.belts[beltIndex].currentPassenger-1]=-1;
            airport.belts[beltIndex].currentPassenger--;
            break;
        }
    }
}
int myBeltPosition(int beltIndex,int passID){
    for(int i=0;i<airport.belts[beltIndex].currentPassenger;i++){
        if(airport.belts[beltIndex].passengers[i]==passID){
            return i;
        }
    }
}

void addPassengerInBoardingLine(int passID){
    airport.boarding.passenger[airport.boarding.currentPassenger]=1;
    airport.boarding.currentPassenger++;
}

void passBoarding(int passID){
    for(int i=0;i<airport.boarding.currentPassenger;i++){
        if(airport.boarding.passenger[i]==passID){
            for(int j=i;j<airport.boarding.currentPassenger-1;j++){
                airport.boarding.passenger[j]=airport.boarding.passenger[j+1];
            }
            airport.boarding.passenger[airport.boarding.currentPassenger-1]=-1;
            airport.boarding.currentPassenger--;
            break;
        }
    }
}

int random=0;
void *Process(void *threadarg) {
    int passID,vip,ptime;
    struct passenger *passengerStatus;
    passengerStatus=(struct passenger_status *)threadarg;
    passID=passengerStatus->id;
    vip=passengerStatus->isVIP;
    ptime=passengerStatus->time;
    //printf("id=%d \nvip=%d \n",passID,vip);
    //pthread_mutex_lock(&mutex);
    int kioskIndex,beltIndex;
    if(vip==0) {
        ptime+=7;
        kioskIndex = (random++)%M;
        beltIndex =(random++)%N;
        if(random==55) random=0;

        //KIOSK CHECK-IN

        sem_wait(&airport.kiosks[kioskIndex].kioskFull);
        printf("Passenger %d has started self check-in Kiosk %d at time %d\n", passID, kioskIndex,ptime);
        pthread_mutex_lock(&kiosk);
        airport.kiosks[kioskIndex].passengers[airport.kiosks[kioskIndex].currentPassenger] = passID;
        airport.kiosks[kioskIndex].currentPassenger++;
        //showKiosk(kioskIndex);
        sleep(w);
        ptime+=w;
        pthread_mutex_unlock(&kiosk);
        printf("Passenger %d has finished self check-in Kiosk %d at time %d\n", passID, kioskIndex,ptime);
        passKiosk(kioskIndex,passID);
        sem_post(&airport.kiosks[kioskIndex].kioskFull);
        ptime++;


        //SECURITY CHECK(BELT)


        //pthread_mutex_lock(&belt);
        sem_wait(&airport.belts[beltIndex].beltFull);
        printf("Passenger %d has started waiting for security check in belt %d from time %d\n", passID, beltIndex, ptime);
        pthread_mutex_lock(&belt);
        airport.belts[beltIndex].passengers[airport.belts[beltIndex].currentPassenger]=passID;
        ptime+=airport.belts[beltIndex].currentPassenger*x;
        airport.belts[beltIndex].currentPassenger++;

        printf("Passenger %d has started the security check at time %d\n",passID,ptime);
        ptime+=x;

        sleep(x);
        passBelt(beltIndex,passID);

//        printf("Passenger %d is going out from Kiosk %d Belt %d\n", passID, kioskIndex, beltIndex);
        pthread_mutex_unlock(&belt);
        sem_post(&airport.belts[beltIndex].beltFull);
        printf("Passenger %d has crossed the security check at time %d\n",passID,ptime);
//
        //showBelt(kioskIndex,beltIndex);

        //BOARDING
        ptime++;
        printf("Passenger %d has started waiting to be boarded at time %d\n",passID,ptime);
        addPassengerInBoardingLine(passID);
        ptime+=(airport.boarding.currentPassenger-1)*y;
        pthread_mutex_lock(&boarding);
        printf("Passenger %d has started boarding the plane at time %d\n",passID,ptime);
        ptime+=y;
        passBoarding(passID);
        printf("Passenger %d has boarded the plane at time %d\n",passID,ptime);
        pthread_mutex_unlock(&boarding);

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
    pthread_mutex_init(&boarding,NULL);

    for(int i=0;i<M;i++){
            airport.kiosks[i].currentPassenger=0;
            airport.kiosks[i].maximumPassenger=1;
            sem_init(&airport.kiosks[i].kioskFull,0,1);
            for(int k=0;k<airport.kiosks[i].maximumPassenger;k++){
                airport.kiosks[i].passengers[k]=-1;
                //printf("Kiosk %d belt %d passenger number %d\n",i,j,k);
            }
    }
    for(int i=0;i<N;i++){
        airport.belts[i].currentPassenger=0;
        airport.belts[i].maximumPassenger=P;
        sem_init(&airport.belts[i].beltFull,0,P);
        for(int k=0;k<P;k++){
            airport.belts[i].passengers[k]=-1;
            //printf("Kiosk %d belt %d passenger number %d\n",i,j,k);
        }
    }
    airport.boarding.currentPassenger=0;
    for(int i=0;i<M*N*P;i++){
        airport.boarding.passenger[i]=-1;
    }
    //sem_init(&kioskFull,0,0);
    for(int t=0; t<NUM_THREADS; t++){
        passengerArray[t].id=t;
        if(t%2==0){
            passengerArray[t].isVIP=1;
        } else{
            passengerArray[t].isVIP=0;
        }
        passengerArray[t].time=TIME;
        TIME+=2;
        printf("Passenger %d has arrived at the airport at time %d\n",t,passengerArray[t].time);
        rc= pthread_create(&passengers[t],NULL,Process,(void *)&passengerArray[t]);
        if(rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    pthread_exit(NULL);
    return  0;
}