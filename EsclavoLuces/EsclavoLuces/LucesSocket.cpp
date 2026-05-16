#include "pch.h"
#include "LucesSocket.h"
#include "EsclavoLucesDlg.h"
#include "Modbus.h"

struct AtenderClienteLucesParams
{
    SOCKET hSocket;
    CEsclavoLucesDlg* pDlg;
};

UINT AtenderClienteLuces(LPVOID lp)
{
    if (!AfxSocketInit()) return 0;

    AtenderClienteLucesParams* params = (AtenderClienteLucesParams*)lp;
    SOCKET hSock = params->hSocket;
    CEsclavoLucesDlg* pDlg = params->pDlg;
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

        if (f.functionCode == 0x06)
        {
            // Actualizar el LED que corresponda
            switch (f.address)
            {
            case 500:
                pDlg->led_freno.SetMode(f.value == 1 ? Led::ON_SOLID : Led::OFF);
                break;
            case 501:
                pDlg->led_izq_del.SetMode(f.value == 1 ? Led::ON_BLINKING : Led::OFF);
                break;
            case 502:
                pDlg->led_der_del.SetMode(f.value == 1 ? Led::ON_BLINKING : Led::OFF);
                break;
            case 503:
                pDlg->led_izq_tras.SetMode(f.value == 1 ? Led::ON_BLINKING : Led::OFF);
                break;
            case 504:
                pDlg->led_der_tras.SetMode(f.value == 1 ? Led::ON_BLINKING : Led::OFF);
                break;
            }

            // Eco de respuesta al Maestro
            unsigned char resp[12];
            int n = f.BuildWriteResponseEcho(resp);
            cliente.Send(resp, n);
        }
    }

    cliente.Close();
    return 0;
}

void LucesSocket::OnAccept(int nErrorCode)
{
    CSocket cliente;
    Accept(cliente);

    SOCKET hSock = cliente.Detach();

    AtenderClienteLucesParams* params = new AtenderClienteLucesParams();
    params->hSocket = hSock;
    params->pDlg = pDlg;

    AfxBeginThread(AtenderClienteLuces, params);
}