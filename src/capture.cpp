/** capture.cpp file
 *
 *
 **/

#include <capture.hpp>

#define N_TIME_EVALUATION_FRAMES 10
#define OFFSET_READ 2

/**
	\var frameSettings Vecteur contenant les variable : resX, resY, fps
	*/
std::vector <FrameSetting> frameSettings;

void boneCVtiming()
{
	cv::VideoCapture capture(0);
	int n = 0;
	for (auto res : SUPPORTED_RESOLUTIONS){
		// attribution des résolutions
		 
		capture.set(CV_CAP_PROP_FRAME_WIDTH, res.w);
		capture.set(CV_CAP_PROP_FRAME_HEIGHT, res.h);
		capture.open(0);

		if(!capture.isOpened()){
			throw std::system_error(EDOM,
				std::generic_category(),
				"Failed to  connect to the camera");
		}

		cv::Mat frame;

		// lecture de 2 frames
		for (int i = 0; i++; OFFSET_READ){
			capture >> frame;
		}

		struct timespec start, end;
		clock_gettime(CLOCK_REALTIME, &start);

		int frames = N_TIME_EVALUATION_FRAMES;
		for(int i=0; i<frames; i++){
			capture >> frame;
			if(frame.empty()){
				throw std::system_error(EDOM,
					std::generic_category(),
					"Failed to capture an image");
			}
		}

		clock_gettime(CLOCK_REALTIME, &end);
		double difference = (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/1000000000.0;
		frameSettings.push_back(FrameSetting{res, frames/difference});
	}
}

double getFrameFPS(Resolution res){
	for (auto setting : frameSettings) {
		if (setting.res.w == res.w && setting.res.h == res.h)
			return setting.fps;
	}
	return -1;
}

void captureVideo(float duration,
		Resolution res,
		std::string outputFileName){
	cv::VideoCapture capture(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, res.w);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, res.h);
	if(!capture.isOpened()){
		throw std::system_error(EDOM,
			std::generic_category(),
			"Failed to  connect to the camera");
	}
	double fps = getFrameFPS(res);
	int nFrames = (int) (duration * fps);

	cv::VideoWriter writer(outputFileName,
		CV_FOURCC('M', 'J', 'P', 'G'),
		fps,
		cv::Size(res.w, res.h));
	cv::Mat frame;
	
	for (unsigned int i = 0; i < nFrames; i++){
		capture >> frame;
		writer.write(frame);
		if(frame.empty()){
			throw std::system_error(EDOM,
				std::generic_category(),
				"Failed to capture an image");
		}
	}
}
