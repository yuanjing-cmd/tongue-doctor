#include "main_view.h"

#include "NApplication.h"
#include "NDisplay.h"
#include "NFont.h"
#include "NCursor.h"
#include "NPixmap.h"

#include "color_define.h"
#include "qrcode_dlg.h"
#include "user_info_dlg.h"
#include "view_msg.h"

#include "camera_reader.h"
#include "logger.h"
#include "model_manager.h"

MainView::MainView() : m_camera_notifier(11, this)
{
    m_yolo_controller = ModelManager::Inst().CreateYoloController(this);
    m_llm_controller = ModelManager::Inst().CreateLLMController(this);
}

MainView::~MainView()
{
}

void MainView::OnCreate()
{
    {
        auto dialog_style = Style();
        dialog_style.nTitleHeight = 0;
        dialog_style.nFooterHeight = 0;
        dialog_style.BackgroundColor = COLOR_WINDOW;
        SetStyle(dialog_style);
    }

    auto button_font = Font();
    button_font.SetSize(24);

    m_camera_frame.Create(this);
    m_camera_frame.SetPos(19, 19);
    m_camera_frame.Resize(642, 482);
    {
        auto frame_style = m_camera_frame.Style();
        frame_style.BorderColor = COLOR_CLOUDS;
        frame_style.BackgroundColor = NCOLOR32(0, 0, 0);
        m_camera_frame.SetStyle(frame_style);
    }

    m_camera_image.Create(this);
    m_camera_image.SetPos(20, 20);
    m_camera_image.Resize(640, 480);

    m_tongue_frame.Create(&m_camera_image);
    auto frame_style = m_tongue_frame.Style();
    frame_style.BackgroundColor = NCOLOR32(0, 0, 0, 0);
    frame_style.BorderColor = COLOR_ALIZARIN;
    frame_style.nBorderWidth = 2;
    frame_style.nBorderRadius = 5;
    m_tongue_frame.SetStyle(frame_style);

    m_user_info_button.Create(this);
    m_user_info_button.SetPos(20, 520);
    m_user_info_button.Resize(145, 60);
    m_user_info_button.SetFont(button_font);
    m_user_info_button.SetText("用户信息");

    m_run_button.Create(this);
    m_run_button.SetPos(185, 520);
    m_run_button.Resize(145, 60);
    m_run_button.SetFont(button_font);

    m_qrcode_button.Create(this);
    m_qrcode_button.SetPos(350, 520);
    m_qrcode_button.Resize(145, 60);
    m_qrcode_button.SetText("查看二维码");
    m_qrcode_button.SetFont(button_font);
    m_qrcode_button.SetEnabled(false);

    m_tongue_type_label.Create(this);
    m_tongue_type_label.SetPos(515, 520);
    m_tongue_type_label.Resize(145, 60);
    m_tongue_type_label.SetFont(button_font);
    {
        auto style = m_tongue_type_label.Style();
        style.dwAlignment = NALIGN_CENTER;
        style.BorderColor = COLOR_CLOUDS;
        style.nBorderWidth = 1;
        style.nBorderRadius = 5;
        m_tongue_type_label.SetStyle(style);
    }

    m_result_label.Create(this);
    m_result_label.SetPos(680, 20);
    m_result_label.Resize(324, 560);
    {
        auto style = m_result_label.Style();
        style.Label.BackgroundColor = NCOLOR32(0, 0, 0, 0);
        style.Label.BorderColor = COLOR_CLOUDS;
        style.Label.nBorderWidth = 1;
        style.Label.nBorderRadius = 5;
        m_result_label.SetStyle(style);
    }

    NGUI_ON_BUTTON_CLICKED(&m_user_info_button, &MainView::OnUserInfoButtonClick);
    NGUI_ON_BUTTON_CLICKED(&m_run_button, &MainView::OnRunButtonClick);
    NGUI_ON_BUTTON_CLICKED(&m_qrcode_button, &MainView::OnQRCodeButtonClick);
    NGUI_ON_OTHER_MESSAGE_NATIVE(this, VIEW_MSG_ID_YOLO_RESULT, &MainView::OnYoloTaskMsg);
    NGUI_ON_OTHER_MESSAGE_NATIVE(this, VIEW_MSG_ID_LLM_RESULT, &MainView::OnLLMTaskMsg);

    CNDisplay* pDisplay = CNApplication::PrimaryDisplay();
    Resize(pDisplay->Width(), pDisplay->Height());
    LOG_INFO("MainView width=%d, height=%d", Width(), Height());

    UserInfoDlg dlg;
    dlg.Exec();
    m_gender = dlg.Gender();
    m_age = dlg.Age();

    Stop();
}

void MainView::OnDestroy()
{
    Stop();
}

void MainView::OnFrameCaptured(std::unique_ptr<ImageBuffer> buffer, std::unique_ptr<ImageBuffer> buffer2)
{
    if (!IsRunning())
    {
        return;
    }

    CNPixmap pixmap(buffer->Data(), static_cast<tuint32>(buffer->Width()), static_cast<tuint32>(buffer->Height()),
                    CNPixmap::RAW_FORMAT::BGRA8888);
    m_camera_image.SetPixmap(pixmap);

    m_yolo_start_time = std::chrono::steady_clock::now();
    m_yolo_controller->Input(std::move(buffer), std::move(buffer2));
}

void MainView::OnYoloInferenceFinish(std::unique_ptr<ImageBuffer> buffer,
                                     std::unique_ptr<object_detect_result_list> od_results)
{
    if (!IsRunning())
    {
        return;
    }


    TaskResult result;

    int max_prop_index = -1;
    float max_prop = 0;
    for (int i = 0; i < od_results->count; i++)
    {
        object_detect_result* det_result = &(od_results->results[i]);
        if (det_result->prop > max_prop)
        {
            max_prop_index = i;
            max_prop = det_result->prop;
        }
    }

    if (max_prop_index < 0)
    {
        result.code = TASK_RESULT_YOLO_NO_TONGUE;
        PostMsg(this, make_shared<TaskMsg>(VIEW_MSG_ID_YOLO_RESULT, result));
        return;
    }

    object_detect_result* det_result = &(od_results->results[max_prop_index]);
    if (det_result->prop < 0.80f)
    {
        result.code = TASK_RESULT_YOLO_NO_TONGUE;
        PostMsg(this, make_shared<TaskMsg>(VIEW_MSG_ID_YOLO_RESULT, result));
        return;
    }

    m_yolo_end_time = std::chrono::steady_clock::now();
    auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(m_yolo_end_time - m_yolo_start_time);
    LOG_INFO("yolo cost time: %ldms", diff_ms.count());
    Stop();

    std::string class_name;
    m_yolo_controller->GetYoloClassName(det_result->cls_id, class_name);

    result.code = TASK_RESULT_YOLO_OK;
    result.tongue_prop = det_result->prop;
    result.tongue_class_id = det_result->cls_id;
    result.tongue_type_name = class_name;
    result.tongue_area =
        NAREA(det_result->box.left, det_result->box.top, det_result->box.right, det_result->box.bottom);
    auto task_msg = make_shared<TaskMsg>(VIEW_MSG_ID_YOLO_RESULT, result);
    task_msg->SetBuffer(std::move(buffer));
    PostMsg(this, task_msg);


    LOG_INFO("%s prop=%.3f", class_name.c_str(), det_result->prop);
} 

void MainView::OnYoloInferenceError()
{
    TaskResult result;
    result.code = TASK_RESULT_YOLO_ERROR;
    PostMsg(this, make_shared<TaskMsg>(VIEW_MSG_ID_YOLO_RESULT, result));
}

void MainView::OnLLMInferenceRunning(const std::string& text)
{
    TaskResult result;
    result.code = TASK_RESULT_LLM_RUNNING;
    result.llm_reply = text;
    PostMsg(this, make_shared<TaskMsg>(VIEW_MSG_ID_LLM_RESULT, result));
}

void MainView::OnLLMInferenceFinish()
{
    TaskResult result;
    result.code = TASK_RESULT_LLM_FINISH;
    PostMsg(this, make_shared<TaskMsg>(VIEW_MSG_ID_LLM_RESULT, result));
}

void MainView::OnLLMInferenceError()
{
    TaskResult result;
    result.code = TASK_RESULT_LLM_ERROR;
    PostMsg(this, make_shared<TaskMsg>(VIEW_MSG_ID_LLM_RESULT, result));
}

void MainView::Start()
{
    m_running.store(true);

    m_camera_notifier.Start();

    m_run_button.SetText("停止检测");
    m_qrcode_button.SetEnabled(false);
    m_llm_text.clear();
}

void MainView::Stop()
{
    m_running.store(false);

    m_camera_notifier.Stop();
    m_llm_controller->Abort();

    m_run_button.SetText("开始检测");
    m_run_button.SetEnabled(true);  // 确保按钮可点击
   
}

bool MainView::IsRunning()
{
    return m_running.load();
}

void MainView::OnUserInfoButtonClick(const shared_ptr<CNMessage>& msg)
{
    UserInfoDlg dlg;
    dlg.SetGender(m_gender);
    dlg.SetAge(m_age);
    dlg.Exec();

    m_gender = dlg.Gender();
    m_age = dlg.Age();
}

void MainView::OnRunButtonClick(const shared_ptr<CNMessage>& msg)
{
    if (IsRunning())
    {
        Stop();
    }
    else
    {
        Start();
    }
}

void MainView::OnQRCodeButtonClick(const shared_ptr<CNMessage>& msg)
{
    QRCodeDlg dlg;
    std::string text = "检测结果：";
    text += m_tongue_type_label.Text();
    text += "\n";
    text += m_result_label.Text();
    dlg.SetText(text.c_str());
    dlg.Exec();
}

void MainView::OnYoloTaskMsg(const shared_ptr<CNMessage>& msg)
{
    auto task_msg = static_pointer_cast<TaskMsg>(msg);
    const TaskResult& result = task_msg->Result();
    switch (result.code)
    {
        case TASK_RESULT_YOLO_NO_TONGUE:
        {
            m_tongue_frame.Show(false);
            break;
        }
        case TASK_RESULT_YOLO_OK:
        {
            const std::unique_ptr<ImageBuffer>& buffer = task_msg->Buffer();
            CNPixmap pixmap(buffer->Data(), static_cast<tuint32>(buffer->Width()),
                            static_cast<tuint32>(buffer->Height()), CNPixmap::RAW_FORMAT::BGRA8888);
            m_camera_image.SetPixmap(pixmap);

            m_tongue_frame.SetGeometry(result.tongue_area);
            m_tongue_frame.Show(true);

            std::string tongue_description = ClassIDToDescription(result.tongue_class_id);
            m_tongue_type_label.SetText(tongue_description.c_str());

            m_result_label.SetText("正在生成调理建议，请稍等...");

            CreateLLMTask(result.tongue_class_id);
            break;
        }
        default:
            break;
    }
}

std::string trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    size_t end = s.find_last_not_of(" \t\n\r\f\v");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::string removeStars(std::string s)
{
    s.erase(std::remove(s.begin(), s.end(), '*'), s.end());
    return s;
}

std::string resultText(const std::string& llm_text)
{
    const std::string THINK_END = "</think>";
    auto pos = llm_text.find(THINK_END);
    if (pos == std::string::npos)
    {
        return "";
    }

    std::string content = llm_text.substr(pos + THINK_END.length());
    content = trim(content);
    content = removeStars(content);
    return content;
}

void MainView::OnLLMTaskMsg(const shared_ptr<CNMessage>& msg)
{
    auto task_msg = static_pointer_cast<TaskMsg>(msg);
    const TaskResult& result = task_msg->Result();
    switch (result.code)
    {
        case TASK_RESULT_LLM_RUNNING:
        {
            m_llm_text += result.llm_reply;
            std::string short_result = resultText(m_llm_text);
            if (!short_result.empty())
            {
                m_result_label.SetText(short_result.c_str());
            }
            printf("%s", result.llm_reply.c_str());
            break;
        }
        case TASK_RESULT_LLM_FINISH:
        {
            m_qrcode_button.SetEnabled(true);
            break;
        }
        case TASK_RESULT_LLM_ERROR:
        {
            m_result_label.SetText("");
            break;
        }
        default:
            break;
    }
}

void MainView::OnCursorLButtonDown(const shared_ptr<CNMessage>& msg)
{
    CNApplication::PrimaryDisplay()->Dump();
}

std::string MainView::ClassIDToDescription(int class_id)
{
    const std::map<int, std::string> class_id_to_description = {
        {0, "舌苔鲜红"}, {1, "舌苔白而薄"}, {2, "舌苔白而厚"}, {3, "舌苔黄而腻"}, {4, "舌苔灰黑"},
    };

    auto it = class_id_to_description.find(class_id);
    if (it != class_id_to_description.end())
    {
        return it->second;
    }

    return "";
}

void MainView::CreateLLMTask(int class_id)
{
    std::string tongue_description = ClassIDToDescription(class_id);
    std::string gender = m_gender == 1 ? "男" : "女";
    std::string age = std::to_string(m_age);
    std::string input_format = "性别%s, 年龄%s,%s,  中医有什么食补建议？";

    // std::string input_text = "用户是" + gender + "，" + age + "岁，舌象为[" + tongue_description + "]。" 
    //                        + "请给出针对此舌象的中医食补建议及调理建议，回答时只需返回建议内容，不要包含任何标签或日志信息。";
    // LOG_INFO("LLM输入: %s", input_text.c_str());
    // m_llm_controller->Input(input_text);

    char input_text[1024 * 5];
    snprintf(input_text, sizeof(input_text), input_format.c_str(), gender.c_str(), age.c_str(),
             tongue_description.c_str());
    LOG_INFO("input_text: %s", input_text);

    m_llm_controller->Input(input_text);
}
