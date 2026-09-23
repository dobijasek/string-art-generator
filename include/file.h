#pragma once
#include <opencv2/opencv.hpp>
#include <fstream>

inline bool writeRawImage(const cv::Mat& image, const std::string& filename)
{
    ofstream file;
    file.open (filename, ios::out|ios::binary);
    if (!file.is_open())
        return false;
    file.write(reinterpret_cast<const char *>(&image.rows), sizeof(int));
    file.write(reinterpret_cast<const char *>(&image.cols), sizeof(int));
    const int depth = image.depth();
    const int type  = image.type();
    const int channels = image.channels();
    file.write(reinterpret_cast<const char *>(&depth), sizeof(depth));
    file.write(reinterpret_cast<const char *>(&type), sizeof(type));
    file.write(reinterpret_cast<const char *>(&channels), sizeof(channels));
    int sizeInBytes = image.step[0] * image.rows;
    file.write(reinterpret_cast<const char *>(&sizeInBytes), sizeof(int));
    file.write(reinterpret_cast<const char *>(image.data), sizeInBytes);
    file.close();
    return true;
}

inline bool readRawImage(cv::Mat& image, const std::string& filename)
{
    int rows, cols, data, depth, type, channels;
    ifstream file (filename, ios::in|ios::binary);
    if (!file.is_open())
        return false;
    try {
        file.read(reinterpret_cast<char *>(&rows), sizeof(rows));
        file.read(reinterpret_cast<char *>(&cols), sizeof(cols));
        file.read(reinterpret_cast<char *>(&depth), sizeof(depth));
        file.read(reinterpret_cast<char *>(&type), sizeof(type));
        file.read(reinterpret_cast<char *>(&channels), sizeof(channels));
        file.read(reinterpret_cast<char *>(&data), sizeof(data));
        image = cv::Mat(rows, cols, type);
        file.read(reinterpret_cast<char *>(image.data), data);
    } catch (...) {
        file.close();
        return false;
    }

    file.close();
    return true;
}