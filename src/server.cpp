// Inspiré par :
// https://aticleworld.com/socket-programming-in-c-using-tcpip/

#include "transfer.hpp"

char resMaks = 0;
char resIndex = INITIAL_RES_INDEX;

int readAdc(){
	char buffer[4] = {0};
	FILE * f = fopen(ADC_FILENAME, READ_FILE_MODE);
	
	fread(buffer, 1, 4, f);
	std::string a (buffer);
	fclose(f);
	return std::stoi(a);
}

int readButton(){
	char buffer[] = "0";
	FILE * f = fopen(GPIO_FILENAME, READ_FILE_MODE);
	
	fread(buffer, 1, 1, f);
	std::string a(buffer);
	fclose(f);
	return std::stoi(a) ; 
}

void setEnvGpio(){

	FILE * f = fopen(EXPORT_FILE, WRITE_FILE_MODE);
	fwrite(GPIO_ID, 1, sizeof(GPIO_ID), f);
	fclose(f);

	f = fopen(GPIO_DIR_DIRECTORY, WRITE_FILE_MODE);
	fwrite(GPIO_DIR, 1, sizeof(GPIO_DIR), f);
	fclose(f);  
}


int sendEntete(cv::Mat frame,int sock){	
	int bytes ;
	cv::Mat* enteteframe = new cv::Mat(frame) ; 
	char* entete = reinterpret_cast<char *>(enteteframe);
	if(( bytes = send(sock,entete,sizeof(cv::Mat),0)) < 0){
		log_info("Error while sending..");
		return -1;
	}
	delete enteteframe ;
	return 0;    
}
 

int sendImage(cv::VideoCapture capture,cv::Mat frame, int sock, int flag){
	capture >> frame;
	int imageSize = frame.total()*frame.elemSize();
	// Send some data
	if (flag == 0){
		sendEntete(frame, sock);
	}
	// send datarecv
	//g_info("frame process");
	if(!frame.empty()){
		if(write(sock,frame.data,imageSize) < 0){
			log_info("Error while sending..");
			return -1;
		}
	}
	//log_info("frame process2");
	return 0;
}

int BindCreatedSocket(int hSocket, int p)
{
	int iRetval=-1;
	int ClientPort = p;
	struct sockaddr_in  remote;
	memset(&remote, 0, sizeof(remote)); // Zero out structure
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
	bool canSendImage = true;

	setEnvGpio();
	uint32_t message;
	char* message_serveur = (char*)&message;

	int socket_desc, sock, clientLen ; 
	struct sockaddr_in server, client;

	uint32_t client_message = 0;

	// Create socket
	socket_desc = SocketCreate();
	if (socket_desc == -1) {
		log_info("Could not create socket");
		return 1;
	}

	// Bind
	if (BindCreatedSocket(socket_desc, PORT_NUMBER) < 0) {
		perror("bind failed.");
		return 1;
	}

	cv::VideoCapture capture(0);
	if(!capture.isOpened()){
		log_info("Failed to  connect to the camera");
	}
	cv::Mat frame;

	int bytes = 0;
	log_info("Capture video");
	capture.set(CV_CAP_PROP_FORMAT,CV_8UC3);
	// Accept and incoming connection
	listen(socket_desc, 5);
	clientLen = sizeof(struct sockaddr_in);
	//accept connection from an incoming client
	log_info("about to acccept");
	sock = accept(socket_desc,(struct sockaddr *)&client,
		(socklen_t*)&clientLen);
	if (sock < 0){
		perror("accept failed");
		return 1;
	} else { 
		log_info("accept connection");
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	capture.set(CV_CAP_PROP_FRAME_WIDTH,
		CAMERA_RESOLUTIONS[INITIAL_RES_INDEX].w);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,
		CAMERA_RESOLUTIONS[INITIAL_RES_INDEX].h);

	for (;;) {
		// Message 1 (serveur -> client) : envoyer l'état.
		if(readAdc() < ADC_THRESHOLD){
			if(readButton() == BUTTON_UP) {
				message = STATE_READY;
				canSendImage = true;
			} else if (canSendImage) {
				message = STATE_PUSHB;
				canSendImage = false;
				pid_t pid = fork();
				if (pid == 0) {
					execl("/home/root/musicPlayer", "/home/root/musicPlayer", (char*) NULL);
					std::cout << "EXEC terminated." << std::endl;
					_exit(0);
				}
			}
		}else{
			message = STATE_IDOWN;
		}
		write(sock, message_serveur, sizeof(uint32_t));
		//log_info("communication");
		// Message 2 (client -> serveur) : recevoir la résolution.
		if (recv(sock, &client_message, sizeof(uint32_t), 0) == -1) {
			log_info("reception error");	
		}
		uint32_t newIndex = getResIndex(client_message);
		if (newIndex<sizeof(CAMERA_RESOLUTIONS) && newIndex!=resIndex){
			resIndex = newIndex;
			capture.set(CV_CAP_PROP_FRAME_WIDTH,
				CAMERA_RESOLUTIONS[resIndex].w);
			capture.set(CV_CAP_PROP_FRAME_HEIGHT,
				CAMERA_RESOLUTIONS[resIndex].h);
		}
		
		//log_info("communication1 ");
		if(((ELE4205_OK & client_message) == ELE4205_OK)
				&& message != STATE_IDOWN) {
			//log_info("ok recu");
			// Message 3 : Envoyer l'image.
			int result = sendImage(capture, frame, sock, 0);
			//log_info(std::string("Resultat : ")
			//	+ std::to_string(result)
			//	+ std::string("\n"));
			if(result == -1){
				close(sock);
				return -1;
			}
		}
		else if ((ELE4205_QUIT & client_message) == ELE4205_QUIT) {
			close(sock);
			return 0;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
	return 0;
}

