/** capture.cpp file
 *
 *
 **/

#include <capture.hpp>

#define NOMBRE_DE_FRAME 10
#define OFFSET_READ 2

std::string getLsUSB(){
	FILE *fpipe;
	char *command = "lsusb";
	char c = 0;
	std::string output;
	if (0 == (fpipe = (FILE*)popen(command, "r"))) {
		perror("popen() failed.");
		return "";
	}
	while (fread(&c, sizeof(c), 1, fpipe)) {
		output += c;
	}
	pclose(fpipe);
	return output;
}

std::string getVideoFileName(std::string cameraID){
	std::string devices = getLsUSB();
	std::string line;
	while (std::getline(devices, line)) {
		if (line.find(cameraID) != std::string::npos)
			break;
	}
	std::string busID = line.substr(4, 7);
	std::string deviceID = line.substr(16, 19);
	return std::string("/dev/bus/usb/" + busID + "/" + deviceID);
}

void boneCVtiming(std::string fileName)
{
	cv::VideoCapture capture(fileName);
	int n = 0;

	for (auto i : SUPPORTED_RESOLUTIONS){
		// attribution des résolutions 
		capture.set(cv::CV_CAP_PROP_FRAME_WIDTH, i.w);
		capture.set(cv::CV_CAP_PROP_FRAME_HEIGHT, i.h);

		if(!capture.isOpened()){
			throw std::system_error("Failed to  connect to the camera");
		}

		cv::Mat frame;

		// lecture de 2 frames
		for (int i = 0; i++; OFFSET_READ){
			capture >> frame;
		}

		struct timespec start, end;
		clock_gettime(CLOCK_REALTIME, &start);

		int frames = NOMBRE_DE_FRAME;
		for(int i=0; i<frames; i++){
			capture >> frame;
			if(frame.empty()){
				throw std::system_error("Failed to capture an image");
			}
		}

		clock_gettime(CLOCK_REALTIME, &end);
		double difference = (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/1000000000.0;
		timeFrame[i] = frames/difference ; 
	}
}

void captureVideo(std::string fileName,
		float duration,
		Resolution res,
		std::string outputFileName){
	cv::VideoCapture capture(fileName);
	capture.set(cv::CV_CAP_PROP_FRAME_WIDTH, res.w);
	capture.set(cv::CV_CAP_PROP_FRAME_HEIGHT, res.h);
	if(!capture.isOpened()){
		throw std::system_error("Failed to  connect to the camera");
	}
	double fps = timeFrame[res];
	int nFrames = (int) (duration * fps);

	cv::VideoWriter writer(outputFileName,
		cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
		fps,
		cv::Size(res.w, res.h));
	cv::Mat frame;
	
	for (unsigned int i = 0; i < nFrames; i++){
		capture >> frame;
		writer.write(frame);
		if(frame.empty()){
			throw std::system_error("Failed to capture an image");
		}
	}
}
