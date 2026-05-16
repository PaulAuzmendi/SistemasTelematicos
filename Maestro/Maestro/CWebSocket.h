#pragma once
#include <afxsock.h>

class CWebSocket : public CAsyncSocket
{
public:
   virtual void OnAccept(int nErrorCode);
};