#ifndef _VIEW_MSG_H_
#define _VIEW_MSG_H_

#include "NVariant.h"
#include "NeatGUI.h"
#include "message.h"

using namespace NeatGUI;

class ImageBuffer;

enum VIEW_MSG_ID : tuint16
{
    VIEW_MSG_ID_BASE = static_cast<VIEW_MSG_ID>(MsgID::BUTT),
    VIEW_MSG_ID_FRAME_CAPTURED,
    VIEW_MSG_ID_YOLO_RESULT,
    VIEW_MSG_ID_LLM_RESULT,
};

enum TASK_RESULT_CODE : tuint32
{
    TASK_RESULT_YOLO_OK,
    TASK_RESULT_YOLO_NO_TONGUE,
    TASK_RESULT_YOLO_ERROR,
    TASK_RESULT_LLM_RUNNING,
    TASK_RESULT_LLM_FINISH,
    TASK_RESULT_LLM_ERROR,
    TASK_RESULT_FAIL,
};

struct TaskResult
{
    TASK_RESULT_CODE code{TASK_RESULT_FAIL};
    float tongue_prop{0};
    NAREA tongue_area{};
    int32_t tongue_class_id{-1};
    std::string tongue_type_name{};
    std::string llm_reply{};
};

class DataMsg : public CNMessage
{
public:
    DataMsg(VIEW_MSG_ID id, CNVariant data) : CNMessage(static_cast<MsgID>(id)), m_data(data)
    {
    }
    ~DataMsg() = default;

    const CNVariant& Data()
    {
        return m_data;
    };

private:
    CNVariant m_data;
};

class TaskMsg : public CNMessage
{
public:
    TaskMsg(VIEW_MSG_ID id, TaskResult task_result) : CNMessage(static_cast<MsgID>(id)), m_result(task_result)
    {
    }
    ~TaskMsg() = default;

    const TaskResult& Result()
    {
        return m_result;
    };

    const unique_ptr<ImageBuffer>& Buffer()
    {
        return m_buffer;
    }

    void SetBuffer(unique_ptr<ImageBuffer> buffer)
    {
        m_buffer = std::move(buffer);
    }

private:
    TaskResult m_result;
    std::unique_ptr<ImageBuffer> m_buffer;
};

#endif // _VIEW_MSG_H_
