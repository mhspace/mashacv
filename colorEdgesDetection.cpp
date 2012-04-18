#include <iostream>
#include "opencv2/core/core.hpp"
#include <opencv2/opencv.hpp>
#define recursionLimit 100000
#include <QtCore> //Шindoшs

using namespace cv;
using namespace std;

bool isIt(uchar* HuePixel)
{
  if ((10 <= (int)*HuePixel) and (100 >= (int)*HuePixel))
    return true;
  else
    return false;
}

int inspectold(int x, int y, Mat* image, bool* counted)
{
  int sx = image->cols - 1;
  int sy = image->rows - 1;
  if (!counted[image->cols*x*image->channels() + y])
    {
      counted[image->cols*x*image->channels() + y] = true;
      int right = 0;
      if (x + 1 < sx)
	if (!counted[image->cols*(x + 1)*image->channels() + y])
	  if (isIt(&image->at<uchar>(x + 1, y)))
        right = inspectold(x + 1, y, image, counted);

      int left = 0;
      if (x - 1 >= 0)
	if (!counted[image->cols*(x - 1)*image->channels() + y])
	  if (isIt(&image->at<uchar>(x - 1, y)))
        left = inspectold(x - 1, y, image, counted);

      int top = 0;
      if (y - 1 >= 0)
	if (!counted[image->cols*x*image->channels() + y - 1])
	  if (isIt(&image->at<uchar>(x, y - 1)))
        top = inspectold(x, y - 1, image, counted);
      
      int bottom = 0;
      if (y + 1 < sy)
	if (!counted[image->cols*x*image->channels() + y + 1])
	  if (isIt(&image->at<uchar>(x, y + 1)))
        bottom = inspectold(x, y + 1, image, counted);
      
      return (right + left + top + bottom + 1);
    }
  
  return 0;
}

int inspect(int x, int y, Mat* image)
{
    image->at<uchar>(y, x) = (uchar)2;

    int right = 0;
    if (x + 1 < image->cols)
        if (image->at<uchar>(y, x + 1) == (uchar)1)
            right = inspect(x + 1, y, image);

    int left = 0;
    if (x - 1 >= 0)
        if (image->at<uchar>(y, x - 1) == (uchar)1)
            left = inspect(x - 1, y, image);

    int top = 0;
    if (y - 1 >= 0)
        if (image->at<uchar>(y - 1, x) == (uchar)1)
            top = inspect(x, y - 1, image);

    int bottom = 0;
    if (y + 1 < image->rows)
        if (image->at<uchar>(y + 1, x) == (uchar)1)
            bottom = inspect(x, y + 1, image);

    return (right + left + top + bottom + 1);
}

int inspectAndCollectPoints(int x, int y, Mat* image, uint* xmin, uint* ymin, uint* xmax, uint* ymax)
{
    if (x < *xmin)
        *xmin = x;
    if (x > *xmax)
        *xmax = x;
    if (y < *ymin)
        *ymin = y;
    if (y > *ymax)
        *ymax = y;

    image->at<uchar>(y, x) = (uchar)2;

    int right = 0;
    if (x + 1 < image->cols)
        if (image->at<uchar>(y, x + 1) == (uchar)1)
            right = inspectAndCollectPoints(x + 1, y, image, xmin, ymin, xmax, ymax);

    int left = 0;
    if (x - 1 >= 0)
        if (image->at<uchar>(y, x - 1) == (uchar)1)
            left = inspectAndCollectPoints(x - 1, y, image, xmin, ymin, xmax, ymax);

    int top = 0;
    if (y - 1 >= 0)
        if (image->at<uchar>(y - 1, x) == (uchar)1)
            top = inspectAndCollectPoints(x, y - 1, image, xmin, ymin, xmax, ymax);

    int bottom = 0;
    if (y + 1 < image->rows)
        if (image->at<uchar>(y + 1, x) == (uchar)1)
            bottom = inspectAndCollectPoints(x, y + 1, image, xmin, ymin, xmax, ymax);

    return (right + left + top + bottom + 1);
}

int findColorAreas(const Mat* src, std::vector<size_t>* edges, std::vector<cv::Point>* firstPoints, bool collectPoints, std::vector<cv::Rect>* points)
{
    Mat img = src->clone();

    if (collectPoints)
    {
        for (int y = 0; y < img.rows; y++)
            for (int x = 0; x < img.cols; x++)
                if (img.at<uchar>(y, x) == (uchar)1)
                {
                    uint xmin = img.cols;
                    uint xmax = 0;
                    uint ymin = img.rows;
                    uint ymax = 0;
                    edges->push_back(inspectAndCollectPoints(x, y, &img, &xmin, &ymin, &xmax, &ymax));
                    firstPoints->push_back(Point(x, y));
                    points->push_back(Rect(xmin, ymin, xmax - xmin + 1, ymax - ymin + 1));
                }
    }
    else
    {
        for (int y = 0; y < img.rows; y++)
            for (int x = 0; x < img.cols; x++)
                if (img.at<uchar>(y, x) == (uchar)1)
                    {
                        edges->push_back(inspect(x, y, &img));
                        firstPoints->push_back(Point(x, y));
                    }
    }

    img.~Mat();

    return 0;
}
