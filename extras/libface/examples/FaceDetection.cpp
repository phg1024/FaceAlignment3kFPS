/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2010-01-03
 * @brief   Faces detection example.
 * @section DESCRIPTION
 *
 * This is a simple example of the use of the libface library.
 * It implements face detection and recognition and uses the opencv libraries.

 * @note: libface does not require users to have openCV knowledge, so here, 
 *        openCV is treated as a "3rd-party" library for image manipulation convenience.
 *
 * @author Copyright (C) 2010 by Alex Jironkin
 *         <a href="alexjironkin at gmail dot com">alexjironkin at gmail dot com</a>
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="adityabhatt at gmail dot com">adityabhatt at gmail dot com</a>
 * @author Copyright (C) 2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <iostream>
#include <vector>

#include <math.h>

#include <LibFace.h>
#include <Face.h>        // Our library

// Extra libraries for use in client programs

#if defined (__APPLE__)
#include <cv.h>
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

using namespace std;

//Use namespace libface in the library.
using namespace libface;

IplImage* resizeToArea(IplImage* img, int area)
{
    // Area of input image
    int W = img->width;
    int H = img->height;

    // We want an area of A pixels in the output image - that should be analyzable
    // Suppose width and height of input image are W and H. And we must divide both by same factor z.
    // Then, (W/z) * (H/z) = A is what we want.
    // So, z = sqrt(W*H/A);

    double z = sqrt((double)(W) * (double)(H) / (double)(area));

    CvSize s;
    s.width  = (int)(W/z);
    s.height = (int)(H/z);

    IplImage* out = cvCreateImage(s, img->depth, img->nChannels);
    cvResize(img, out);
    return out;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        cout << "Bad Args!!!\nUsage: " << argv[0] << " <image1> <image2> ..." << endl;
        return 0;
    }

    vector<Face>  result, finalresult;
    libface::Mode mode;
    mode             = libface::DETECT;
    LibFace* libFace = new LibFace(mode, string("."));
    IplImage* img    = 0;

    for (int i = 1; i < argc; ++i)
    {
        // Load input image
        cout << "Loading image " << argv[i] << endl;
        img = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

        // We can give the filename to this function too, but a better method is the one done below,
        // in which raw image data is passed
        result = libFace->detectFaces(img->imageData,img->width, img->height, img->widthStep, img->depth, img->nChannels);
        cout << " detected" << endl;

        for (unsigned int j = 0; j < result.size(); ++j)	// Draw squares over detected faces
        {
            Face* face = &result.at(j);
            cout<<"Drawing"<<endl;
            cvRectangle( img, cvPoint(face->getX1(), face->getY1())
                         , cvPoint(face->getX2(), face->getY2())
                         , CV_RGB(255,0,0), 3, 2, 0);
        }

        if(img->height*img->width > 400000)
            img = resizeToArea(img, 400000);

        cout << "Displaying " << argv[i] << endl;
        cvNamedWindow(argv[i]);
        cvShowImage(argv[i], img);
        cvWaitKey(0);
        cvDestroyWindow(argv[i]);
        cvSaveImage("out.png", img);
        finalresult.insert(finalresult.end(), result.begin(), result.end());	// Append result to finalresult
        result.clear();
    }

    return 0;
}
