#include "client.hpp"

int currentResolutionIndex = INITIAL_RES_INDEX;
uint32_t resMask = 0;

int imageID = 0;

int SocketConnect(int hSocket, int port)
{
	// Inspiré par
	// https://aticleworld.com/socket-programming-in-c-using-tcpip/
	int iRetval=-1;
	struct sockaddr_in remote= {0};
	remote.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
	remote.sin_family = AF_INET;
	remote.sin_port = htons(port);
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
			if (y > (lowestY - GUI_BUTTON_PADDING)
			    && y < (lowestY + TEXT_HEIGHT + GUI_BUTTON_PADDING)
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
		"Gardez le bouton de la souris enfonce sur un format.",
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

void readAndSendMusic(int imageID, cv::Mat* frame)
{
	std::string imgName = ("img_" + std::to_string(imageID) + ".png");
	cv::Mat grey;
	cv::cvtColor(*frame, grey, CV_BGR2GRAY);
	cv::threshold(grey, grey, 110, 220, cv::THRESH_BINARY);
	cv::imwrite(imgName, grey);

	// Lecture des caractères. Inspiré de :
	// (https://tesseract-ocr.github.io/tessdoc/APIExample.html
	tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
	if (ocr->Init(NULL, "eng")) {
		log_info("Could not initialize tesseract.");
		_exit(0);
	}
	ocr->SetVariable("tessedit_char_whitelist",
		"0123456789abcdefgABCDEFGR#");
	Pix *textImage = pixRead(imgName.c_str());
	ocr->SetImage(textImage);
	std::string text = ocr->GetUTF8Text();

	usleep(2000000);
	int musicSocket = SocketCreate();
	if (SocketConnect(musicSocket, MUSIC_PORT_NUMBER) < 0) {
		log_info("connect failed.\n");
		_exit(0);
	}
	log_info("Sucessfully conected with server (music)");
	uint32_t length = text.size();
	write(musicSocket, &length, sizeof(uint32_t));
	usleep(2000);
	write(musicSocket, text.data(), text.size());
	usleep(200000);
	close(musicSocket);

	ocr->End();
	delete ocr;
}

int main(int argc, char *argv[])
{
	bool canSendText = true;
	int hSocket, read_size;
	struct sockaddr_in server;
	// Donnees a envoyer vers le serveurs
	uint32_t message;
	char* message_data = (char*)&message;

	uint32_t server_message = 0;

	//Create socket
	hSocket = SocketCreate();

	if(hSocket == -1) {
		log_info("Could not create socket");
		return 1;
	}

	log_info("Socket is created");
	// Connect to remote server
	if (SocketConnect(hSocket) < 0) {
		log_info("connect failed.\n");
		return 1;
	}
	log_info("Sucessfully conected with server");
	
	cv::Mat menu(MENU_H, MENU_W, CV_64FC4);
	initializeMenu(menu);
	cv::imshow(MENU_WINDOW_NAME, menu);
	cv::setMouseCallback(MENU_WINDOW_NAME, mouseCallBack, NULL);

	cv::Mat blankFrame(BLANK_FRAME_H, BLANK_FRAME_W, CV_64FC4);
	cv::Mat noImageFrame(BLANK_FRAME_H, BLANK_FRAME_W, CV_64FC4);
	cv::putText(noImageFrame,
		BLANK_FRAME_NAME,
		cv::Point(10, 40),cv::FONT_HERSHEY_DUPLEX,
		1, cv::Scalar(0,0,255), 2, false);

	for (;;) {
		// Message 1 (serveur -> client) : recevoir l'état.
		if (recv(hSocket, &server_message, sizeof(uint32_t), 0) == -1){
			log_info("reception error");
		}
		log_info("Received server state");

		// Message 2 (client -> serveur) : envoyer la résolution.
		bool mustQuit = false;
		int key = cv::waitKey(ESC_WAIT_TIME_MICRO_S)&0xFF;
		message = ELE4205_OK;
		if(key == ESC){
			message = ELE4205_QUIT;
			mustQuit = true;
			log_info("escape");
		}
		message |= resMask;
		send(hSocket, message_data, sizeof(uint32_t), 0);
		if (mustQuit){
			close(hSocket);
			return 0;
		}

		if(server_message == STATE_READY || server_message == STATE_PUSHB){
			//Message 3 : receive first header of mat object 
			char *entete;
			log_info("about to receive header");
			read_size = recv(hSocket, entete, sizeof(cv::Mat),0);
			log_info("received header");
			cv::Mat* head = reinterpret_cast<cv::Mat*>(entete); 

			log_info("Start message sending");

			int imgSize = head->rows * head->cols * CV_ELEM_SIZE(head->flags);
			uchar* sockData = new uchar[imgSize];

			int bytes = 0;
			// receive first frame and additionnal frame
			for (int i = 0; i < imgSize; i += bytes) {
				bytes = recv(hSocket, sockData+i, imgSize-i, 0);
				if (bytes == -1){
					log_info("reception error");	
				}
			}
			cv::Mat frame(head->rows,head->cols, head->type(), sockData);

			// Diviser le programme en processus enfant et parent.
			if (server_message == STATE_PUSHB && canSendText){
				canSendText = false;
				imageID++;
				pid_t pid = fork();
				if (pid == 0) {
					readAndSendMusic(imageID, &frame);
					_exit(0);
				}
			}

			cv::imshow(FRAME_WINDOW_NAME, frame);
			
			delete sockData;
		} else if (server_message == STATE_IDOWN) {
			log_info("Ne rien afficher.");
			cv::imshow(FRAME_WINDOW_NAME, blankFrame);
			cv::imshow(FRAME_WINDOW_NAME, noImageFrame);
		}
		if(server_message != STATE_PUSHB){
			canSendText = true;
		}
	}
		
	log_info("Close everything");
	close(hSocket);
	return 0;
}

