/**
 * Face collection application. This application helps collect
 * facial data for face recognition.
 *
 * It combines face detection and image corpping functions. A 
 * rectangle will occur on the video if any face is detected. 
 * And the user just need to press [space] on the keyboard to 
 * save the face image as an image file.
 *
 * Author:  David Qiu.
 * Email:   david@davidqiu.com
 * Email:   dicong.qiu@intel.com
 * Website: http://www.davidqiu.com/
 *
 * Copyright (C) 2014, David Qiu. All rights reserved.
 */

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
  bool saveImageFlag = false;
  int saveImageCount = 0;

  // Check arguments
  if (argc != 3)
  {
    cout << "usage: " << argv[0] << " </path/to/haar_cascade> </path/to/device id>" << endl;
    cout << "\t </path/to/haar_cascade> -- Path to the Haar Cascade for face detection." << endl;
    cout << "\t <device id> -- The webcam device id to grab frames from." << endl;
    exit(1);
  }

  // Get the arguments
  string fn_haar = string(argv[1]);
  int deviceId = atoi(argv[2]);

  // Define the size of standard image
  int im_width = 200;
  int im_height = 200;
  
  // Load the cascade classifier model
  CascadeClassifier haar_cascade;
  haar_cascade.load(fn_haar);

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
    
    // Convert the current frame to grayscale
    Mat gray;
    cvtColor(original, gray, CV_BGR2GRAY);

    // Find the faces in the frame
    vector< Rect_<int> > faces;
    haar_cascade.detectMultiScale(gray, faces);
    
    // Process the faces in the current frame
    for(int i = 0; i < faces.size(); i++)
    {
      // Crop the face from the image. So simple with OpenCV C++
      Mat face = gray(faces[i]);

      // Resizing the face to standard size
      Mat face_resized;
      cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);

      // Check if save image
      if (saveImageFlag)
      {
        // Construct the file name
        stringstream ssFilename;
        string strFilename;
        ssFilename << time(NULL) << "_" << saveImageCount << ".jpg";
        ssFilename >> strFilename;

        // Save image as file
        imwrite(strFilename.c_str(), face_resized);
        saveImageCount++;
        cout << "[INFO] Image saved as " << strFilename << endl;
      }

      // Draw a green rectangle around the detected face
      rectangle(original, faces[i], CV_RGB(0, 255,0), 1);
    }

    // Reset the save image flag
    saveImageFlag = false;

    // Show the result
    imshow("face_collection", original);

    // Process keyboard events
    char key = (char) waitKey(10);
    switch (key)
    {
      case 27: exitAppFlag = true; break;
      case ' ': saveImageFlag = true; break;
    }
    
    // Check if exit application
    if (exitAppFlag) break;
  }

  return 0;
}

