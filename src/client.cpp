/**
 * 
 * Inspired by :
 * https://aticleworld.com/socket-programming-in-c-using-tcpip/
 * auhtor :
 * 
 * **/

//#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>

#define PORT_NUMBER 4099
#define ELE4205_OK 0b1
#define ELE4205_QUIT 0b10
#define ESC 27

//Create a Socket for server communication
short SocketCreate(void)
{
    short hSocket;
    printf("Create the socket\n");
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    return hSocket;
}
//try to connect with server
int SocketConnect(int hSocket)
{
    int iRetval=-1;
    struct sockaddr_in remote= {0};
    //remote.sin_addr.s_addr = inet_addr("192.168.7.2"); //Local Host
    remote.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote.sin_family = AF_INET;
    remote.sin_port = htons(PORT_NUMBER);
    iRetval = connect(hSocket,(struct sockaddr *)&remote,sizeof(struct sockaddr_in));
    return iRetval;
}

//main driver program
int main(int argc, char *argv[])
{
    int hSocket, read_size;
    struct sockaddr_in server;
    // Donnees a envoyer vers le serveurs
    uint32_t message;
    char* message_data = (char*)&message;

    //Create socket
    hSocket = SocketCreate();

    if(hSocket == -1)
    {
        printf("Could not create socket\n");
        return 1;
    }
    
    printf("Socket is created\n");
    //Connect to remote server
    if (SocketConnect(hSocket) < 0)
    {
        perror("connect failed.\n");
        return 1;
    }
    printf("Sucessfully conected with server\n");
    //receive first header of mat object 
	char *entete ;
    read_size = recv(hSocket, entete, sizeof(*entete), MSG_WAITALL);
	cv::Mat* frame = reinterpret_cast<cv::Mat*>(entete);

    uchar *iptr = frame->data;
    int imgSize = frame->total() * frame->elemSize();
        //make img continuos
    if ( ! frame->isContinuous() ) { 
         *frame = frame->clone();
    }
    while(1){
	
        // receive first frame and additionnal frame 
        read_size = recv(hSocket, iptr, imgSize, MSG_WAITALL);
        cv::namedWindow("Show camera"); // Create a window

        imshow("FRAME", *frame); 
        int bytes = 0;

        int key = cv::waitKey(30);
        if(key == ESC){
	    printf("escape\n");
            message = ELE4205_QUIT; 
            send(hSocket, message_data, sizeof(message), 0);
            close(hSocket);
            return 0;
        }else{
		    printf("send OK\n");
            message = ELE4205_OK; 
            send(hSocket, message_data, sizeof(message), 0);
        }
    }
	printf("Close everything\n");
    close(hSocket);
    return 0;
}
