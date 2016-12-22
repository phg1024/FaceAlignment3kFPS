/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2010-01-03
 * @brief   Random tests example.
 * @section DESCRIPTION
 *
 * This file is intended to test some simple functions in libface's interface.
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

// Use namespace libface in the library.
using namespace libface;

int main(int argc, char** argv)
{
    // Load libface with DETECT to only do detection mode
    //"." means look for configuration file in current directory

    LibFace libFace = LibFace(ALL, ".");

    cout << "Count of people that libface has been trained with : " << libFace.count() << endl;

    return 0;
}
