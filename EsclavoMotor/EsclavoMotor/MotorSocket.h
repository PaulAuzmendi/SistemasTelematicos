#pragma once
#include <afxsock.h>


class CEsclavoMotorDlg;
class MotorSocket :
    public CSocket
{
public: 
    CEsclavoMotorDlg* pDlg;  

    virtual void OnAccept(int nErrorCode);
};

