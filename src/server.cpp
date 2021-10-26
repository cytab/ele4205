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
#include <chrono>
#include <thread>

#include <sys/types.h>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>

#define PORT_NUMBER 4099
#define ELE4205_OK 0b1
#define ELE4205_QUIT 0b10


int sendEntete(cv::Mat frame,int sock){	
    int bytes ;
    cv::Mat* enteteframe = new cv::Mat(frame) ; 
    char* entete = reinterpret_cast<char *>(enteteframe);
    if(( bytes = send(sock,entete,sizeof(cv::Mat),0)) < 0){
            std::cout << "Error while sending..";
	    return -1;
    }
    delete enteteframe ;
    return 0;    
}
 

int sendImage(cv::VideoCapture capture,cv::Mat frame, int sock, int bytes, int flag){
    capture >> frame;
    //std::cout << "Error while sending..";
    int imageSize = frame.total()*frame.elemSize();
    // Send some data
    if (flag == 0){
	sendEntete(frame, sock);

     }	
    
    // send datarecv
    std::cout <<"frame process\n" << std::endl;
    if(!frame.empty()){
        if((bytes = write(sock,frame.data,imageSize))<0){
            std::cout << "Error while sending..";
	    return -1;
        }
   }
   std::cout <<"frame process2\n" << std::endl;
   return 0;
	
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
     
    cv::VideoCapture capture(0);
	//capture.set(CV_CAP_PROP_FRAME_WIDTH, res.w);
	//capture.set(CV_CAP_PROP_FRAME_HEIGHT, res.h);
	if(!capture.isOpened()){
		std:: cout << "Failed to  connect to the camera" ;
	}

    cv::Mat frame;

    int bytes = 0;
    printf("Capture video\n");
    capture.set(CV_CAP_PROP_FORMAT,CV_8UC3);
    //Accept and incoming connection
    int onetime = 0;
    listen(socket_desc, 5);

	std::cout << "Loop start." << std::endl;
        clientLen = sizeof(struct sockaddr_in);
        //accept connection from an incoming client
        std::cout <<"about to acccept\n" << std::endl;
        sock = accept(socket_desc,(struct sockaddr *)&client,(socklen_t*)&clientLen);
        if (sock < 0)
        {
            perror("accept failed");
            return 1;
        }else if (onetime == 0) { 
 	    std::cout <<"accept connection\n" << std::endl;
            if(sendImage(capture, frame, sock, bytes, 0) == -1){
                close(sock);
                return -1;
            }
            onetime = 1 ;
        }

    for (;;)
    {
	
        std::cout <<"communication\n" << std::endl;
        //Receive a reply from the client
	int numBytesRcvd = 0;
        // receive first frame and additionnal frame
	for (int i = 0; i < sizeof(uint32_t); i += numBytesRcvd) {
    		if ((numBytesRcvd = recv(sock, &client_message, sizeof(uint32_t), 0)) == -1){
			printf("reception error\n");	
		}
       			
	};
        std::cout <<"communication1\n" << std::endl;
        if(ELE4205_OK == client_message)
        {
            std::cout <<"ok recu\n" << std::endl;
		int result = sendImage(capture, frame, sock, bytes, 1);
		std::cout << "Resultat : " << result << std::endl;
            if(result == -1){
                close(sock);
                return -1;
		std::cout <<"bizarre\n" << std::endl;
            }
        }
        else if(ELE4205_QUIT == client_message)
        {
            close(sock);
            return 0;
        }
	std::cout << "Loop end." << std::endl;
        //close(sock);
	//sleep(30);
	std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    return 0;
}
