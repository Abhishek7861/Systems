#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <map> 
#include <sstream>
#include <iterator> 
#include <pthread.h>

#define MAX_INPUT_SIZE 5000
#define MAX_TOKEN_SIZE 1000
#define MAX_NUM_TOKENS 1000
#define THREAD 10000

char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for(i =0; i < strlen(line); i++){

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
            token[tokenIndex] = '\0';
            if (tokenIndex != 0){
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0;
            }
        } else {
            token[tokenIndex++] = readChar;
        }
    }

    free(token);
    tokens[tokenNo] = NULL ;
    return tokens;
}

char command[10][5000] = {"connect 127.0.0.1 8080\n","read ","update ","delete ","create ","disconnect\n"};
char value[10] = " hello\n";
char length[10] = " 5";



void *load(void * threadid){

    char  line[MAX_INPUT_SIZE];
    char  **tokens;
    int activeConnection=0;
    char choice[1000];
    int switchvalue=0;
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    
    srand(time(NULL));
    while (1) {
        //sleep(1);
        char buffer[5000] = {0};
        char strnum[6];
        bzero(line, MAX_INPUT_SIZE);
        int index = rand()%5;
        int key = rand()%100;
        sprintf(strnum,"%d",key);
        //puts(strnum);
        strcpy(line,command[index]); 
        tokens = tokenize(line);
        strcpy(choice, tokens[0]);
        if (strcmp(choice, "connect") == 0)
            switchvalue = 1;
        if (strcmp(choice, "disconnect") == 0)
            switchvalue = 2;
        if (strcmp(choice, "create") == 0)
            switchvalue = 3;
        if (strcmp(choice, "read") == 0)
            switchvalue = 4;
        if (strcmp(choice, "update") == 0)
            switchvalue = 5;
        if (strcmp(choice, "delete") == 0)
            switchvalue = 6;

        switch (switchvalue) {
            case 1:
            	switchvalue=0;
                if (activeConnection == 0) {
                    //  printf("%s",line);
                    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                    {
                         printf("\n Socket creation error \n");
                        //return -1;
                    }
                    memset(&serv_addr, '0', sizeof(serv_addr));
                    serv_addr.sin_family = AF_INET;
                    int port;
                    sscanf(tokens[2], "%d", &port);
                    serv_addr.sin_port = htons(port);
                    if(inet_pton(AF_INET, tokens[1], &serv_addr.sin_addr)<=0)
                    {
                         printf("\nInvalid address/ Address not supported \n");
                        //return -1;
                    }
                        int a = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
                    if (a < 0)
                    {
                        // printf("Connection Failed \n");
                        //return -1;
                    }
                    //printf("OK fd %d\n",sock);
                    activeConnection = 1;
                    break;
                } else {
                    // printf("You can have only 1 server connected\n");
                    break;
                }
            case 2:
            	switchvalue=0;
                if (activeConnection == 1) {
                    //  printf("%s",line);
                    write(sock,line , strlen(line));
                    valread = read(sock,buffer,5000);
                    close(sock);
                    //  printf("OK\n");
                    //  printf("You can Connect to different server\n");
                    activeConnection=0;
                    break;
                }
                 else {
                    //  printf("You do not have Any Active Connection\n");
                    break;
                }
            case 3:
            	switchvalue=0;
                if (activeConnection == 1) {
                    strcat(line,strnum);
                    strcat(line,length);
                    strcat(line,value);
                    //  printf("%s",line);
                    write(sock, line , strlen(line));
                    valread = read(sock,buffer,5000);
                    //   printf("%s\n",buffer); 
                    break;
                } else {
                    //  printf("You do not have Any Active Connection\n");
                    break;
                }

            case 4:
            	switchvalue=0;
                if (activeConnection == 1) 
                {
                    strcat(line,strnum);
                    strcat(line,"\n");    
                    // printf("%s",line);
                    write(sock, line , strlen(line));
                    valread = read(sock,buffer,5000);
                    // printf("%s\n",buffer);  
                    break;
                } else {
                    //  printf("You do not have Any Active Connection\n");
                    break;
                }
            case 5:
                switchvalue=0;
                if (activeConnection == 1) {
                    strcat(line,strnum);
                    strcat(line,length);
                    strcat(line,value);
                    // printf("%s",line);
                    write(sock, line , strlen(line));
                    valread = read(sock,buffer,5000);
                    // printf("%s\n",buffer);  
                                       
                    break;
                } else {
                //  printf("You do not have Any Active Connection\n");
                    break;
                }
            case 6:
            	switchvalue=0;
                if (activeConnection == 1) {
                    strcat(line,strnum);
                    strcat(line,"\n");
                    // printf("%s",line);
                    write(sock, line , strlen(line));
                    valread = read(sock,buffer,5000);
                    // printf("%s\n",buffer);  
                    break;
                } else {
                    // printf("You do not have Any Active Connection\n");
                    break;
                }
            default:
                //  printf("incorrect command\n");
                break;
        }
   
   int i=0;
        while(tokens[i]!=NULL){
        free(tokens[i]);
        i++;
        }
        if(tokens!=NULL)
        free(tokens);
    }
}



int main(int argc, char *argv[])
{
pthread_t Load[THREAD];
for(int i=0;i<THREAD;i++){
    if(pthread_create(&Load[i], NULL, load, &i)<0)
        printf("not\n");
    pthread_join(Load[i],NULL);
}     
 }