/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2010-01-03
 * @brief   Test example.
 * @section DESCRIPTION
 *
 * This is a simple example of the use of the libface library.
 * It implements face detection and recognition and uses the opencv libraries.
 *
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

// Extra libraries for use in client programs
#if defined (__APPLE__)
#include <cv.h>
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

// Our library
#include "LibFace.h"
#include "Face.h"

using namespace std;

//Use namespace libface in the library.
using namespace libface;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        cout << "Bad Args!!!\nUsage: " << argv[0] << " <image1> <image2> ..." << endl;
        return 0;
    }

    // Load libface with DETECT to only do detection mode
    // "." means look for configuration file in current directory

    LibFace libFace = LibFace(ALL, ".");

    int i, j;
    IplImage* img=0;
    vector<Face> result;    // Vector of faces returned from a particular photo's detection
    vector<Face> finalresult;    // The combined vector of faces after detection on all photos is over

    for (i = 1; i < argc; ++i)
    {
        // Load input image
        cout << "Loading image " << argv[i] << endl;
        img    = cvLoadImage(argv[1]);
        result = libFace.detectFaces(string(argv[i]));
        cout << " detected" << endl;

        for (j = 0; j < result.size(); ++j)    // Draw squares over detected faces
        {

            Face* face = &result.at(j);

            cout << "Drawing" << endl;
            cvRectangle( img, cvPoint(face->getX1(), face->getY1()),
                         cvPoint(face->getX2(), face->getY2()),
                         CV_RGB(255,0,0), 3, 2, 0);
        }

        //cout<<"Displaying "<<argv[i]<<endl;
        //LibFaceUtils::showImage(img);

        finalresult.insert(finalresult.end(), result.begin(), result.end());    // Append result to finalresult
        result.clear();
    }

    cout << "Will recognize " << finalresult.size() << " faces..." << endl;

    vector<pair<int, double> >recognised;
    recognised = libFace.recognise(&finalresult);

    for( i = 0; i < recognised.size(); ++i)
    {
        cout << "ID matched : " << recognised.at(i).first << endl;
        cout << "Distance : " << recognised.at(i).second << endl;
    }

    return 0;
}
