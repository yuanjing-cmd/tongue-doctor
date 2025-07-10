#include "user_info_dlg.h"

#include "NApplication.h"
#include "NDisplay.h"
#include "NFont.h"

#include "color_define.h"

using namespace NeatGUI;

void UserInfoDlg::OnCreate()
{
    {
        auto dialog_style = Style();
        dialog_style.nTitleHeight = 60;
        dialog_style.nFooterHeight = 0;
        dialog_style.BackgroundColor = COLOR_WINDOW;
        // dialog_style.TitleBackgroundColor = COLOR_BELIZEHOLD;
        SetStyle(dialog_style);
    }

    {
        auto font = Font();
        font.SetSize(32);
        SetFont(font);
    }

    SetTitle("智能舌诊系统及 AI 辅助诊断系统");

    m_image.Create(this);
    m_image.Resize(240, 240);
    m_image.SetAnchor(NANCHOR_TOP_MID);
    m_image.SetPos(0, 80);
    m_image.SetAlignMode(CNImage::IMAGE_ALIGN_MODE::KEEP_ASPECT_RATIO);
    m_image.SetImage("./skin/tongue.png");

    m_gender_label.Create(this);
    m_gender_label.Resize(80, 34);
    m_gender_label.SetText("性别");

    auto style = m_gender_label.Style();
    style.dwAlignment = NALIGN_CENTER;
    m_gender_label.SetStyle(style);

    m_radio_male.Create(this);
    m_radio_male.Resize(80, 34);
    m_radio_male.SetText("男");

    m_radio_female.Create(this);
    m_radio_female.Resize(80, 34);
    m_radio_female.SetText("女");

    m_gender_group.AddButton(&m_radio_male);
    m_gender_group.AddButton(&m_radio_female);
    m_radio_male.SetChecked(true);

    m_age_label.Create(this);
    m_age_label.Resize(80, 34);
    m_age_label.SetText("年龄");
    m_age_label.SetStyle(style);

    // m_age_edit.Create(this);
    // m_age_edit.Resize(200, 34);
    // m_age_edit.SetRange(0, 200, false);
    // m_age_edit.SetPlaceholderText("请输入年龄");

    m_age_combo.Create(this);
    m_age_combo.Resize(180, 34);
    for (int i = 1; i < 100; i++)
    {
        m_age_combo.AddItem(std::to_string(i).c_str());
    }
    m_age_combo.SetCurrentRow(17);

    m_ok_btn.Create(this);
    m_ok_btn.Resize(180, 40);
    m_ok_btn.SetText("确定");

    CNDisplay* pDisplay = CNApplication::PrimaryDisplay();
    Resize(pDisplay->Width(), pDisplay->Height());

    m_layout.SetParent(this);
    m_layout.SetAnchor(NANCHOR_BOTTOM_MID);
    m_layout.SetPos(0, -60);
    m_layout.SetPadding(20, 10);
    m_layout.AddWidget(&m_gender_label);
    m_layout.AddWidget(&m_radio_male);
    m_layout.AddWidget(&m_radio_female);
    m_layout.AddLine();
    m_layout.AddWidget(&m_age_label);
    m_layout.AddWidget(&m_age_combo);
    m_layout.AddLine(20);
    m_layout.AddSpace(80);
    m_layout.AddWidget(&m_ok_btn);

    NGUI_ON_BUTTON_CLICKED(&m_ok_btn, &UserInfoDlg::OnOKClicked);
}

void UserInfoDlg::SetGender(int32_t gender)
{
    m_radio_male.SetChecked(gender == 1);
    m_radio_female.SetChecked(gender == 2);
}

void UserInfoDlg::SetAge(int32_t age)
{
    m_age_combo.SetCurrentRow(age - 1);
}

int32_t UserInfoDlg::Gender()
{
    return m_radio_male.IsChecked() ? 1 : 2;
}

int32_t UserInfoDlg::Age()
{
    auto age = std::string(m_age_combo.CurrentText());
    return age.empty() ? 0 : std::stoi(age);
}

void UserInfoDlg::OnOKClicked(const shared_ptr<CNMessage>& pMsg)
{
    Accept();
}
