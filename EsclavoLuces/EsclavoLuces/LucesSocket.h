#pragma once
#include <afxsock.h>

class CEsclavoLucesDlg;

class LucesSocket : public CSocket
{
public:
    CEsclavoLucesDlg* pDlg;
    virtual void OnAccept(int nErrorCode);
};