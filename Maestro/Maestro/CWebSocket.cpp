#include "pch.h"
#include "CWebSocket.h"
#include "MaestroDlg.h"


void CWebSocket::OnAccept(int nErrorCode)
{
    if (nErrorCode == 0) {
        CMaestroDlg* pDlg = (CMaestroDlg*)AfxGetMainWnd();
        pDlg->OnWebAccept();
    }
    CAsyncSocket::OnAccept(nErrorCode);
}