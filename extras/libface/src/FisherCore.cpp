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

#include "FisherCore.h"

namespace libface
{
  
FisherCore::FisherCore()
{
    // TODO Auto-generated constructor stub
}

FisherCore::~FisherCore()
{
    // TODO Auto-generated destructor stub
}

CvMat* FisherCore::mean(CvMat* matrix)
{
    int rows      = matrix->rows;
    int cols      = matrix->cols;
    CvMat* result = cvCreateMat(rows, 1, CV_32FC1);

    for (int i=0 ; i<rows ; i++)
    {
        double sum = 0;

        for (int j=0 ; j<cols ; j++)
        {
            sum = sum + cvGet2D(matrix, i, j).val[0];
        }
        sum = sum / cols;

        CvScalar mean = cvScalarAll(sum);
        cvSet2D(result,i,0,mean);
    }

    return result;
}

CvMat* FisherCore::subtract(CvMat* src1, CvMat* src2)
{
    CvMat* result = cvCreateMat(src1->rows, src1->cols, CV_32FC1);

    for (int i=0 ; i<src1->rows ; i++)
    {
        for (int j=0 ; j<src1->cols ; j++)
        {
            double diff   = cvGet2D(src1, i, j).val[0] - cvGet2D(src2, i, 0).val[0];
            CvScalar data = cvScalarAll(diff);
            cvSet2D(result, i, j, data);
        }
    }

    return result;
}

void FisherCore::reverseOrder(CvMat* matrix)
{
    CvMat* result = cvCreateMat(matrix->rows,matrix->cols,CV_32FC1);

    for (int i=matrix->rows-1 ; i>=0 ; i--)
    {
        for (int j=matrix->cols-1 ; j>=0 ; j--)
        {
            cvSet2D(result, matrix->rows-i-1, matrix->cols-j-1, cvGet2D(matrix,i,j));
        }
    }
}

CvMat* FisherCore::getColoumn(CvMat* matrix, int col)
{
    CvMat* result = cvCreateMat(matrix->rows,1,CV_32FC1);

    for (int i=0 ; i<matrix->rows ; i++)
    {
        cvSet2D(result, i, 0, cvGet2D(matrix, i, col));
    }

    return result;
}

void FisherCore::calulateFisherStats()
{
    printf("Loading training data \n");
    int Class_Number     = 3;
    int Class_Population = 1;
    int P                = Class_Number * Class_Population;
    int i,j;

    //images need to be scaled to the same size.
    IplImage* img = cvLoadImage("src/TrainDatabase/2.jpg",0); //0 - grayscale
    CvMat* img2   = cvCreateMat(img->height, img->width, CV_32FC1);
    cvConvert(img, img2);
    CvMat* row    = transpose(reshape(img2));

    //CvMat* img3 = cvCreateMat(200, 180, CV_32FC1);

    IplImage* img_training = cvLoadImage("src/TrainDatabase/3.jpg",0);
    CvMat* img2_training   = cvCreateMat(img_training->height, img_training->width, CV_32FC1);
    cvConvert(img_training, img2_training);
    CvMat* row2            = transpose(reshape(img2_training));

    CvMat* T = combine(row, row2);

    img  = cvLoadImage("src/TrainDatabase/5.jpg",0); //0 - grayscale
    img2 = cvCreateMat(img->height, img->width, CV_32FC1);
    cvConvert(img, img2);
    row  = transpose(reshape(img2));

    T    = combine(T, row);

    printf("Calculating mean of training data... \n");
    CvMat* m_database = mean(T);
    printf("Size of mean: %d x %d \n",m_database->rows,m_database->cols);

    printf("Subtracting mean from training data...\n");
    CvMat* A = subtract(T, m_database);
    printf("Size of A: %d x %d\n",A->rows,A->cols);

    printf("Calculating Transposed Multiplication...\n");
    CvMat* L = cvCreateMat(A->width, A->width, CV_32FC1);

    cvMulTransposed(A,L,1);
    printf("Size of L: %d x %d\n",L->rows,L->cols);

    printf("Calculating Eigen Vectors...\n");
    CvMat* values = cvCreateMat(L->rows,L->rows,CV_32FC1); //null here
    CvMat* vector = cvCreateMat(L->rows,L->rows,CV_32FC1); //null here  initialise both to the size of the covv which is a*a'

    cvEigenVV(L,vector ,values);

    for (int i=0 ; i<values->rows ; i++)
    {
        for (int j=0 ; j<values->cols ; j++)
        {
            printf("%f \t",cvGet2D(values, i, j).val[0]);
        }
        printf("\n");
    }

    for (int i=0 ; i<vector->rows ; i++)
    {
        for (int j=0 ; j<vector->cols ; j++)
        {
            printf("%f \t",cvGet2D(vector, i, j).val[0]);
        }
        printf("\n");
    }

    printf("Calculating V_PCA... \t");
    CvMat* V_PCA = cvCreateMat(A->rows, A->cols, CV_32FC1);
    cvMatMul(A, vector, V_PCA);

    printf("Projecting into Eigen Space...\n");
    CvMat* V_PCA_t             = transpose(V_PCA);
    CvMat* ProjectedImages_PCA = 0;

    for (i=0;i<P;i++)
    {
        CvMat* col  = getColoumn(A, i);
        CvMat* temp = cvCreateMat(V_PCA_t->rows, col->cols,CV_32FC1);
        cvMatMul(V_PCA_t, col, temp);

        if (ProjectedImages_PCA == 0)
        {
            ProjectedImages_PCA = temp;
        }
        else
        {
            ProjectedImages_PCA = combine(ProjectedImages_PCA, temp);
        }
    }
    printf("DONE\n");

    printf("Size of ProjectedImages_PCA: %d x %d\n",ProjectedImages_PCA->rows,ProjectedImages_PCA->cols);

    printf("Calculating mean of Eigen values...\n");

    CvMat* m_PCA = mean(ProjectedImages_PCA);
    printf("Done\n");
    CvMat* m  = 0;
    CvMat* Sw = 0;
    CvMat* Sb = 0;
}

} // namespace libface
