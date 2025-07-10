#ifndef _YOLO_CONTROLLER_H_
#define _YOLO_CONTROLLER_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

#include "yolov7.h"

#include "postprocess.h"

#include "camera_notifier.h"

class YoloController
{
public:
    class YoloObserver
    {
    public:
        virtual void OnYoloInferenceFinish(std::unique_ptr<ImageBuffer> buffer, std::unique_ptr<object_detect_result_list> od_results) = 0;
        virtual void OnYoloInferenceError() = 0;
    };

public:
    YoloController(rknn_app_context_t* rknn_ctx, YoloObserver* observer);
    virtual ~YoloController();

    bool Input(std::unique_ptr<ImageBuffer> buffer, std::unique_ptr<ImageBuffer> buffer2);

    void WaitFinish();

    bool GetYoloClassName(int class_id, std::string& class_name);

private:
    void WorkThread();

private:
    rknn_app_context_t* m_rknn_ctx{nullptr};
    YoloObserver* m_observer{nullptr};

    std::thread m_work_thread;

    bool m_running;
    std::mutex m_mutex;
    std::condition_variable m_cond;

    std::unique_ptr<ImageBuffer> m_buffer1;
    std::unique_ptr<ImageBuffer> m_buffer2;
};

#endif // _YOLO_CONTROLLER_H_
