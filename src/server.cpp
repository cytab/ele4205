/**
 * 
 * Inspired by :
 * https://aticleworld.com/socket-programming-in-c-using-tcpip/
 * auhtor :
 * 
 * **/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>

#define PORT_NUMBER 4099
#define ELE4205_OK 0b1
#define ELE4205_QUIT 0b10

int sendImage(cv::VideoCapture capture,cv::Mat frame, cv::Mat flippedFrame , int sock, int bytes, int imageSize){
    // Send some dataà
    capture >> frame;
    // flip the frame
    flip(frame, flippedFrame, 1);
    if(!frame.empty()){
        if((bytes = send(sock,flippedFrame.data,imageSize,0))<0){
            std::cout << "Error while sending..";
        }
   }
}

short SocketCreate(void)
{
    int hSocket;
    printf("Create the socket\n");
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    return hSocket;
}



int BindCreatedSocket(int hSocket, int p)
{
    int iRetval=-1;
    int ClientPort = p;
    struct sockaddr_in  remote;
     memset(&remote, 0, sizeof(remote));       // Zero out structure
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
    cv::VideoCapture capture(0);
	//capture.set(CV_CAP_PROP_FRAME_WIDTH, res.w);
	//capture.set(CV_CAP_PROP_FRAME_HEIGHT, res.h);
	if(!capture.isOpened()){
			std:: cout << "Failed to  connect to the camera" ;
	}

    cv::Mat frame, flippedFrame;
    frame = cv::Mat::zeros(480,640,CV_8UC3);
    if (!frame.isContinuous()){
        frame = frame.clone();
    }

    int imageSize = frame.total()*frame.elemSize();
    int bytes = 0;
    std::cout  << "here ";
    capture.set(CV_CAP_PROP_FORMAT,CV_8UC3);
    //Accept and incoming connection
    int onetime = 0;
    listen(socket_desc, 5);

    for (;;)
    {
                //make img continuos
        if ( ! frame.isContinuous() ) { 
            frame = frame.clone();
        }

        memcpy(message, "test connect",13 );

        clientLen = sizeof(struct sockaddr_in);
        //accept connection from an incoming client

        sock = accept(socket_desc,(struct sockaddr *)&client,(socklen_t*)&clientLen);
        if (sock < 0)
        {
            perror("accept failed");
            return 1;
        }else{ 
            if(sendImage(capture, frame,flippedFrame, sock, bytes, imageSize) == -1 && onetime != 0){
                close(sock);
                return -1;
            }
            onetime = 1 ;
        }

        memcpy(message, "test avec feedback", 19);

        //Receive a reply from the client
        if( ssize_t numBytesRcvd = recv(sock, client_m, sizeof(uint32_t), 0) < 0)
        {
            printf("recv failed");
            break;
        }

        if(ELE4205_OK == client_message)
        {
            if(sendImage(capture, frame, flippedFrame, sock, bytes, imageSize) == -1){
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
