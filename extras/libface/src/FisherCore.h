/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2009-12-21
 * @brief
 * @section DESCRIPTION
 *
 * @author Copyright (C) 2009 by Aleksey
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

#ifndef _FISHERCORE_H_
#define _FISHERCORE_H_

#if defined (__APPLE__)
#include <cv.h>
#else
#include <opencv/cv.h>
#endif

namespace libface
{

class FACEAPI FisherCore
{
public:

    FisherCore();
    virtual ~FisherCore();

    void calulateFisherStats();

private:

    CvMat* mean(CvMat* matrix);
    CvMat* subtract(CvMat* src1, CvMat* src2);
    void reverseOrder(CvMat* matrix);
    CvMat* getColoumn(CvMat* matrix, int col);
};

} // namespace libface

#endif /* _FISHERCORE_H_ */
