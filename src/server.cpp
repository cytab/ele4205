// Inspiré par :
// https://aticleworld.com/socket-programming-in-c-using-tcpip/

#include "transfer.hpp"

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
	log_info("frame process");
	if(!frame.empty()){
		if(write(sock,frame.data,imageSize) < 0){
			log_info("Error while sending..");
			return -1;
		}
	}
	log_info("frame process2");
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
	// Déterminer la résolution (par défaut ou spécifiée)
	int resW = -1;
	int resH = -1;
	if (argc == 3){
		resW = std::stoi(std::string(argv[1]));
		resH = std::stoi(std::string(argv[2]));;
	}

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
	if (resW > 0 && resH > 0){
		capture.set(CV_CAP_PROP_FRAME_WIDTH, resW);
		capture.set(CV_CAP_PROP_FRAME_HEIGHT, resH);
	}
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
		if(sendImage(capture, frame, sock, 0) == -1){
			close(sock);
			return -1;
		}
	}

	for (;;) {
		log_info("communication");
		// Receive a reply from the client
		int numBytesRcvd = 0;
		// receive first frame and additionnal frame
		for (int i = 0; i < sizeof(uint32_t); i += numBytesRcvd) {
			if ((numBytesRcvd = recv(sock, &client_message,
					sizeof(uint32_t), 0)) == -1) {
				log_info("reception error");	
			}
		};
		log_info("communication1");
		if(ELE4205_OK == client_message) {
			log_info("ok recu");
			int result = sendImage(capture, frame, sock, 1);
			log_info(std::string("Resultat : ")
				+ std::to_string(result)
				+ std::string("\n"));
			if(result == -1){
				close(sock);
				return -1;
			}
		}
		else if (ELE4205_QUIT == client_message) {
			close(sock);
			return 0;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
	return 0;
}

