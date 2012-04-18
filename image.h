#ifndef IMAGE_H
#define IMAGE_H

#include "opencv2/core/core.hpp"
#include <opencv2/opencv.hpp>

#include "colorEdgesDetection.h"

class Image
{
public:
    Image(std::string fileName);
    ~Image();
    Mat* getRasterMergedMask();

    Mat originalImage;
    Mat rasterMergedMask;
    Mat selMask;

    unsigned int nchannels;

    std::vector<Mat> channels;
    std::vector<std::string> channelsNames;
    std::vector<Mat> channelsMaskLUT;

    std::string imageFileName;

    void saveSel();
    void saveChannelMask();
};

#endif // IMAGE_H
