//
// Created by aniki on 12/31/2021.
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "semaphore.h"
#include "time.h"
#include "math.h"
#define NUM_THREADS	4
//#define M 5
//#define N 3
//#define P 5
//#define w 2
//#define x 4
//#define y 4
//#define z 1
#define whomToPrint 1
clock_t start,end;
int M,N,P,w,x,y,z;
//sem_t kioskFull;
pthread_mutex_t kiosk;
pthread_mutex_t spkiosk;
pthread_mutex_t belt;
pthread_mutex_t boarding;
pthread_mutex_t vipCh;
int TIME=0;
struct Boarding{
    int passenger[10000];
    int currentPassenger;
    sem_t boarding;
    pthread_mutex_t mutex;
};
struct VIPchannel{
    int passengerGoing[10000];
    int passengerWaiting[10000];
    int passengerComingBack[10000];
    int going;
    int waiting;
    int comingBack;
    pthread_mutex_t mutex;
};
struct passenger{
    int id;
    int isVIP;
    int time;
    //pthread_mutex_t mutex;
};
struct Belt{
    int passengers[10000];
    int currentPassenger;
    int maximumPassenger;
    sem_t beltFull;
    pthread_mutex_t mutex;

};
struct Kiosk{
    //struct Belt belts[N];
    int passengers[10000];
    int currentPassenger;
    int maximumPassenger;
    sem_t kioskFull;
    pthread_mutex_t mutex;
};

struct Airport{
    struct Kiosk *kiosks;
    struct Belt *belts;
    struct Boarding boarding;
    struct VIPchannel viPchannel;
    struct Kiosk specialKiosk;
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
    airport.boarding.passenger[airport.boarding.currentPassenger]=passID;
    airport.boarding.currentPassenger++;
}

void passBoarding(int passID){
    for(int i=0;i<airport.boarding.currentPassenger;i++){
        if(airport.boarding.passenger[i]==passID){
            for(int j=i;j<airport.boarding.currentPassenger-1;j++){
                airport.boarding.passenger[j]=airport.boarding.passenger[j+1];
            }
            airport.boarding.passenger[airport.boarding.currentPassenger-1]=-1;
            //printf("what %d\n",airport.boarding.currentPassenger);
            airport.boarding.currentPassenger--;
            //printf("if %d\n",airport.boarding.currentPassenger);
            break;
        }
    }
}

int addVIPChannelComingBack(int passID){
    if(airport.viPchannel.going==0 && airport.viPchannel.waiting==0){
        airport.viPchannel.passengerComingBack[airport.viPchannel.comingBack]=passID;
        airport.viPchannel.comingBack++;
        return 1;
    }
    else{
        return 0;
    }

}

void passComingBack(int passID){
    for(int i=0;i<airport.viPchannel.comingBack;i++){
        if(airport.viPchannel.passengerComingBack[i]==passID){
            for(int j=i;j<airport.viPchannel.comingBack-1;j++){
                airport.viPchannel.passengerComingBack[j]=airport.viPchannel.passengerComingBack[j+1];
            }
            airport.viPchannel.passengerComingBack[airport.viPchannel.comingBack-1]=-1;
            airport.viPchannel.comingBack--;
            break;
        }
    }
}

int addGoing(int passID){
    if(airport.viPchannel.comingBack==0){
        airport.viPchannel.passengerGoing[airport.viPchannel.going]=passID;
        airport.viPchannel.going++;
        return 1;
    }
    else{
        return 0;
    }
}

void passGoing(int passID){
    for(int i=0;i<airport.viPchannel.going;i++){
        if(airport.viPchannel.passengerGoing[i]==passID){
            for(int j=i;j<airport.viPchannel.going-1;j++){
                airport.viPchannel.passengerGoing[j]=airport.viPchannel.passengerGoing[j+1];
            }
            airport.viPchannel.passengerGoing[airport.viPchannel.going-1]=-1;
            airport.viPchannel.going--;
            break;
        }
    }
}
int getTime(double t){
    t= round(t);
    int t1=(int )t;
    return t1;
}

void passSpecialKiosk(int pass){
    airport.specialKiosk.passengers[airport.specialKiosk.currentPassenger-1] = -1;
    //airport.viPchannel.comingBack=0;
    //printf("%d\n",airport.viPchannel.comingBack);
    airport.specialKiosk.currentPassenger--;
}
//int peopleKiosk;
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
        //ptime+=7;
        kioskIndex = (random++)%M;
        beltIndex =(random++)%N;
        if(random==55) random=0;

        //KIOSK CHECK-IN

        //sem_wait(&airport.kiosks[kioskIndex].kioskFull);
        //printf("%d\n",airport.kiosks[kioskIndex].currentPassenger);
        if(airport.kiosks[kioskIndex].currentPassenger!=0) {
            ptime += airport.kiosks[kioskIndex].currentPassenger * w - 2;
        }
        airport.kiosks[kioskIndex].passengers[airport.kiosks[kioskIndex].currentPassenger] = passID;
        airport.kiosks[kioskIndex].currentPassenger++;
        pthread_mutex_lock(&airport.kiosks[kioskIndex].mutex);
        sleep(w);
        //ptime+=airport.kiosks[kioskIndex].currentPassenger*w;
        printf("Passenger %d has started self check-in Kiosk %d at time %d\n", passID, kioskIndex,ptime);
        //printf("%d\n",airport.kiosks[kioskIndex].currentPassenger);
        //showKiosk(kioskIndex);
        ptime+=w;
        pthread_mutex_unlock(&airport.kiosks[kioskIndex].mutex);
        printf("Passenger %d has finished self check-in Kiosk %d at time %d\n", passID, kioskIndex,ptime);
        passKiosk(kioskIndex,passID);
        //sem_post(&airport.kiosks[kioskIndex].kioskFull);
        ptime++;
        sleep(1);


        //SECURITY CHECK(BELT)


        //pthread_mutex_lock(&belt);

        sem_wait(&airport.belts[beltIndex].beltFull);
        ptime+=airport.belts[beltIndex].currentPassenger*x;
        printf("Passenger %d has started waiting for security check in belt %d from time %d\n", passID, beltIndex, ptime);
        airport.belts[beltIndex].passengers[airport.belts[beltIndex].currentPassenger]=passID;
        airport.belts[beltIndex].currentPassenger++;
        pthread_mutex_lock(&airport.belts[beltIndex].mutex);
        printf("Passenger %d has started the security check at time %d\n",passID,ptime);
        ptime+=x;
        sleep(x);
        pthread_mutex_unlock(&airport.belts[beltIndex].mutex);
        passBelt(beltIndex,passID);
        sem_post(&airport.belts[beltIndex].beltFull);
        printf("Passenger %d has crossed the security check at time %d\n",passID,ptime);




        //BOARDING
        //sem_wait(&airport.boarding.boarding);
        ptime++;
        sleep(1);
        printf("Passenger %d has started waiting to be boarded at time %d\n",passID,ptime);
        addPassengerInBoardingLine(passID);
        ptime+=(airport.boarding.currentPassenger-1)*y;
        sleep((airport.boarding.currentPassenger-1)*y);
        //pthread_mutex_lock(&boarding);
        if(passID==0) {
            printf("Passenger %d has started boarding the plane at time %d\n", passID, ptime);
            pthread_mutex_lock(&airport.boarding.mutex);
            sleep(y);
            ptime += y;
            pthread_mutex_unlock(&airport.boarding.mutex);
            passBoarding(passID);
            printf("Passenger %d has boarded the plane at time %d\n", passID, ptime);
        }
        else{
            passBoarding(passID);
            printf("Passenger %d has started boarding the plane at time %d\n", passID, ptime);
            printf("Passenger %d has lost his boarding pass.\nNow he has to go back to kiosk via VIP channel at time %d\n",passID,ptime);
            int isComingBack=0;
            while (isComingBack==0){
                if(addVIPChannelComingBack(passID)==1){
                    ptime++;
                    sleep(1);
                    isComingBack=1;
                    printf("Passenger %d is going back to special kiosk via VIP channel at time %d\n",passID,ptime);
                }
            }





            //SPECIAL KIOSK
            pthread_mutex_lock(&airport.viPchannel.mutex);
            passComingBack(passID);
            ptime+=z;
            sleep(z);
            pthread_mutex_unlock(&airport.viPchannel.mutex);



            //SPECIAL KIOSK

            //sem_wait(&airport.specialKiosk.kioskFull);
            ptime+=airport.specialKiosk.currentPassenger*w;
            printf("Passenger %d has started self check-in Special Kiosk  at time %d\n", passID,ptime);
            airport.specialKiosk.passengers[airport.specialKiosk.currentPassenger] = passID;
            airport.specialKiosk.currentPassenger++;
            pthread_mutex_lock(&airport.specialKiosk.mutex);
            //showKiosk(kioskIndex);
            sleep(w);
            ptime+=w;
            pthread_mutex_unlock(&airport.specialKiosk.mutex);

            printf("Passenger %d has finished self check-in Special Kiosk  at time %d\n", passID,ptime);
//            printf("%d\n",airport.viPchannel.comingBack);
            passSpecialKiosk(passID);
            //sem_post(&airport.specialKiosk.kioskFull);
            //ptime++;


            //VIP CHANNEL
            airport.viPchannel.waiting++;

            while(1){
                ptime++;
                sleep(1);
                if(addGoing(passID)==1){
                    printf("Passenger %d is going back to boarding via VIP channel at time %d\n",passID,ptime);
                    airport.viPchannel.waiting--;
                    break;
                }
                //printf("looped");
            }
            passGoing(passID);
            sleep(z);
            ptime+=z;
            printf("Passenger %d has started waiting to be boarded at time %d\n",passID,ptime);
            //printf("%d\n",airport.boarding.currentPassenger);
            addPassengerInBoardingLine(passID);
            ptime+=(airport.boarding.currentPassenger-1)*y;
            pthread_mutex_lock(&airport.boarding.mutex);
            printf("Passenger %d has started boarding the plane at time %d\n", passID, ptime);
            ptime += y;
            sleep(y);
            pthread_mutex_unlock(&airport.boarding.mutex);
            //pthread_mutex_lock(&airport.boarding.mutex);
            passBoarding(passID);

            //sem_post(&airport.boarding.boarding);
            printf("Passenger %d has boarded the plane at time %d\n", passID, ptime);

        }
//        pthread_mutex_unlock(&boarding);
//        sem_post(&airport.boarding.boarding);

    }



        //FOR VIPs



    else{
        //ptime+=7;
        //printf("ptime %d\n",airport.kiosks[kioskIndex].currentPassenger);
        kioskIndex = (random++)%M;
        beltIndex =(random++)%N;
        if(random==55) random=0;
        //printf("ptime %d  %d\n",airport.kiosks[kioskIndex].currentPassenger,kioskIndex);

        //KIOSK CHECK-IN


        if(airport.kiosks[kioskIndex].currentPassenger!=0) {
            ptime += airport.kiosks[kioskIndex].currentPassenger * w;
        }
        //printf("ptime %d\n",airport.kiosks[kioskIndex].currentPassenger);
        //sem_wait(&airport.kiosks[kioskIndex].kioskFull);
        airport.kiosks[kioskIndex].passengers[airport.kiosks[kioskIndex].currentPassenger] = passID;
        airport.kiosks[kioskIndex].currentPassenger++;
        pthread_mutex_lock(&airport.kiosks[kioskIndex].mutex);
        printf("Passenger %d has started self check-in Kiosk %d at time %d\n", passID, kioskIndex,ptime);
        sleep(w);
        ptime+=w;
        pthread_mutex_unlock(&airport.kiosks[kioskIndex].mutex);
        printf("Passenger %d has finished self check-in Kiosk %d at time %d\n", passID, kioskIndex,ptime);
        passKiosk(kioskIndex,passID);
        //sem_post(&airport.kiosks[kioskIndex].kioskFull);
        //ptime++;



        //VIP CHANNEL


        airport.viPchannel.waiting++;
        while(1){
            ptime++;
            sleep(1);
            if(addGoing(passID)==1){
                printf("Passenger %d is going  to boarding via VIP channel at time %d\n",passID,ptime);
                airport.viPchannel.waiting--;
                break;
            }
            //printf("looped");
        }
        passGoing(passID);
        ptime+=z;
        sleep(z);




        //BOARDING



        //sem_wait(&airport.boarding.boarding);
        ptime++;
        sleep(1);
        printf("Passenger %d has started waiting to be boarded at time %d\n",passID,ptime);
        addPassengerInBoardingLine(passID);
        ptime+=(airport.boarding.currentPassenger-1)*y;
        sleep((airport.boarding.currentPassenger-1)*y);
        //pthread_mutex_lock(&boarding);
        if(passID==0) {
            printf("Passenger %d has started boarding the plane at time %d\n", passID, ptime);
            //sem_wait(&airport.boarding.boarding);
            pthread_mutex_lock(&airport.boarding.mutex);
            sleep(y);
            ptime += y;
            pthread_mutex_unlock(&airport.boarding.mutex);
            passBoarding(passID);
            //sem_post(&airport.boarding.boarding);
            printf("Passenger %d has boarded the plane at time %d\n", passID, ptime);
        }
        else{
            passBoarding(passID);
            //sem_wait(&airport.boarding.boarding);
//            pthread_mutex_lock(&boarding);
            printf("Passenger %d has started boarding the plane at time %d\n", passID, ptime);
            printf("Passenger %d has lost his boarding pass.\nNow he has to go back to kiosk via VIP channel at time %d\n",passID,ptime);
            int isComingBack=0;
            while (isComingBack==0){
                if(addVIPChannelComingBack(passID)==1){
                    ptime++;
                    sleep(1);
                    isComingBack=1;
                    printf("Passenger %d is going back to special kiosk via VIP channel at time %d\n",passID,ptime);
                }

            }




            //VIP CHANNEL




            pthread_mutex_lock(&airport.viPchannel.mutex);
            passComingBack(passID);
            //printf("%d",airport.viPchannel.comingBack);
            ptime+=z;
            sleep(z);
            pthread_mutex_unlock(&airport.viPchannel.mutex);


            //SPECIAL KIOSK



            //sem_wait(&airport.specialKiosk.kioskFull);
            ptime+=airport.specialKiosk.currentPassenger*w;
            airport.specialKiosk.passengers[airport.specialKiosk.currentPassenger] = passID;
            airport.specialKiosk.currentPassenger++;
            printf("Passenger %d has started self check-in Special Kiosk  at time %d\n", passID,ptime);
            pthread_mutex_lock(&airport.specialKiosk.mutex);
            sleep(w);
            ptime+=w;
            pthread_mutex_unlock(&airport.specialKiosk.mutex);
            printf("Passenger %d has finished self check-in Special Kiosk  at time %d\n", passID,ptime);
//            printf("%d\n",airport.viPchannel.comingBack);
            passSpecialKiosk(passID);
            //sem_post(&airport.specialKiosk.kioskFull);
            //ptime++;


            //VIP CHANNEL
            airport.viPchannel.waiting++;
            while(1){
                ptime++;
                sleep(1);
                if(addGoing(passID)==1){
                    printf("Passenger %d is going back to boarding via VIP channel at time %d\n",passID,ptime);
                    airport.viPchannel.waiting--;
                    break;
                }
                //printf("looped");
            }
            passGoing(passID);
            ptime+=z;
            sleep(z);
            printf("Passenger %d has started waiting to be boarded at time %d\n",passID,ptime);
            //printf("%d\n",airport.boarding.currentPassenger);
            addPassengerInBoardingLine(passID);
            ptime+=(airport.boarding.currentPassenger-1)*y;
            pthread_mutex_lock(&airport.boarding.mutex);
            printf("Passenger %d has started boarding the plane at time %d\n", passID, ptime);
            ptime += y;
            sleep(y);
            pthread_mutex_unlock(&airport.boarding.mutex);
            //sem_wait(&airport.boarding.boarding);
            //pthread_mutex_lock(&airport.boarding.mutex);
            passBoarding(passID);
            //sem_post(&airport.boarding.boarding);
            printf("Passenger %d has boarded the plane at time %d\n", passID, ptime);

        }

    }
    //pthread_mutex_unlock(&mutex);
    //printf("%d\n",index);

    pthread_exit(NULL);

}
int main()
{

    FILE *fptr;

    if ((fptr = fopen("D:/L3T2/Sessional/314 OS/Offline2/input.txt","r")) == NULL){
        printf("Error! opening file");

        // Program exits if the file pointer returns NULL.
        exit(1);
    }

    fscanf(fptr,"%d", &M);
    fscanf(fptr,"%d", &N);
    fscanf(fptr,"%d", &P);
    fscanf(fptr,"%d", &w);
    fscanf(fptr,"%d", &x);
    fscanf(fptr,"%d", &y);
    fscanf(fptr,"%d", &z);



    //printf("Value of z=%d\n", z);
    fclose(fptr);
//    int *arr;
//    arr=(int*) malloc(5*sizeof(int ));
//    arr[0]=1;
//    arr[4]=4;
//    arr[5]=3;
//    arr[100]=4;
//    printf("%d\n",arr[100]);
    pthread_t passengers[NUM_THREADS];
    int rc;
    pthread_mutex_init(&kiosk,NULL);
    pthread_mutex_init(&belt,NULL);
    pthread_mutex_init(&boarding,NULL);
    pthread_mutex_init(&spkiosk,NULL);
    pthread_mutex_init(&vipCh,NULL);
    airport.kiosks=(struct Kiosk *) malloc(M* sizeof(struct Kiosk));
    airport.belts=(struct Belt *) malloc(N* sizeof(struct Belt));
    sem_init(&airport.specialKiosk.kioskFull,0,1);
    pthread_mutex_init(&airport.specialKiosk.mutex,NULL);
    for(int i=0;i<M;i++){
        airport.kiosks[i].currentPassenger=0;
        airport.kiosks[i].maximumPassenger=10000;
        airport.specialKiosk.currentPassenger=0;
        airport.specialKiosk.maximumPassenger=10000;
        pthread_mutex_init(&airport.kiosks[i].mutex,NULL);
        sem_init(&airport.kiosks[i].kioskFull,0,1);
        //printf("p %d\n",airport.kiosks[0].currentPassenger);
        //printf("m %d\n",airport.kiosks[0].maximumPassenger);
        for(int k=0;k<1;k++){
            airport.kiosks[i].passengers[k]=-1;
            airport.specialKiosk.passengers[k]=-1;
            //printf("pd %d\n",k);
            //printf("Kiosk %d belt %d passenger number %d\n",i,j,k);
        }
        //printf("p %d\n",airport.kiosks[0].currentPassenger);
    }
    //printf("p %d\n",airport.kiosks[0].currentPassenger);
    for(int i=0;i<N;i++){
        airport.belts[i].currentPassenger=0;
        airport.belts[i].maximumPassenger=P;
        sem_init(&airport.belts[i].beltFull,0,P);
        pthread_mutex_init(&airport.belts[i].mutex,NULL);
        for(int k=0;k<P;k++){
            airport.belts[i].passengers[k]=-1;
            //printf("Kiosk %d belt %d passenger number %d\n",i,j,k);
        }
    }
    sem_init(&airport.boarding.boarding,0,1);
    airport.boarding.currentPassenger=0;
    pthread_mutex_init(&airport.boarding.mutex,NULL);
    for(int i=0;i<M*N*P;i++){
        airport.boarding.passenger[i]=-1;
        airport.viPchannel.passengerGoing[i]=-1;
        airport.viPchannel.passengerComingBack[i]=-1;
        airport.viPchannel.passengerWaiting[i]=-1;
    }
    pthread_mutex_init(&airport.viPchannel.mutex,NULL);
    airport.viPchannel.going=0;
    airport.viPchannel.waiting=0;
    airport.viPchannel.comingBack=0;
    //printf("p %d\n",airport.kiosks[0].currentPassenger);
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
        sleep(2);
    }
    pthread_exit(NULL);
    return  0;
    //return 0;
}