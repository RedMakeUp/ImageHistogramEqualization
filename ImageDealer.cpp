#include "ImageDealer.h"

#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QRgb>

#include <vector>

void ImageDealer::GrayScale(const std::vector<unsigned char>& image, std::vector<unsigned char>& grayImage, int width, int height, int numChannels)
{
    grayImage.clear();
    grayImage.resize(static_cast<size_t>(width) * static_cast<size_t>(height));

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            int indexOfPixel = j * width + i;
            int indexOfByte = indexOfPixel * numChannels;

            auto r = static_cast<int>(image[indexOfByte + 0]);
            auto g = static_cast<int>(image[indexOfByte + 1]);
            auto b = static_cast<int>(image[indexOfByte + 2]);

            float gray = 0.2989f * r + 0.5870f * g + 0.1140f * b;
            int graylevel = static_cast<int>(gray + 0.5);
            grayImage[indexOfPixel] = graylevel;
        }
    }
}

void ImageDealer::CalcHistogram(const std::vector<unsigned char>& grayImage, std::vector<int>& histogram)
{
    histogram.clear();
    histogram.resize(256, 0);

    for(size_t i = 0; i < grayImage.size(); i++){
        int grayLevel = static_cast<int>(grayImage[i]);
        histogram[grayLevel]++;
    }
}

void ImageDealer::Equalize(const std::vector<unsigned char>& grayImage, const std::vector<int>& histogram, std::vector<unsigned char>& equalizedImage, int width, int height, int numLevels)
{
    equalizedImage.clear();
    equalizedImage.resize(grayImage.size());

    std::vector<double> probabilitiesOfGrayLevel(numLevels);
    std::vector<double> accumulativeProbalities(numLevels);
    std::vector<int> mapping(numLevels);

    for(int i = 0; i < numLevels; i++){
        probabilitiesOfGrayLevel[i] = histogram[i] * 1.0 / grayImage.size();

        if(i == 0) accumulativeProbalities[0] = probabilitiesOfGrayLevel[0];
        accumulativeProbalities[i] = accumulativeProbalities[i - 1] + probabilitiesOfGrayLevel[i];

        mapping[i] = static_cast<int>(accumulativeProbalities[i] * (numLevels - 1) + 0.5);
    }


    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            int indexOfPixel = j * width + i;
            equalizedImage[indexOfPixel] = mapping[grayImage[indexOfPixel]];
        }
    }
}
