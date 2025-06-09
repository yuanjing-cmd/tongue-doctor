#ifndef __CAMERA_READER_H__
#define __CAMERA_READER_H__

#include "common.h"

#include "opencv2/opencv.hpp"

class CameraReader
{
public:
    CameraReader(int device_no);
    ~CameraReader();

    bool Read(image_buffer_t& buffer);

private:
    cv::VideoCapture m_cap;
};

#endif // __CAMERA_READER_H__
