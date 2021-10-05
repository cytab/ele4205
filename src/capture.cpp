/** capture.cpp file
 *
 *
 **/

#include <capture.hpp>

#define NOMBRE_DE_FRAME 10
#define OFFSET_READ 2

using namespace std;
using namespace cv;



std::string getLsUSB(){
	FILE *fpipe;
	char *command = "lsusb";
	char c = 0;
	std::string output;
	if (0 == (fpipe = (FILE*)popen(command, "r"))) {
		perror("popen() failed.");
		return "";
	}
	while (fread(&c, sizeof c, 1, fpipe)) {
		output += c;
	}
	pclose(fpipe);
	return 
}

std::string getVideoFileName(std::string cameraID){
	std::string devices = getLsUSB();
	std::string line;
	while (std::getline(devices, line)) {
		if (line.fins(CAMERA_ID) != std::string::npos)
			break;
	}
	std::string busID = line.substr(4, 7);
	std::string deviceID = line.substr(16, 19);
	return std::string("/dev/bus/usb/" + busID + "/" + deviceID);
}

void bonecVtiming(std::string fileName)
{
    
    VideoCapture capture(fileName);
    int n = 0;

    for (auto i : SUPPORTED_RESOLUTIONS){

        // attribution des résolutons 
        capture.set(CV_CAP_PROP_FRAME_WIDTH, i.w);
        capture.set(CV_CAP_PROP_FRAME_HEIGHT, i.h);

        if(!capture.isOpened()){
             throw std::invalid_argument("Failed to  connect to the camera");
        }

        Mat frame, edges;

        // lecture de 2 frames
        for (int i = 0; i++; OFFSET_READ){
            capture >> frame;
        }

        struct timespec start, end;
        clock_gettime( CLOCK_REALTIME, &start );

        int frames = NOMBRE_DE_FRAME;
        for(int i=0; i<frames; i++){
            capture >> frame;
            if(frame.empty()){
                throw std::invalid_argument("Failed to capture an image");
            }
        }

        clock_gettime( CLOCK_REALTIME, &end );
        double difference = (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/1000000000.0;
        timeFrame[i] = frames/difference ; 

    }

}
