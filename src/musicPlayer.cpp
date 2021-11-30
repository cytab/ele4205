#include "musicPlayer.hpp"

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

int getTempo(std::string* sheetMusic)
{
	std::string tmp = "";
	for (unsigned int i = 0; i < sheetMusic->size(); i++) {
		char element = sheetMusic->at(i);
		if (element != ' ')
			tmp += element;
		else
			break;
	}
	return std::stoi(tmp);
}

Note getNote(std::string code)
{
	Note note = {};
	bool hasSharp = false;
	if (code.size() <= 1 || code.size() > 4) {
		note.frequency = 0;
		note.duration = 0.0;
		return note;
	}
	// Special case: '#' is decoded as '11'.
	if (code.size() == 4) {
		if (code.at(1) == '1' && code.at(2) == '1') {
			code = code.at(0) + "#" + code.at(3);
		}
		else {
			note.frequency = 0;
			note.duration = 0.0;
			return note;
		}
	}
	// Get note height (frequency)
	try {
		std::string f_chars = "";
		if (code.at(1) == '#') {
			f_chars = code.substr(0, 2);
			hasSharp = true;
		}
		else {
			f_chars = code.substr(0, 1);
		}
		// Special cases during erroneous character recognition.
		// 'B' interpreted as '8'
		if (f_chars.at(0) == '8') {
			f_chars[0] = 'B';
		}
		// 'c' interpreted at '0'
		if (f_chars.at(0) == '0') {
			f_chars[0] = 'c';
		}
		note.frequency = NOTE_FREQUENCIES.at(f_chars);
	}
	catch (...) {
		note.frequency = 0;
		note.duration = 0.0;
		return note;
	}
	// Get note duration.
	try {
		std::string d_chars = "";
		if (hasSharp) {
			d_chars = code.substr(2, 3);
		}
		else {
			d_chars = code.substr(1, 2);
		}
		// Special cases during character recognition
		// 'B' interpreted as '8'
		if (d_chars.at(0) == 'B') {
			d_chars[0] = '8';
		}
		// '0' interpreted at 'c'
		if (d_chars.at(0) == 'c') {
			d_chars[0] = '0';
		}
		int durationIndex = std::stoi(d_chars);
		float duration = NOTE_DURATIONS.at(durationIndex);
		note.duration = duration;
	}
	catch (...) {
		note.duration = 0.0;
	}
	return note;
}

void getNotes(std::string* sheetMusic, std::vector<Note>& notes)
{
	std::string buffer = "";
	bool isTempoPassed = false;
	for (unsigned int i = 0; i < sheetMusic->size(); i++){
		if (!isTempoPassed) {
			if (sheetMusic->at(i) == ' ') {
				isTempoPassed = true;
			}
		}
		else {
			std::string c(1, sheetMusic->at(i));
			std::regex re("[A-Z]|[a-z]|[0-9]|#");
			std::cmatch m;
			if (std::regex_match(c.c_str(), m, re)){
				buffer += c;
			}
			else {
				if (buffer.size() > 0){
					Note tmp = getNote(buffer);
					notes.push_back(tmp);
					buffer = "";
				}
			}
		}
	}
}

void playNote(Note& note, float beat)
{
	FILE * f = fopen(FREQUENCY_PATH, "w");
	std::string freq = std::to_string(note.frequency);
	fwrite(freq.c_str(), freq.size(), sizeof(char), f);
	fclose(f);

	f = fopen(ENABLE_PATH, "w");
	if (note.frequency == 0){
		fwrite("0", 1, sizeof(char), f);
	} else {
		fwrite("1", 1, sizeof(char), f);
	}
	fclose(f);

	int duration_ms = note.duration * 1000.0 * beat;
	std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));

	f = fopen(ENABLE_PATH, "w");
	fwrite("0", 1, sizeof(char), f);
	fclose(f);
}

void playMusic(std::string* sheetMusic)
{
	// Tempo
	int tempo = getTempo(sheetMusic);
	float beat = 60.0 / (float) tempo;
	// Notes
	std::vector<Note> notes = {};
	getNotes(sheetMusic, notes);
	// Configure volume.
	FILE * f = fopen(DUTY_PATH, "w");
	std::string dutyCycle = "512";
	fwrite(dutyCycle.c_str(), dutyCycle.size(), sizeof(char), f);
	fclose(f);
	// Play all notes
	for(Note n : notes){
		playNote(n, beat);
	}
	// Close everything
	f = fopen(DUTY_PATH, "w");
	dutyCycle = "0";
	fwrite(dutyCycle.c_str(), dutyCycle.size(), sizeof(char), f);
	fclose(f);
}

int main(int argc, char *argv[])
{
	uint32_t message;
	char* message_serveur = (char*)&message;

	int socket_desc, sock, clientLen ;
	struct sockaddr_in client;

	uint32_t client_message = 0;

	// Create socket
	socket_desc = SocketCreate();
	if (socket_desc == -1) {
		log_info("Could not create socket");
		return 1;
	}
	// Bind
	if (BindCreatedSocket(socket_desc, MUSIC_PORT_NUMBER) < 0) {
		perror("bind failed.");
		return 1;
	}

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

	if (recv(sock, &client_message, sizeof(uint32_t), 0) == -1) {
		log_info("Size reception error");
	}
	char* sockData = new char[client_message];
	for (int i = 0; i < client_message-1; i += 1) {
		recv(sock, &sockData[i], sizeof(char), 0);
	}
	
	std::string sheetMusic;
	for (int i = 0; i < client_message-1; i += 1) {
		sheetMusic += sockData[i];
	}

	delete sockData;

	playMusic(&sheetMusic);
	close(sock);
}
