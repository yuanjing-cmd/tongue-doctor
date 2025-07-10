#include "qrcode_dlg.h"

#include "color_define.h"

using namespace NeatGUI;

void QRCodeDlg::OnCreate()
{
    {
        auto dialog_style = Style();
        dialog_style.nTitleHeight = 0;
        dialog_style.nFooterHeight = 0;
        dialog_style.BackgroundColor = COLOR_MIDNIGHTBLUE;
        SetStyle(dialog_style);
    }

    SetCloseOnClickOutside(true);

    m_qrcode.Create(this);
    m_qrcode.SetAnchor(NANCHOR_CENTER);
    m_qrcode.Resize(480, 480);

    Resize(500, 500);
}

void QRCodeDlg::SetText(const std::string& text)
{
    m_qrcode.SetText(text.c_str());
}
