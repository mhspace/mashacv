#include "image.h"
#include <fstream>
#include <iostream>

Image::Image(std::string fileName)
{
    this->imageFileName = fileName;

    this->originalImage = imread(this->imageFileName, -1);

    Mat HSVImage;
    cvtColor(this->originalImage, HSVImage, CV_BGR2HSV_FULL);

    this->nchannels = this->originalImage.channels();

    Mat channels[nchannels];
    split(HSVImage, channels);
    for (int i = 0; i < nchannels; i++)
    {
        this->channels.push_back(channels[i]);
        Mat channelMaskLUT = Mat::ones(1, 256, CV_8U);
        this->channelsMaskLUT.push_back(channelMaskLUT);
    }
    this->channelsNames.push_back("H");
    this->channelsNames.push_back("S");
    this->channelsNames.push_back("V");

    ifstream fcrange((this->imageFileName + ".colorrange").c_str());
    if (fcrange)
    {
        for (int i = 0; i < nchannels; i++)
        {
            char ch[770];
            fcrange.getline(ch, 770);
            std::string chs(ch);
            chs = chs.substr(1, chs.length() - 2);
            for (int s = 0; s < 255; s++)
            {
                unsigned int c = atoi(chs.substr(s*3, 1).c_str());
                this->channelsMaskLUT.at(i).at<uchar>(s) = (uchar)c;
            }
        }
        fcrange.close();
    }

    ifstream fmask((this->imageFileName + ".mask.png").c_str());
    if (fmask)
    {
        fmask.close();
        this->selMask = imread(this->imageFileName + ".mask.png", 0);
    }
    else
    {
        this->selMask = Mat::zeros(this->originalImage.rows, this->originalImage.cols, CV_8UC1);
    }

    cvtColor(this->originalImage, this->originalImage, CV_BGR2RGB);
}

Image::~Image()
{
}

Mat *Image::getRasterMergedMask()
{
    Mat* mergedMask = new Mat(this->originalImage.rows, this->originalImage.cols, CV_8UC1);
    *mergedMask = mergedMask->ones(this->originalImage.rows, this->originalImage.cols, CV_8UC1);
    bitwise_and(*mergedMask, this->selMask, *mergedMask);
    for (int i = 0; i < nchannels; i++)
    {
        Mat channelmask = Mat::zeros(this->originalImage.rows, this->originalImage.cols, CV_8UC1);
        LUT(this->channels[i], this->channelsMaskLUT[i], channelmask);
        bitwise_and(*mergedMask, channelmask, *mergedMask);
    }

    return mergedMask;
}

void Image::saveSel()
{
    imwrite(this->imageFileName + ".mask.png", this->selMask);
}

void Image::saveChannelMask()
{
    ofstream fcrange((this->imageFileName + ".colorrange").c_str());
    for (int i = 0; i < nchannels; i++)
        fcrange << this->channelsMaskLUT.at(i) << std::endl;
    fcrange.flush();
    fcrange.close();
}
