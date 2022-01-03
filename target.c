//
// Created by aniki on 1/1/2022.
//
///should be named testing.c

#include <stdio.h>
#include "math.h"
#include "time.h"
#include "unistd.h"
int factorial(int n)
{
    int s=1;
    for(int i=n;i>0;i--){
        s=s*i;
    }
    return s;
}
double poissonDist(int lambda,int x)
{
    //printf("%f  ",(double )lambda);
    double a= exp(-1.0*(double )lambda);
    //printf("%f   ",a);
    a=a*pow((double )lambda,(double )x);
    a=a/(double ) factorial(x);
    int ans=(int )a;
    return a;
}
double TestFunction(int max, int numIntervals,int x) {

    //int intervals[numIntervals];

    double exponent = 2;

    double n = pow(exponent, numIntervals) - 1;
    double d = exponent - 1;

    double t = max / (n / d);


    double interval = t * pow(exponent, x);
    return interval;

}
int main(){
    int lambda,x=1;
    lambda=10;
    int maxTime=150000;
    int timePassed=0;
    for(int i=0;i<100;i++){
        if(x>lambda) {
            x = 1;
            timePassed+=maxTime-timePassed;
        }
        int s=(int)(round(poissonDist(lambda,x)*1000000));
        timePassed+=s/10;
        printf("%d %d\n",timePassed,s);
        sleep((int)s/100000);
        x++;
    }

}