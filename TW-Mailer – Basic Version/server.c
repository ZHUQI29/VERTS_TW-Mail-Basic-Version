#include <stdio.h>
#include <unistd.h>
#include<string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include "DataStructs.h"

/*Server Variables*/
pthread_t tid;
#define numberW 100
pthread_t readerthreads[numberW];

int port;
char *dir;
/*Function to allocate space*/
char* allocateSpace(){
    char *str;
    str=(char*)malloc(500*sizeof(char));
    return str;
}

/*Function to create User File*/
void createUserSpace(char *name)
{
    FILE *fp=fopen(name,"w");
    fclose(fp);
}
/*Function to handle send command*/
void sendCommand(int newSocket)
{
    struct Send *send=malloc(sizeof(struct Send));
    recv(newSocket,send,sizeof(*send),0);
    printf("Sender: %s\n",send->sender);
    printf("Receiver: %s\n",send->reciever);
    printf("Subject: %s\n",send->subject);
    printf("Message: %s\n",send->message);
    if(strlen(send->reciever)<=8 && strlen(send->sender)<=8){
    strcat(send->reciever,".txt");
      FILE *re=fopen(send->reciever,"a");
      if(re!=NULL){
      fputs(send->subject,re);
      fputs(send->message,re);
  
      fclose(re);
      }
      }
      free(send);
}
/*Function to count Number of lines in a File*/
int countLines(char *filename){

   FILE *fp;
   char ch;
   int linesCount=0;
   fp=fopen(filename,"r");
   if(fp==NULL) {
      return -1;
   }
   while((ch=fgetc(fp))!=EOF) {
      if(ch=='\n')
         linesCount++;
   }

   fclose(fp);
   return (linesCount/2);
}
/*Server Implementation of LIST command*/
void ListServer(int newSocket)
{
    char *user=allocateSpace();
    recv(newSocket,user,sizeof(user),0);
    strcat(user,".txt");
    printf("%s\n",user);
    char *mess=allocateSpace();
    char *subj=allocateSpace();
    int lines=countLines(user);
    send(newSocket,&lines,sizeof(int),0); //sending number of messages
    FILE *fp=fopen(user,"r");
    int i=0;
    int j=0;
    while(lines!=0)
    {
     fgets(subj,100,fp);
     fgets(mess,100,fp);
     printf("Subject: %s\n",subj);
     send(newSocket,subj,100,0);
     lines--;
    }
    free(subj);
    free(mess);
    free(user);
}
/*Del Command implementation server side*/
void DelServer(int newSocket){
     char *user=allocateSpace();
     recv(newSocket,user,sizeof(user),0);
     int msgNumber=0;
     recv(newSocket,&msgNumber,sizeof(int),0);
     strcat(user,".txt");
     printf("%s\n",user);
     int res=-1;
     FILE *fp=fopen(user,"r");
     if(fp==NULL){
        send(newSocket,&res,sizeof(int),0);
     }
     else{
         int lines=0;
         lines=countLines(user);
         struct Message *objMsg=(struct Message*)malloc(lines*sizeof(struct Message));
         if(lines<msgNumber){
            send(newSocket,&res,sizeof(int),0);
         }
         else{
            send(newSocket,&lines,sizeof(int),0);
         }
        char *subj=allocateSpace();
        char *mess=allocateSpace();
        int i=0;
        while(lines!=0){
            subj=allocateSpace();
            mess=allocateSpace();
            fgets(subj,100,fp);
            fgets(mess,100,fp);
            strcpy(objMsg[i].subject,subj);
            strcpy(objMsg[i].message,mess);
            i++;
            lines--;
            free(mess);
            free(subj);
        }
        fclose(fp);
        /*Updating User db*/
        int j=0;
        fp=fopen(user,"w");
        while(j<i){
            if(j!=msgNumber-1){
                fputs(objMsg[j].subject,fp);
                 fputs(objMsg[j].message,fp);
            }
            j++;
        }
        fclose(fp);
        free(user);
        free(objMsg);

    }
}
/*Read Command Server implementation*/
void ReadServer(int newSocket){
     char *user=allocateSpace();
     recv(newSocket,user,sizeof(user),0);
     int msgNumber=0;
     recv(newSocket,&msgNumber,sizeof(int),0);
     strcat(user,".txt");
     printf("%s\n",user);
     int res=-1;
     FILE *fp=fopen(user,"r");
     if(fp==NULL){
        send(newSocket,&res,sizeof(int),0);
     }
     else{
         int lines=0;
         lines=countLines(user);
         if(lines<msgNumber){
            send(newSocket,&res,sizeof(int),0);
         }
         else{
            send(newSocket,&lines,sizeof(int),0);
         }
        char *subj=allocateSpace();
        char *mess=allocateSpace();
        while(lines!=0){
            fgets(subj,100,fp);
            fgets(mess,100,fp);
            if(lines==msgNumber){
                 send(newSocket,mess,100,0);
                 break;
            }
            lines--;
        }
        fclose(fp);
        free(subj);
        free(mess);
        free(user);

     }
}
/*
Server Side Thread to handle each client on server
*/
void* reader(void* param)
{

    int newSocket=*((int*)param);
    int choice=0;
    recv(newSocket,&choice, sizeof(choice), 0);
    printf("Client Choice: %d\n",choice);
    char *reply;
    char *command;

    reply=allocateSpace();
    send(newSocket,reply,sizeof(reply),0);
    free(reply);

    command=allocateSpace();
    //Loop to deal with user command choice
    while(strcmp(command,"QUIT")!=0){
        recv(newSocket,command,sizeof(command),0);
        if(strcmp(command,"SEND")==0){
            sendCommand(newSocket);
            printf("Send Command Executed\n");

        }
        else if(strcmp(command,"LIST")==0){
            ListServer(newSocket);
        }
        else if(strcmp(command,"READ")==0){
            ReadServer(newSocket);
        }
        else if(strcmp(command,"DEL")==0){
            DelServer(newSocket);
        }

    }

    pthread_exit(NULL);
}


/*
Driver Code
*/
int main(int argc,char *argv[])
{
    if(argc<3){
        printf("Provide Valid Command Line Arguments (./server port directory)\n");
        return -1;
    }
    port=atoi(argv[1]);
    dir=allocateSpace();
    strcpy(dir,argv[2]);
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    int serverSocket, newSocket;
    char *ip="127.0.0.1";
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);


    bind(serverSocket,
         (struct sockaddr*)&serverAddr,
         sizeof(serverAddr));



    pthread_t tid[60];

    int i = 0;

    while (1) {

        if (listen(serverSocket, 1) >= 0)
            printf("Listening\n");
        else
            printf("Error\n");

        addr_size = sizeof(serverStorage);

        newSocket = accept(serverSocket,(struct sockaddr*)&serverStorage,&addr_size);
        int choice = 0;
        printf("Thread Created\n");
        if (pthread_create(&readerthreads[i], NULL,reader, &newSocket)!= 0)
        {
            printf("Failed to create thread\n");
        }
         i++;
        pthread_detach(readerthreads[i]);

    }
    free(dir);
    return 0;
}
