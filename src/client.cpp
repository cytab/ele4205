/**
 * 
 * Inspiré par
 * https://aticleworld.com/socket-programming-in-c-using-tcpip/
 * 
 * **/

#include "transfer.hpp"

//try to connect with server
int SocketConnect(int hSocket)
{
    int iRetval=-1;
    struct sockaddr_in remote= {0};
    remote.sin_addr.s_addr = inet_addr("192.168.7.2"); //Local Host
    //remote.sin_addr.s_addr = inet_addr("127.0.0.1");
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
    read_size = recv(hSocket, entete, sizeof(cv::Mat),0);
    cv::Mat* head = reinterpret_cast<cv::Mat*>(entete); 
    

    printf("Start messgae sending\n");

    std::cout << head->rows << head->cols << std::endl;
    int imgSize = head->rows * head->cols * CV_ELEM_SIZE(head->flags);
    uchar* sockData = new uchar[imgSize];
    std::cout << std::hex << &sockData[0] << std::endl;
    printf("Start messgae sending1\n");
    while(1){
	int bytes = 0;
        // receive first frame and additionnal frame
	for (int i = 0; i < imgSize; i += bytes) {
    		if ((bytes = recv(hSocket, sockData+i, imgSize-i, 0)) == -1){
			printf("reception error\n");	
		}
       			
	};
	cv::Mat frame(head->rows,head->cols, head->type(), sockData);
	//cout << &frame.data << std::endl;
        cv::namedWindow("Show camera"); // Create a window
	printf("afficher image\n");
        cv::imshow("FRAME", frame);
        printf("afficher image1\n");
        int key = cv::waitKey(30)&0xFF;
        if(key == ESC){
	    printf("escape\n");
            message = ELE4205_QUIT; 
            send(hSocket, message_data, sizeof(uint32_t), 0);
            close(hSocket);
            return 0;
        }else{
            printf("send OK\n");
            message = ELE4205_OK; 
            send(hSocket, message_data, sizeof(uint32_t), 0);
	    printf("send OK1\n");
        }
    }
	printf("Close everything\n");
    delete sockData ; 
    close(hSocket);
    return 0;
}
