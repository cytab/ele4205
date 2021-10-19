/**
 * 
 * Inspired by :
 * https://aticleworld.com/socket-programming-in-c-using-tcpip/
 * auhtor :
 * 
 * **/
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

#define PORT_NUMBER 4099
#define ELE4205_OK 0b1
#define ELE4205_QUIT 0b10

int sendImage(int sock, char* message){
    // Send some data
    if( send(sock, message, strlen(message), 0) < 0)
    {
        printf("Send failed");
        return -1;
    }
    return 1;
}

short SocketCreate(void)
{
    short hSocket;
    printf("Create the socket\n");
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    return hSocket;
}



int BindCreatedSocket(int hSocket, int p)
{
    int iRetval=-1;
    int ClientPort = p;
    struct sockaddr_in  remote= {0};
    /* Internet address family */
    remote.sin_family = AF_INET;
    /* Any incoming interface */
    remote.sin_addr.s_addr = htonl(INADDR_ANY);
    remote.sin_port = htons(ClientPort); /* Local port */
    iRetval = bind(hSocket,(struct sockaddr *)&remote,sizeof(remote));
    return iRetval;
}
int main(int argc, char *argv[])
{
    // 
    int socket_desc, sock, clientLen ; 
    struct sockaddr_in server, client;

    uint32_t client_message = 0;
    // char test -> apres image
    char message[100] = {0};

    //Create socket
    socket_desc = SocketCreate();
    if (socket_desc == -1)
    {
        printf("Could not create socket");
        return 1;
    }

    //Bind
    if( BindCreatedSocket(socket_desc, PORT_NUMBER) < 0)
    {
        //print the error message
        perror("bind failed.");
        return 1;
    }

    char* client_m = (char*)&client_message; 
    //Listen
    listen(socket_desc, 3);
    //Accept and incoming connection
    int onetime = 0;
    while(1)
    {
        memcpy(message, "test connect",13 );

        clientLen = sizeof(struct sockaddr_in);
        //accept connection from an incoming client
        sock = accept(socket_desc,(struct sockaddr *)&client,(socklen_t*)&clientLen);
        if (sock < 0)
        {
            perror("accept failed");
            return 1;
        }else{ 
            if(sendImage(sock, message) == -1 && onetime != 0){
                close(sock);
                return -1;
            }
            onetime = 1 ;
        }

        memcpy(message, "test avec feedback", 19);

        //Receive a reply from the client
        if( recv(sock, client_m, sizeof(uint32_t), 0) < 0)
        {
            printf("recv failed");
            break;
        }

        if(ELE4205_OK == client_message)
        {
            if(sendImage(sock, message) == -1){
                close(sock);
                return -1;
            }
        }
        else if(ELE4205_QUIT == client_message)
        {
            close(sock);
            return 0;
        }

        close(sock);
        sleep(30);
    }
    return 0;
}