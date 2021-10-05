/** capture.cpp file
*
*
**/

#include <capture.hpp>

#include<iostream>
#include<time.h>

using namespace std;
using namespace cv;

void bonecVtiming()
{
    
    VideoCapture capture(0);
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
        capture >> frame;
        capture >> frame;

        struct timespec start, end;
        clock_gettime( CLOCK_REALTIME, &start );

        int frames=10;
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
