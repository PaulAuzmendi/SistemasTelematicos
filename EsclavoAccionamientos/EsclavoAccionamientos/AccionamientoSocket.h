#pragma once
#include <afxsock.h>

class CEsclavoAccionamientosDlg;


class AccionamientoSocket : public CSocket
{
public:
    CEsclavoAccionamientosDlg* pDlg;
    virtual void OnAccept(int nErrorCode);
};

