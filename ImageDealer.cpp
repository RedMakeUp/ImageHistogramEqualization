#include "ImageDealer.h"

#include <vector>
#include <algorithm>

#include "stb_image.h"
#include "stb_image_write.h"

std::shared_ptr<RawImage> ImageDealer::GrayScale(const std::shared_ptr<RawImage> image)
{
    auto grayImage = std::make_shared<RawImage>();

    grayImage->width = image->width;
    grayImage->height = image->height;
    grayImage->numChannels = 1;
    grayImage->data.resize(static_cast<size_t>(grayImage->width) * grayImage->height);

    for(int i = 0; i < image->width; i++){
        for(int j = 0; j < image->height; j++){
            int indexOfPixel = j * image->width + i;
            int indexOfByte = indexOfPixel * image->numChannels;

            auto r = static_cast<int>(image->data[indexOfByte + 0]);
            auto g = static_cast<int>(image->data[indexOfByte + 1]);
            auto b = static_cast<int>(image->data[indexOfByte + 2]);

            float gray = 0.2989f * r + 0.5870f * g + 0.1140f * b;
            int graylevel = static_cast<int>(gray + 0.5);

            grayImage->data[indexOfPixel] = graylevel;
        }
    }

    return grayImage;
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

std::shared_ptr<RawImage> ImageDealer::Equalize(const std::shared_ptr<RawImage> grayImage, int numLevels)
{
    // Result to return
    std::shared_ptr<RawImage> result = std::make_shared<RawImage>();
    // Gray histogram of origin gray image
    std::vector<int> histogram;

    // Occurrence probability of each gray level
    std::vector<double> probabilitiesOfGrayLevel(numLevels);
    // Accumulative sum of the probability
    std::vector<double> accumulativeProbalities(numLevels);
    // Mapping from a gray level to another gray level
    std::vector<int> mapping(numLevels);


    // Calculate gray hisrogram first
    CalcHistogram(grayImage->data, histogram);

    // Calculate the mapping
    for(int i = 0; i < numLevels; i++){
        probabilitiesOfGrayLevel[i] = histogram[i] * 1.0 / grayImage->data.size();

        if(i == 0) accumulativeProbalities[0] = probabilitiesOfGrayLevel[0];
        accumulativeProbalities[i] = accumulativeProbalities[i - 1] + probabilitiesOfGrayLevel[i];

        mapping[i] = static_cast<int>(accumulativeProbalities[i] * (numLevels - 1) + 0.5);
    }


    result->Ready(*grayImage);
    for(int i = 0; i < result->width; i++){
        for(int j = 0; j < result->height; j++){
            result->SetGray(i, j, mapping[grayImage->GetGray(i, j)]);
        }
    }

    return result;
}

std::shared_ptr<RawImage> ImageDealer::Mirror(const std::shared_ptr<RawImage> image, bool verticalFlip)
{
    auto result = std::make_shared<RawImage>();

    result->Ready(*image);

    // Left-right mirror
    if(verticalFlip){
        for(int j = 0; j < result->height; j++){
            for(int i = 0; i < result->width; i++){
                int dstIndexOfPixel = j * result->width + i;
                int srcIndexOfPixel = (j + 1) * result->width -1 - i;
                int dstIndexOfByte = dstIndexOfPixel * result->numChannels;
                int srcIndexOfByte = srcIndexOfPixel * result->numChannels;

                for(int k = 0; k < result->numChannels; k++){
                    result->data[dstIndexOfByte + k] = image->data[srcIndexOfByte + k];
                }
//                result->data[dstIndexOfByte + 0] = image->data[srcIndexOfByte + 0];
//                result->data[dstIndexOfByte + 1] = image->data[srcIndexOfByte + 1];
//                result->data[dstIndexOfByte + 2] = image->data[srcIndexOfByte + 2];
            }
        }
    }
    // Up-down mirror
    else{
        for(int j = 0; j < result->width; j++){
            for(int i = 0; i < result->height; i++){
                int dstIndexOfPixel = i * result->width + j;
                int srcIndexOfPixel = (result->height - 1 - i) * result->width + j;
                int dstIndexOfByte = dstIndexOfPixel * result->numChannels;
                int srcIndexOfByte = srcIndexOfPixel * result->numChannels;

                for(int k = 0; k < result->numChannels; k++){
                    result->data[dstIndexOfByte + k] = image->data[srcIndexOfByte + k];
                }
//                result->data[dstIndexOfByte + 0] = image->data[srcIndexOfByte + 0];
//                result->data[dstIndexOfByte + 1] = image->data[srcIndexOfByte + 1];
//                result->data[dstIndexOfByte + 2] = image->data[srcIndexOfByte + 2];
            }
        }
    }

    return result;
}

std::shared_ptr<RawImage> ImageDealer::WeightedFilter(const std::shared_ptr<RawImage> image, std::vector<float> filter, int filterShape, bool divideBySum)
{
    // Only deal with gray image
    if(image->numChannels != 1) return nullptr;
    // Do not deal with images with too small size
    if(image->data.size() < filter.size()) return nullptr;
    // Require filter shape to be n*n
    if(filter.size() != static_cast<size_t>(filterShape) * filterShape) return nullptr;

    auto result = std::make_shared<RawImage>();
    int halfShape = filterShape / 2;

    if(divideBySum){
        float sum = 0.0f;
        for(const auto num: filter) sum += num;

        if(sum - 0.0f < 0.000001f) return nullptr;

        for(auto& num: filter) num /= sum;
    }

    // Ready to receive data
    result->Ready(*image);

    // Weight for every pixel with the filter
    for(int j = 0; j < result->height; j++){
        for(int i = 0; i < result->width; i++){
            if(i < halfShape || j < halfShape){
                result->SetGray(i, j, image->GetGray(i, j));
                continue;
            }

            int minX = i - halfShape;
            int minY = j - halfShape;
            float sum = 0.0f;

            for(size_t k = 0; k < filter.size(); k++){
                int offsetX = k % filterShape;
                int offsetY = k / filterShape;
                int x = minX + offsetX;
                int y = minY + offsetY;
                sum += image->GetGray(x, y) * filter[k];
            }

            result->SetGray(i, j, static_cast<int>(sum + 0.5f));
        }
    }

    return result;
}

std::shared_ptr<RawImage> ImageDealer::MedianFilter(const std::shared_ptr<RawImage> image, int filterShape)
{
    // Only deal with gray image
    if(image->numChannels != 1) return nullptr;
    // Do not deal with images with too small size
    if(image->data.size() < static_cast<size_t>(filterShape) * filterShape) return nullptr;

    auto result = std::make_shared<RawImage>();
    int halfShape = filterShape / 2;
    int filterSize = filterShape * filterShape;
    int halfSize = filterSize / 2;
    std::vector<int> buffer(filterSize);

    // Ready to receive data
    result->Ready(*image);

    // Find the median of pixels under the filter
    for(int j = 0; j < result->height; j++){
        for(int i = 0; i < result->width; i++){
            if(i < halfShape || j < halfShape){
                result->SetGray(i, j, image->GetGray(i, j));
                continue;
            }

            int minX = i - halfShape;
            int minY = j - halfShape;

            for(int k = 0; k < filterSize; k++){
                int offsetX = k % filterShape;
                int offsetY = k / filterShape;
                int x = minX + offsetX;
                int y = minY + offsetY;
                buffer[k] = image->GetGray(x, y);
            }

            std::nth_element(buffer.begin(), buffer.begin() + halfSize, buffer.end());

            result->SetGray(i, j, buffer[halfSize]);
        }
    }

    return result;
}

std::shared_ptr<RawImage> ImageDealer::MaxFilter(const std::shared_ptr<RawImage> image, int filterShape)
{
    // Only deal with gray image
    if(image->numChannels != 1) return nullptr;
    // Do not deal with images with too small size
    if(image->data.size() < static_cast<size_t>(filterShape) * filterShape) return nullptr;

    auto result = std::make_shared<RawImage>();
    int halfShape = filterShape / 2;
    int filterSize = filterShape * filterShape;

    // Ready to receive data
    result->Ready(*image);

    // Find the max value of pixels under the filter
    for(int j = 0; j < result->height; j++){
        for(int i = 0; i < result->width; i++){
            if(i < halfShape || j < halfShape){
                result->SetGray(i, j, image->GetGray(i, j));
                continue;
            }

            int minX = i - halfShape;
            int minY = j - halfShape;
            int maxValue = -1;

            for(int k = 0; k < filterSize; k++){
                int offsetX = k % filterShape;
                int offsetY = k / filterShape;
                int x = minX + offsetX;
                int y = minY + offsetY;
                if(image->GetGray(x, y) > maxValue) maxValue = image->GetGray(x, y);
            }

            result->SetGray(i, j, maxValue);
        }
    }

    return result;
}

std::shared_ptr<RawImage> ImageDealer::LoadRawImage(const char* fileName)
{
    std::shared_ptr<RawImage> rawImage = nullptr;

    int width;
    int height;
    int numChannels;
    auto data = stbi_load(fileName, &width, &height, &numChannels, 0);
    if(data){
        rawImage = std::make_shared<RawImage>();

        rawImage->width = width;
        rawImage->height = height;
        rawImage->numChannels = numChannels;

        rawImage->data.clear();
        rawImage->data.resize(static_cast<size_t>(width) * height * numChannels);
        for(size_t i = 0; i < rawImage->data.size(); i++) rawImage->data[i] = data[i];

        stbi_image_free(data);
    }

    return rawImage;
}
