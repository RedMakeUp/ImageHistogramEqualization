#ifndef IMAGEDEALER_H
#define IMAGEDEALER_H

#include <memory>
#include <vector>

class ImageDealer{
public:
    // Grayscale an image.
    // Assume that the memory is row-major and channels(if exists) in each pixel are ordered as following:
    //     red, green, blue, alpha
    // The gray's calculation equation is
    //     gray = 0.2989 * red + 0.5870 * green + 0.1140 * blue
    //
    // @image        the original image
    // @grayImge     the result image
    // @width        width of original image
    // @height       height of original image
    // @numChannels  number of channels
    static void GrayScale(const std::vector<unsigned char>& image, std::vector<unsigned char>& grayImage,int width, int height, int numChannels = 3);

    // Calculate occurrence of each gray level, namely histogram of the image
    //
    // @grayImage    the image with only one gray channel
    // @histogram    result array
    static void CalcHistogram(const std::vector<unsigned char>& grayImage, std::vector<int>& histogram);

    // Equalize histogram of the gray image
    //
    // @grayImage       the image with only one gray channel
    // @histogram       histogram array of the gray image
    // @equalizedImage  result image
    // @width           width of original image
    // @height          height of original image
    // @numLevels       number of gray levels
    static void Equalize(const std::vector<unsigned char>& grayImage, const std::vector<int>& histogram, std::vector<unsigned char>& equalizedImage,int width, int height, int numLevels = 256);
};

#endif // IMAGEDEALER_H
