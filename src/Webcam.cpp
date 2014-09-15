
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>

using namespace cv;
using namespace std;


int main(int argc, const char *argv[])
{
  bool exitAppFlag = false;
  
  // Check arguments
  if (argc != 2)
  {
    cout << "usage: " << argv[0] << " <device_id>" << endl;
    cout << "\t <device_id> -- The webcam device id to grab frames from." << endl;
    exit(1);
  }

  // Get the arguments
  int deviceId = atoi(argv[1]);

  // Open the camera device
  VideoCapture cap(deviceId);
  if(!cap.isOpened()) {
    cerr << "Capture Device ID " << deviceId << "cannot be opened." << endl;
    return -1;
  }

  // Holds the current frame from the Video device
  Mat frame;
  while (true)
  {
    // Get a new frame from the video stream
    cap >> frame;
    Mat original = frame.clone();
    
    // Show the result
    imshow("webcam", original);

    // Process keyboard events
    char key = (char) waitKey(10);
    switch (key)
    {
      case 27: exitAppFlag = true; break;
    }
    
    // Check if exit application
    if (exitAppFlag) break;
  }

  return 0;
}

