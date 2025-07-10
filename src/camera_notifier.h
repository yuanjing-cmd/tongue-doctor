#ifndef __CAMERA_NOTIFIER_H__
#define __CAMERA_NOTIFIER_H__

#include "NTimer.h"

#include "common.h"

#include "opencv2/opencv.hpp"

#include <thread>

class ImageBuffer
{
public:
    ImageBuffer(cv::Mat frame);
    ~ImageBuffer() = default;

    uchar* Data() const;
    int Width() const;
    int Height() const;

    int Stride() const;
    int Total() const;
    int ElemSize() const;

private:
    cv::Mat m_frame;
};

class CameraNotifier
{
public:
    class CameraObserver
    {
    public:
        virtual void OnFrameCaptured(std::unique_ptr<ImageBuffer> buffer, std::unique_ptr<ImageBuffer> buffer2) = 0;
    };

public:
    CameraNotifier(int index, CameraObserver* observer);
    ~CameraNotifier();

    void Start();
    void Stop();

    bool IsRunning() const;

private:
    void Run();
    void ReadLoop();

private:
    cv::VideoCapture m_cap;
    CameraObserver* m_observer;
    tuint32 m_timer_id;
    bool m_running;
    std::thread m_read_thread;

    std::mutex m_mu;
    cv::Mat m_converted_frame_bgra;
    cv::Mat m_converted_frame_rgb;
    bool m_frame_ready;
};

#endif // __CAMERA_NOTIFIER_H__
