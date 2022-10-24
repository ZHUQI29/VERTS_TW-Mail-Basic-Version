#include <netinet/in.h> //INETADDSTRLEN
#pragma once

#define messageLength 500
#define userData 12
#define sendLength 100
#define subjectLength 80

struct Send{
    char sender[userData];
    char reciever[userData];
    char subject[subjectLength];
    char message[messageLength];
};


struct Message{
    char subject[subjectLength];
    char message[messageLength];
};
char* allocateSpaceSize(int size){
    char *str;
    str=(char*)malloc(size*sizeof(char));
    return str;
}

