//
// Created by aniki on 1/1/2022.
//

#include <stdio.h>
int main(){
    FILE *fp;
    fp = fopen("", "w");//opening file
    fprintf(fp, "Hello file by fprintf...\n");//writing data into file
    fclose(fp);//closing file
}