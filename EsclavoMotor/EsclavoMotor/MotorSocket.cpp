#include "pch.h"
#include "MotorSocket.h"
#include "EsclavoMotorDlg.h"
#include "Modbus.h"


// Estructura para pasar varios datos al hilo
struct AtenderClienteParams
{
    SOCKET hSocket;
    CEsclavoMotorDlg* pDlg;
};

UINT AtenderCliente(LPVOID lp)
{
    if (!AfxSocketInit()) return 0;

    AtenderClienteParams* params = (AtenderClienteParams*)lp;
    SOCKET hSock = params->hSocket;
    CEsclavoMotorDlg* pDlg = params->pDlg;
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
                if (reg == 400) vals[i] = (unsigned short)pDlg->m_temp;
                else if (reg == 401) vals[i] = (unsigned short)pDlg->m_rpm;
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


void MotorSocket::OnAccept(int nErrorCode)
{
    CSocket cliente;
    Accept(cliente);

    // Desvincular el handle del wrapper local
    SOCKET hSock = cliente.Detach();

    // Preparar parámetros para el hilo
    AtenderClienteParams* params = new AtenderClienteParams();
    params->hSocket = hSock;
    params->pDlg = pDlg;

    // Lanzar hilo
    AfxBeginThread(AtenderCliente, params);
}