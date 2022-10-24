#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "DataStructs.h"

int port;
char *ipAdd;
/*Function to allocate dynamic memory*/
char* allocateSpace(){
    char *str;
    str=(char*)malloc(500*sizeof(char));
    return str;
}
/*Send Command implementation client side*/
void sendClient(int network_socket){
            struct Send *args=malloc(sizeof(struct Send));
            printf("Sender: ");
            scanf("%s",args->sender);
            printf("Receiver: ");
            scanf("%s",args->reciever);
            getchar();
            printf("Subject: ");
            fgets(args->subject, sizeof(args->subject), stdin);
            printf("Message: ");
            fgets(args->message, sizeof(args->message), stdin);
            send(network_socket, args,sizeof(*args), 0);
            char *command=allocateSpace();
            scanf("%s",command); // dot to end commmand
            if(strlen(args->sender)<=8 && strlen(args->reciever)<=8){
             printf("\n<OK>\n");
            }
            else{
             printf("\n<ERR(username>8)>\n");
            }
            free(args);
            free(command);
}
/*List command Handling client side*/
void ListClient(int network_socket){
            char *username=allocateSpace();
            scanf("%s",username);
            send(network_socket,username,sizeof(username),0);
            int lin=0;
            recv(network_socket,&lin,sizeof(int),0);
            printf("\n<%d>\n",lin);
            char *subj=allocateSpace();

            while(lin!=0)
            {
                recv(network_socket,subj,100,0);
                printf("%s\n",subj);
                lin--;
            }
            free(username);
            free(subj);
}
/*Read Command Handling Client Side*/
void ReadClient(int network_socket){
    char *username=allocateSpace();
    scanf("%s",username);
    send(network_socket,username,sizeof(username),0);
    int msgNum=0;
    scanf("%d",&msgNum);
    send(network_socket,&msgNum,sizeof(int),0);
    int okFlag=-1;
    recv(network_socket,&okFlag,sizeof(int),0);
    if(okFlag==-1){
        printf("ERR\n");
    }
    else{
      char *reply=allocateSpace();
      recv(network_socket,reply,100,0);
      printf("\n<OK>\n");
      printf("%s\n",reply);
    }
    free(username);
}
/*DEl command handling client Side*/
void DelClient(int network_socket){
     char *username=allocateSpace();
    scanf("%s",username);
    send(network_socket,username,sizeof(username),0);
    int msgNum=0;
    scanf("%d",&msgNum);
    send(network_socket,&msgNum,sizeof(int),0);
    int res=0;
    recv(network_socket,&res,sizeof(int),0);
    if(res>0){
        printf("\n<OK>\n");
    }
    else
    {

        printf("\n<ERR>\n");
    }
}
/*Client Side Driver Helper Function*/
void clienthread(int client_r)
{
    /* Initializing Socket */
    int client_request = client_r;
    int network_socket;
    char *ip="127.0.0.1";
    /* Creating  Socket */
    network_socket = socket(AF_INET,SOCK_STREAM, 0);

    /* Initializing Socket Family and port for connection */
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip);
    server_address.sin_port = htons(port);  //port for socket connection


    int connection_status = connect(network_socket,
                                    (struct sockaddr*)&server_address,
                                    sizeof(server_address));


    if (connection_status < 0) {
        puts("Error\n");
        return;
    }
    /* Connection Established Here */
    printf("Connection established\n");

    int choice=1;


    char *reply;
    char *command;
    send(network_socket,&choice,sizeof(choice),0); //sending user choice to server

    reply=allocateSpace();
    recv(network_socket,reply,sizeof(reply),0);
    printf("Server: %s\n",reply);
    free(reply);


    command=allocateSpace();
    while(strcmp(command,"QUIT")!=0){
    system("clear");
    printf("\n\n------\n");
    printf("Send\n");
    printf("List\n");
    printf("Read\n");
    printf("Del\n");
    printf("Quit\n");
    printf("------\n");
        printf("\n---Mail Server---\n");
        scanf("%s",command);  //command of user

        if(strcmp(command,"SEND")==0)  //Send Command
        {
            send(network_socket,command,sizeof(command),0);
            sendClient(network_socket);

        }
        else if(strcmp(command,"LIST")==0){  //List Commmand
              send(network_socket,command,sizeof(command),0);
              ListClient(network_socket);
        }
        else if(strcmp(command,"READ")==0){  //Read Command
            send(network_socket,command,sizeof(command),0);
              ReadClient(network_socket);
        }
        else if(strcmp(command,"QUIT")==0) //Quit Command
        {
            send(network_socket,command,sizeof(command),0);
        }
        else if(strcmp(command,"DEL")==0){ //DEL command
            send(network_socket,command,sizeof(command),0);
            DelClient(network_socket);
        }
          printf("\n---Press any key to return back to Home---\n%c",getchar());
             getchar();

    }

    /*Closing Connection*/
    free(command);
    close(network_socket);
    pthread_exit(NULL);
}


/*
Driver Code
*/
int main(int argc,char *argv[])
{
     if(argc<3){
        printf("Provide Valid Command Line Arguments(./client Port ip )\n");
        return -1;
    }
    int client_request = 1;
    port=atoi(argv[1]);
    ipAdd=allocateSpace();
    strcpy(ipAdd,argv[2]);
    clienthread(client_request);
    free(ipAdd);
}
