/**
 * Image displaying demo with OpenCV.
 *
 * Author:  David Qiu.
 * Email:   david@davidqiu.com
 * Email:   dicong.qiu@intel.com
 * Website: http://www.davidqiu.com/
 *
 * Copyright (C) 2014, David Qiu. All rights reserved.
 */

#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: %s <Image_Path>\n", argv[0]);
        return -1;
    }

    Mat image;
    image = imread( argv[1], 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);

    waitKey(0);

    return 0;
}

