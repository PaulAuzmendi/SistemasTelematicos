#include "pch.h"
#include "AccionamientoSocket.h"
#include "EsclavoAccionamientosDlg.h"
#include "Modbus.h"

struct AtenderClienteAccParams
{
    SOCKET hSocket;
    CEsclavoAccionamientosDlg* pDlg;
};

UINT AtenderClienteAcc(LPVOID lp)
{
    if (!AfxSocketInit()) return 0;

    AtenderClienteAccParams* params = (AtenderClienteAccParams*)lp;
    SOCKET hSock = params->hSocket;
    CEsclavoAccionamientosDlg* pDlg = params->pDlg;
    delete params;

    CSocket cliente;
    cliente.Attach(hSock);

    unsigned char buf[260];

    while (true)
    {
        int len = cliente.Receive(buf, sizeof(buf));
        if (len <= 0) break;

        Modbus f;
        if (!f.ParseRequest(buf, len)) continue;

        if (f.functionCode == 0x03)
        {
            unsigned short vals[125];
            for (int i = 0; i < f.quantity; i++)
            {
                unsigned short reg = f.address + i;
                if (reg == 400) vals[i] = pDlg->freno ? 1 : 0;
                else if (reg == 401) vals[i] = pDlg->izquierdo ? 1 : 0;
                else if (reg == 402) vals[i] = pDlg->derecho ? 1 : 0;
                else                 vals[i] = 0;
            }

            unsigned char resp[260];
            int n = f.BuildReadResponse(resp, vals, f.quantity);
            cliente.Send(resp, n);
        }
    }

    cliente.Close();
    return 0;
}

void AccionamientoSocket::OnAccept(int nErrorCode)
{
    CSocket cliente;
    Accept(cliente);

    SOCKET hSock = cliente.Detach();

    AtenderClienteAccParams* params = new AtenderClienteAccParams();
    params->hSocket = hSock;
    params->pDlg = pDlg;

    AfxBeginThread(AtenderClienteAcc, params);
}