// Inspiré par
// https://aticleworld.com/socket-programming-in-c-using-tcpip/

#include "capture.hpp"
#include "transfer.hpp"

int currentResolutionIndex = INITIAL_RES_INDEX;
uint32_t resMask = 0;

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

void mouseCallBack(int event, int x, int y, int flags, void* userdata)
{
	int lowestY = FIRST_BUTTON_Y - TEXT_HEIGHT;
	int index = -1;
	if  (event == cv::EVENT_LBUTTONDOWN ) {
		for (int i = 0; i < sizeof(CAMERA_RESOLUTIONS); i++) {
			if (y > lowestY
					&& y < lowestY + TEXT_HEIGHT
					&& x > BUTTON_X
					&& x < BUTTON_X + TEXT_WIDTH) {
				index = i;
				break;
			}
			lowestY += FIRST_BUTTON_Y;
		}
	}
	if (index >= 0 && currentResolutionIndex != index) {
		resMask = getResMask(index);
		currentResolutionIndex = index;
	}
	else {
		resMask = 0;
	}
}

void initializeMenu(cv::Mat &menuImage)
{
	cv::putText(menuImage,
		"Cliquez sur un format.",
		cv::Point(10, 40),cv::FONT_HERSHEY_DUPLEX,
		1, cv::Scalar(0,0,255), 2, false);
	int yOffset = FIRST_BUTTON_Y;
	int xOffset = BUTTON_X;
	for (auto res : CAMERA_RESOLUTIONS) {
		cv::rectangle(menuImage,
			cv::Point(xOffset - 10, yOffset - TEXT_HEIGHT),
			cv::Point(xOffset + TEXT_WIDTH, yOffset + 10),
			cv::Scalar(255, 0, 0), 4);
		cv::putText(menuImage,
			std::to_string(res.w) + " X " + std::to_string(res.h),
			cv::Point(xOffset, yOffset),cv::FONT_HERSHEY_DUPLEX,
			1, cv::Scalar(0,255,0), 2, false);
		yOffset += FIRST_BUTTON_Y;
	}
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

	cv::Mat menu(MENU_W, MENU_H, CV_64FC4);
	initializeMenu(menu);
	cv::imshow(MENU_WINDOW_NAME, menu);
	cv::setMouseCallback(MENU_WINDOW_NAME, mouseCallBack, NULL);

	while(1) {
		//receive first header of mat object 
		char *entete ;
		read_size = recv(hSocket, entete, sizeof(cv::Mat),0);
		cv::Mat* head = reinterpret_cast<cv::Mat*>(entete); 

		log_info("Start message sending");

		int imgSize = head->rows * head->cols * CV_ELEM_SIZE(head->flags);
		uchar* sockData = new uchar[imgSize];
		log_info("Start message sending1");


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
			message = ELE4205_OK | resMask;
			//std::cout << "envoi: " << (int)resMask << std::endl; 
			send(hSocket, message_data, sizeof(uint32_t), 0);
			log_info("send OK1");
		}
		delete sockData;
	}
	log_info("Close everything");
	close(hSocket);
	return 0;
}

