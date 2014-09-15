/**
 * Face recognition from video stream using OpenCv.
 *
 * Author:  David Qiu.
 * Email:   david@davidqiu.com
 * Email:   dicong.qiu@intel.com
 * Website: http://www.davidqiu.com/
 *
 * Copyright (C) 2014, David Qiu.
 * 
 * This piece of code is modified from the video stream face recognition 
 * toturial of OpenCV.
 */

/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

using namespace cv;
using namespace std;

const int STD_FACE_WIDTH  = 64;
const int STD_FACE_HEIGHT = 64;
const int STD_DETECTION_WIDTH  = 320;
const int STD_DETECTION_HEIGHT = 240;


static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, map<int, string>& names, char separator = ';') {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel, classname;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel, separator);
        getline(liness, classname);
        if(!path.empty() && !classlabel.empty() && !classname.empty()) {
            Mat img_original = imread(path, 0);
            Mat img_resized;
            cv::resize(img_original, img_resized, Size(STD_FACE_WIDTH, STD_FACE_HEIGHT), 1.0, 1.0, INTER_CUBIC);
            images.push_back(img_resized);
            labels.push_back(atoi(classlabel.c_str()));
            names.insert( pair<int, string>(atoi(classlabel.c_str()), classname) );
        }
    }
}

int main(int argc, const char *argv[]) {
    bool saveFaceFlag = false;
    int saveImageCount = 0;
    bool exitAppFlag = false;

    // Check the arguments
    if (argc != 4) {
        cout << "usage: " << argv[0] << " </path/to/haar_cascade> </path/to/csv.ext> </path/to/device id>" << endl;
        cout << "\t </path/to/haar_cascade> -- Path to the Haar Cascade for face detection." << endl;
        cout << "\t </path/to/csv.ext> -- Path to the CSV file with the face database." << endl;
        cout << "\t <device id> -- The webcam device id to grab frames from." << endl;
        exit(1);
    }

    // Get the arguments
    string fn_haar = string(argv[1]);
    string fn_csv = string(argv[2]);
    int deviceId = atoi(argv[3]);

    // Load the face database
    vector<Mat> images;
    vector<int> labels;
    map<int, string> names;
    try {
        read_csv(fn_csv, images, labels, names);
    } catch (cv::Exception& e) {
        cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
        exit(1);
    }
    
    // Get the standard face image size
    int im_width = images[0].cols;
    int im_height = images[0].rows;

    // Create and train a face recognizer
    Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
    model->train(images, labels);

    // Create and train a face detecter
    CascadeClassifier haar_cascade;
    haar_cascade.load(fn_haar);

    // Open the video capture device
    VideoCapture cap(deviceId);
    if(!cap.isOpened()) {
        cerr << "Capture Device ID " << deviceId << "cannot be opened." << endl;
        return -1;
    }

    // Get the frame inversed scale factor
    Mat frame;
    cap >> frame;
    double invfscale_x = ((double)frame.cols)/((double)STD_DETECTION_WIDTH);
    double invfscale_y = ((double)frame.rows)/((double)STD_DETECTION_HEIGHT);

    // Process frames from the video stream
    for(;;) {
        // Obtain the next frame from the video stream
        cap >> frame;

        // Clone the current frame
        Mat original = frame.clone();

        // Resize the frame
        Mat original_resized;
        cv::resize(original, original_resized, Size(STD_DETECTION_WIDTH, STD_DETECTION_HEIGHT), 1.0, 1.0, INTER_CUBIC);
        
        // Convert to grayscale
        Mat gray;
        cvtColor(original_resized, gray, CV_BGR2GRAY);

        // Find the faces in the frame
        vector< Rect_<int> > faces;
        haar_cascade.detectMultiScale(gray, faces);

        // Process all the faces detected
        for(int i = 0; i < faces.size(); i++) {
            // Obtain the current face
            Rect face_i = faces[i];
            Rect face_i_original(
                (double)face_i.x * invfscale_x,
                (double)face_i.y * invfscale_y,
                (double)face_i.width * invfscale_x,
                (double)face_i.height * invfscale_y);

            // Crop the face from the image
            Mat face = gray(face_i);

            // Resize the face image for recognition
            Mat face_resized;
            cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);

            // Perform prediction
            double confidence = 0.0;
            int prediction = -1;
            model->predict(face_resized, prediction, confidence);

            // Check if save face image
            if (saveFaceFlag)
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


            // Tag the face with rectangle
            rectangle(original, face_i_original, CV_RGB(0, 255,0), 1);

            // Put information above the rectangle
            string strName = names[prediction];
            string box_text = format("Prediction = %s [%lf]", strName.c_str(), confidence);
            int pos_x = std::max(face_i_original.tl().x - 10, 0);
            int pos_y = std::max(face_i_original.tl().y - 10, 0);
            putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0), 2.0);
        }

        // Reset save face flag
        saveFaceFlag = false;

        // Show the result
        imshow("face_recognizer", original);

        // Detect keyboard events
        char key = (char) waitKey(1);
        switch (key) {
            case 27: exitAppFlag=true; break;
            case ' ': saveFaceFlag=true; break;
        }
        if (exitAppFlag) break;
    }
    return 0;
}


