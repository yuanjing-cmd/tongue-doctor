#include "camera_reader.h"

#include <chrono>

CameraReader::CameraReader(int index)
{
    m_cap.open(index);
    if (!m_cap.isOpened())
    {
        std::cout << "Failed to open camera" << std::endl;
    }
}

CameraReader::~CameraReader()
{
    m_cap.release();
}

bool CameraReader::Read(image_buffer_t& buffer)
{
    memset(&buffer, 0, sizeof(image_buffer_t));

    cv::Mat frame;
    m_cap >> frame;
    if (frame.empty())
    {
        return false;
    }

    // 根据Mat的类型和通道数确定格式
    switch (frame.type())
    {
        case CV_8UC1:
        {
            buffer.format = IMAGE_FORMAT_GRAY8;
            break;
        }
        case CV_8UC3:
        {
            cv::Mat converted_frame;
            cv::cvtColor(frame, converted_frame, cv::COLOR_BGR2RGB);
            std::swap(frame, converted_frame);
            buffer.format = IMAGE_FORMAT_RGB888;
            break;
        }
        case CV_8UC4:
        {
            cv::Mat converted_frame;
            cv::cvtColor(frame, converted_frame, cv::COLOR_BGRA2RGBA);
            std::swap(frame, converted_frame);
            buffer.format = IMAGE_FORMAT_RGBA8888;
            break;
        }
    }

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    std::string filename = "photo_" + ss.str() + ".jpg";
    imwrite(filename, frame);

    buffer.width = frame.cols;
    buffer.height = frame.rows;
    buffer.width_stride = static_cast<int>(frame.step[0]);
    buffer.height_stride = frame.rows;
    size_t buffer_size = buffer.width_stride * buffer.height;
    buffer.virt_addr = new uchar[buffer_size];
    memcpy(buffer.virt_addr, frame.data, buffer_size);
    buffer.size = static_cast<int>(frame.total() * frame.elemSize());
    buffer.fd = -1; // 文件描述符，默认为-1

    return true;
}
