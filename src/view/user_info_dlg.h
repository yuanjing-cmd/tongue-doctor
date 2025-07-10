#ifndef _USER_INFO_DLG_H_
#define _USER_INFO_DLG_H_

#include "NButton.h"
#include "NButtonGroup.h"
#include "NComboBox.h"
#include "NDialog.h"
#include "NFrame.h"
#include "NImage.h"
#include "NLabel.h"
#include "NLineLayout.h"
#include "NRadioButton.h"
#include "NTextEdit.h"

#include "NQRCode.h"

using namespace NeatGUI;

class UserInfoDlg : public CNDialog
{
public:
    UserInfoDlg() = default;
    ~UserInfoDlg() = default;

    virtual void OnCreate();

    void SetGender(int32_t gender);
    void SetAge(int32_t age);

    int32_t Gender();
    int32_t Age();

protected:
    void OnOKClicked(const shared_ptr<CNMessage>& pMsg);

private:
    CNLineLayout m_layout;
    CNImage m_image;
    CNLabel m_gender_label;
    CNButtonGroup m_gender_group;
    CNRadioButton m_radio_male;
    CNRadioButton m_radio_female;
    CNLabel m_age_label;
    CNComboBox m_age_combo;
    CNButton m_ok_btn;
};

#endif // _USER_INFO_DLG_H_
