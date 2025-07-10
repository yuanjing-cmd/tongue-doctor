#ifndef _MAIN_VIEW_H_
#define _MAIN_VIEW_H_

#include "NButton.h"
#include "NDialog.h"
#include "NFrame.h"
#include "NLabel.h"

#include "NHBoxLayout.h"
#include "NImage.h"
#include "NQRCode.h"
#include "NScrollLabel.h"
#include "NVBoxLayout.h"

#include "camera_notifier.h"
#include "llm_controller.h"
#include "yolo_controller.h"

#include <atomic>

using namespace NeatGUI;

class MainView : public CNDialog,
                 public CameraNotifier::CameraObserver,
                 public YoloController::YoloObserver,
                 public LLMController::LLMObserver
{
public:
    MainView();
    ~MainView();

    virtual void OnCreate();
    virtual void OnDestroy();

    virtual void OnFrameCaptured(std::unique_ptr<ImageBuffer> buffer, std::unique_ptr<ImageBuffer> buffer2) override;

    virtual void OnYoloInferenceFinish(std::unique_ptr<ImageBuffer> buffer,
                                       std::unique_ptr<object_detect_result_list> od_results) override;
    virtual void OnYoloInferenceError() override;

    virtual void OnLLMInferenceRunning(const std::string& text) override;
    virtual void OnLLMInferenceFinish() override;
    virtual void OnLLMInferenceError() override;

private:
    void Start();
    void Stop();
    bool IsRunning();

    void OnUserInfoButtonClick(const shared_ptr<CNMessage>& msg);
    void OnRunButtonClick(const shared_ptr<CNMessage>& msg);
    void OnQRCodeButtonClick(const shared_ptr<CNMessage>& msg);

    void OnYoloTaskMsg(const shared_ptr<CNMessage>& msg);
    void OnLLMTaskMsg(const shared_ptr<CNMessage>& msg);

    void OnCursorLButtonDown(const shared_ptr<CNMessage>& msg);

    std::string ClassIDToDescription(int class_id);

    void CreateLLMTask(int class_id);

private:
    CameraNotifier m_camera_notifier;
    std::unique_ptr<YoloController> m_yolo_controller;
    std::unique_ptr<LLMController> m_llm_controller;

    int32_t m_age;
    int32_t m_gender; // 1:男, 2:女

    std::atomic<bool> m_running{false};

    CNFrame m_camera_frame;
    CNImage m_camera_image;
    CNWidget m_tongue_frame_container;
    CNFrame m_tongue_frame;

    CNButton m_user_info_button;
    CNButton m_run_button;
    CNLabel m_tongue_type_label;
    CNButton m_qrcode_button;

    CNScrollLabel m_result_label;

    std::string m_llm_text;

    // metrics
    std::chrono::steady_clock::time_point m_yolo_start_time;
    std::chrono::steady_clock::time_point m_yolo_end_time;
};

#endif // _MAIN_VIEW_H_
