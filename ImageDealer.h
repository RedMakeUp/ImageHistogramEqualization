#ifndef IMAGEDEALER_H
#define IMAGEDEALER_H

#include <memory>
#include <vector>

struct RawImage{
    std::vector<unsigned char> data;
    int width = 0;
    int height = 0;
    int numChannels = 0;

    void Ready(const RawImage& other, bool needResize = true){
        width = other.width;
        height = other.height;
        numChannels = other.numChannels;

        data.clear();
        if(needResize) data.resize(other.data.size());
    }

    void SetGray(int i, int j, int gray){
        data[j * width + i] = static_cast<unsigned char>(gray);
    }

    int GetGray(int i, int j) const{
        return static_cast<int>(data[j * width + i]);
    }
};

class ImageDealer{
public:
    // Grayscale an image.
    // Assume that the memory is row-major and channels(if exists) in each pixel are ordered as following:
    //     red, green, blue, alpha
    // The gray's calculation equation is
    //     gray = 0.2989 * red + 0.5870 * green + 0.1140 * blue
    static std::shared_ptr<RawImage> GrayScale(const std::shared_ptr<RawImage> image);

    // Calculate occurrence of each gray level, namely histogram of the image
    static void CalcHistogram(const std::vector<unsigned char>& grayImage, std::vector<int>& histogram);

    // Equalize histogram of the gray image
    static std::shared_ptr<RawImage> Equalize(const std::shared_ptr<RawImage> grayImage, int numLevels = 256);

    // Flip an image. Set @verticalFlip to true for vertically flip, false for horizontally flip
    static std::shared_ptr<RawImage> Mirror(const std::shared_ptr<RawImage> image, bool verticalFlip = true);

    // Weight all pixels under @filter to the center pixel. The shape of @filter must be @filterShape * @filterShape.
    // If set @divideBySum to true, @filter will be normalized before being applied
    static std::shared_ptr<RawImage> WeightedFilter(const std::shared_ptr<RawImage> image, std::vector<float> filter, int filterShape, bool divideBySum = true);

    // Set the median of all pixels under @filter to the center pixel
    static std::shared_ptr<RawImage> MedianFilter(const std::shared_ptr<RawImage> image, int filterShape);

    // Set the max value of all pixels under @filter to the center pixel
    static std::shared_ptr<RawImage> MaxFilter(const std::shared_ptr<RawImage> image, int filterShape);

    static std::shared_ptr<RawImage> LoadRawImage(const char* fileName);
};

#endif // IMAGEDEALER_H
