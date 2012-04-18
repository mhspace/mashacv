#include <iostream>
#include "colorEdgesDetection.h"
#include "hueWidget.h"
#include <opencv2/opencv.hpp>

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QSlider>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
  if (argc == 1)
    {
      // using GUI

      QApplication a(argc, argv);
      MainWidget w;

      w.show();

      return a.exec();
    

      /*cout << "Usage: " << argv[0] << " <image_filename>" << endl;
	return 0;*/
    }
  else
    {
      double t = (double)getTickCount();                      // time counting
      
      vector<unsigned int> sizes;
      
      Mat originalImage;
      originalImage = imread(argv[1], -1);
      
      Mat HSVImage;
      cvtColor(originalImage, HSVImage, CV_BGR2HSV);
      
      /*
	Mat HLSImage;
	cvtColor(originalImage, HLSImage, CV_BGR2HLS);
      */
      
      int from_to[] = { 0,0 };
      
      Mat HueImage(originalImage.rows, originalImage.cols, CV_8UC1);
      
      mixChannels(&HSVImage, 1, &HueImage, 1, from_to, 1);
      
      bool counted[HueImage.rows * HueImage.cols];
      for (int i = 0; i < HueImage.cols * HueImage.rows; i++)
	counted[i] = false;
      
      for (int i = 0; i < HueImage.rows; i++)
	for (int j = 0; j < HueImage.cols; j++)
	  if (isIt(&HueImage.at<uchar>(i, j)) && !counted[HueImage.cols * i + j])
        sizes.push_back(inspectold(i, j, &HueImage, counted));
      
      Mat maskImage(originalImage.rows, originalImage.cols, CV_8UC1);
      for (int i = 0; i < maskImage.rows; i++)
	for (int j = 0; j < maskImage.cols; j++)
	  if (counted[maskImage.cols * i + j])
	    maskImage.at<uchar>(i, j) = (uchar)255;
      imwrite("hue.png", HueImage);
      imwrite("out.png", maskImage);
      
      for (int i = 0; i < sizes.size(); i++)
	cout << sizes.at(i) << " ";
      cout << endl;
      
      
      unsigned int summSizes = 0;
      for (int i = 0; i < sizes.size(); i++)
	summSizes += sizes.at(i);
      
      unsigned int summSquareSizes = 0;
      for (int i = 0; i < sizes.size(); i++)
	summSquareSizes += (sizes.at(i)*sizes.at(i));

      double average = summSizes/sizes.size();
      double sqaverage = summSquareSizes/sizes.size();

      cout << "D " << ( sqaverage - (average*average) ) << endl << "average " << average << endl << "sum " << summSizes << endl << "n " << sizes.size() << endl;

      t = ((double)getTickCount() - t)/getTickFrequency();    // time counting
      cout << "Times passed in seconds: " << t << endl;       // time counting

      return 0;
    }
}
