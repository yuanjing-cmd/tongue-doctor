#include "yolo_controller.h"

#include "logger.h"

#include <cstring>

YoloController::YoloController(rknn_app_context_t* rknn_ctx, YoloObserver* observer)
    : m_rknn_ctx(rknn_ctx), m_observer(observer)
{
    m_work_thread = std::thread(&YoloController::WorkThread, this);
}

YoloController::~YoloController()
{
    m_running = false;
    m_cond.notify_all();
    m_work_thread.join();
}

bool YoloController::Input(std::unique_ptr<ImageBuffer> buffer, std::unique_ptr<ImageBuffer> buffer2)
{
    m_buffer1 = std::move(buffer);
    m_buffer2 = std::move(buffer2);
    m_cond.notify_all();
    return true;
}

void YoloController::WaitFinish()
{
}

bool YoloController::GetYoloClassName(int class_id, std::string& class_name)
{
    class_name = coco_cls_to_name(class_id);
    return true;
}

void YoloController::WorkThread()
{
    while (m_running)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock);

        if (m_buffer2)
        {
            image_buffer_t image_buffer;
            image_buffer.format = IMAGE_FORMAT_RGB888;
            image_buffer.width = m_buffer2->Width();
            image_buffer.height = m_buffer2->Height();
            image_buffer.width_stride = m_buffer2->Stride();
            image_buffer.height_stride = m_buffer2->Height();
            image_buffer.virt_addr = m_buffer2->Data();
            image_buffer.size = static_cast<int>(m_buffer2->Total() * m_buffer2->ElemSize());
            image_buffer.fd = -1; // 文件描述符，默认为-1

            std::unique_ptr<object_detect_result_list> od_results(new object_detect_result_list);
            memset(od_results.get(), 0, sizeof(object_detect_result_list));

            int ret = inference_yolov7_model(m_rknn_ctx, &image_buffer, od_results.get());
            if (ret != 0)
            {
                LOG_ERROR("inference_yolov7_model fail! ret=%d\n", ret);
                m_observer->OnYoloInferenceError();
            }
            else
            {
                m_observer->OnYoloInferenceFinish(std::move(m_buffer1), std::move(od_results));
            }

            m_buffer2.reset();
        }
    }
}
