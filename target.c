//
// Created by aniki on 1/1/2022.
//

#include <stdio.h>
int main(){
    FILE *fp;
    fp = fopen("D:/L3T2/Sessional/314 OS/Offline2/output.txt", "w");//opening file
    fprintf(fp, "Hello file by fprintf...%d\n",(a+1));//writing data into file
    fclose(fp);//closing file
}