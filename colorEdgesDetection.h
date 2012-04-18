#ifndef COLOR_EDGES_DETECTION
#define COLOR_EDGES_DETECTION


#include <iostream>
#include "opencv2/core/core.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

bool isIt(uchar*);
int inspectold(int, int, Mat*, bool*);
int findColorAreas(const Mat*, std::vector<size_t>*, std::vector<cv::Point>*, bool = false, std::vector<cv::Rect>* = NULL);

#endif
