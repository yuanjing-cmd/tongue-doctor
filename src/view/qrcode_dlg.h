#ifndef _QRCODE_DLG_H_
#define _QRCODE_DLG_H_

#include "NButton.h"
#include "NDialog.h"
#include "NLabel.h"
#include "NFrame.h"

#include "NQRCode.h"

using namespace NeatGUI;

class QRCodeDlg : public CNDialog
{
public:
    QRCodeDlg() = default;
    ~QRCodeDlg() = default;

    virtual void OnCreate();

    void SetText(const std::string& text);

private:
    CNQRCode m_qrcode;
};

#endif // _QRCODE_DLG_H_
