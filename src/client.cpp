// Inspiré par
// https://aticleworld.com/socket-programming-in-c-using-tcpip/

#include "transfer.hpp"

int SocketConnect(int hSocket)
{
	int iRetval=-1;
	struct sockaddr_in remote= {0};
	remote.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
	remote.sin_family = AF_INET;
	remote.sin_port = htons(PORT_NUMBER);
	iRetval = connect(hSocket,(struct sockaddr *)&remote,
		sizeof(struct sockaddr_in));
	return iRetval;
}

int main(int argc, char *argv[])
{
	int hSocket, read_size;
	struct sockaddr_in server;
	// Donnees a envoyer vers le serveurs
	uint32_t message;
	char* message_data = (char*)&message;

	//Create socket
	hSocket = SocketCreate();

	if(hSocket == -1) {
		log_info("Could not create socket");
		return 1;
	}

	log_info("Socket is created");
	//Connect to remote server
	if (SocketConnect(hSocket) < 0) {
		log_info("connect failed.\n");
		return 1;
	}
	log_info("Sucessfully conected with server");
	//receive first header of mat object 
	char *entete ;
	read_size = recv(hSocket, entete, sizeof(cv::Mat),0);
	cv::Mat* head = reinterpret_cast<cv::Mat*>(entete); 

	log_info("Start message sending");

	int imgSize = head->rows * head->cols * CV_ELEM_SIZE(head->flags);
	uchar* sockData = new uchar[imgSize];
	log_info("Start message sending1");
	while(1) {
		int bytes = 0;
		// receive first frame and additionnal frame
		for (int i = 0; i < imgSize; i += bytes) {
			bytes = recv(hSocket, sockData+i, imgSize-i, 0);
			if (bytes == -1){
				log_info("reception error");	
			}
		};
		cv::Mat frame(head->rows,head->cols, head->type(), sockData);
		log_info("afficher image");
		cv::imshow(FRAME_WINDOW_NAME, frame);
		log_info("afficher image1");
		int key = cv::waitKey(30)&0xFF;
		if(key == ESC){
			log_info("escape");
			message = ELE4205_QUIT; 
			send(hSocket, message_data, sizeof(uint32_t), 0);
			close(hSocket);
			return 0;
		}else{
			log_info("send OK");
			message = ELE4205_OK; 
			send(hSocket, message_data, sizeof(uint32_t), 0);
			log_info("send OK1");
		}
	}
	log_info("Close everything");
	delete sockData ; 
	close(hSocket);
	return 0;
}

