#include "camera_notifier.h"

#include "logger.h"

#include <chrono>

using namespace NeatGUI;

CameraNotifier::CameraNotifier(int index, CameraObserver* observer)
    : m_observer(observer), m_timer_id(0), m_running(false), m_frame_ready(false)
{
    // m_cap.open(index);
    m_cap.open("v4l2src device=/dev/video11 ! video/x-raw,format=NV12, width=640, height=480, framerate=30/1 ! "
               "videoconvert ! appsink");
    if (m_cap.isOpened())
    {
        LOG_INFO("Opened camera [%d]", index);
    }
    else
    {
        LOG_INFO("Failed to open camera %d", index);
    }
}

CameraNotifier::~CameraNotifier()
{
    m_cap.release();
}

void CameraNotifier::Start()
{
    if (m_running)
    {
        return;
    }

    m_running = true;

    const int FRAME_RATE = 30;
    m_timer_id = CNTimer::Create(1000 / FRAME_RATE, [=]() { Run(); });

    m_read_thread = std::thread(&CameraNotifier::ReadLoop, this);
}

void CameraNotifier::Stop()
{
    if (!m_running)
    {
        return;
    }

    CNTimer::Destroy(m_timer_id);
    m_running = false;
    if (m_read_thread.joinable())
    {
        m_read_thread.join();
    }
}

bool CameraNotifier::IsRunning() const
{
    return m_running;
}

void CameraNotifier::Run()
{
    if (!m_frame_ready)
    {
        return;
    }

    cv::Mat frame;
    cv::Mat frame2;
    {
        std::lock_guard<std::mutex> lock(m_mu);
        std::swap(frame, m_converted_frame_bgra);
        std::swap(frame2, m_converted_frame_rgb);
        m_frame_ready = false;
    }

    unique_ptr<ImageBuffer> buffer(new ImageBuffer(frame));
    unique_ptr<ImageBuffer> buffer2(new ImageBuffer(frame2));
    m_observer->OnFrameCaptured(std::move(buffer), std::move(buffer2));
}

void CameraNotifier::ReadLoop()
{
    const int target_fps = 30;
    const int frame_interval = 1000 / target_fps;
    auto last_frame_time = std::chrono::steady_clock::now();
    while (m_running)
    {
        auto next_frame_time = last_frame_time + std::chrono::milliseconds(frame_interval);
        {
            cv::Mat frame;
            m_cap >> frame;
            switch (frame.type())
            {
                case CV_8UC3:
                {
                    std::lock_guard<std::mutex> lock(m_mu);
                    cv::cvtColor(frame, m_converted_frame_bgra, cv::COLOR_BGR2BGRA);
                    cv::cvtColor(frame, m_converted_frame_rgb, cv::COLOR_BGR2RGB);
                    m_frame_ready = true;
                    break;
                }
            }
        }

        auto now = std::chrono::steady_clock::now();
        if (now < next_frame_time)
        {
            std::this_thread::sleep_until(next_frame_time);
        }

        last_frame_time = std::chrono::steady_clock::now();
    }
}

ImageBuffer::ImageBuffer(cv::Mat frame) : m_frame(frame)
{
}

uchar* ImageBuffer::Data() const
{
    return m_frame.data;
}

int ImageBuffer::Width() const
{
    return m_frame.cols;
}

int ImageBuffer::Height() const
{
    return m_frame.rows;
}

int ImageBuffer::Stride() const
{
    return m_frame.step[0];
}

int ImageBuffer::Total() const
{
    return m_frame.total();
}

int ImageBuffer::ElemSize() const
{
    return m_frame.elemSize();
}
