#include "pch.h"
#include "MotorSocket.h"
#include "EsclavoMotorDlg.h"


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
        if (len < 8) continue;

        unsigned char fc = buf[7];

        if (fc == 0x03 && len >= 12)
        {
            unsigned short addr = (buf[8] << 8) | buf[9];
            unsigned short qty = (buf[10] << 8) | buf[11];

            unsigned char resp[260];
            resp[0] = buf[0];
            resp[1] = buf[1];
            resp[2] = buf[2];
            resp[3] = buf[3];

            unsigned short respLen = 3 + 2 * qty;
            resp[4] = (respLen >> 8) & 0xFF;
            resp[5] = respLen & 0xFF;

            resp[6] = buf[6];
            resp[7] = 0x03;
            resp[8] = (unsigned char)(2 * qty);

            for (int i = 0; i < qty; i++)
            {
                unsigned short reg = addr + i;
                unsigned short val = 0;

                if (reg == 400)        val = (unsigned short)pDlg->m_temp;
                else if (reg == 401)   val = (unsigned short)pDlg->m_rpm;

                resp[9 + 2 * i] = (val >> 8) & 0xFF;
                resp[9 + 2 * i + 1] = val & 0xFF;
            }

            cliente.Send(resp, 9 + 2 * qty);
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