// MaestroDlg.cpp: archivo de implementación
//

#include "pch.h"
#include "framework.h"
#include "Maestro.h"
#include "MaestroDlg.h"
#include "afxdialogex.h"
#include <math.h>
#include "Modbus.h"

#define WM_USER_LOG (WM_USER + 100)


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cuadro de diálogo CAboutDlg utilizado para el comando Acerca de

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cuadro de diálogo de CMaestroDlg


CMaestroDlg::CMaestroDlg(CWnd* pParent /*= nullptr*/)
	: CDialogEx(IDD_MAESTRO_DIALOG, pParent)
	, motor_ip(_T("127.0.0.1"))
	, motor_port(502)
	, start(FALSE)
    , accionamientos_ip(_T("127.0.0.1"))
    , accionamientos_port(503)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_tempValue = 0;
	m_rpmValue = 0;
}

void CMaestroDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_MOTO_RIP, motor_ip);
    DDX_Text(pDX, IDC_MOTOR_PORT, motor_port);
    DDX_Control(pDX, IDC_MOTOR_LED, motor_led);
    DDX_Control(pDX, IDC_TEMP, motor_temp);
    DDX_Control(pDX, IDC_RPM, motor_rpm);
    DDX_Control(pDX, IDC_LOGS, logs);
    DDX_Check(pDX, IDC_START, start);
    DDX_Text(pDX, IDC_ACCIONAMIENTOS_IP, accionamientos_ip);
    DDX_Text(pDX, IDC_ACCIONAMIENTOS_PORT, accionamientos_port);
    DDX_Control(pDX, IDC_IZQUIERDO, led_izq);
    DDX_Control(pDX, IDC_DERECHO, led_der);
    DDX_Control(pDX, IDC_FRENO, led_freno);
    DDX_Control(pDX, IDC_ACCIONAMIENTOS_LED, accionamientos_led);
}

BEGIN_MESSAGE_MAP(CMaestroDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SALIR, &CMaestroDlg::OnBnClickedSalir)
	ON_BN_CLICKED(IDC_START, &CMaestroDlg::OnClickedStart)
    ON_MESSAGE(WM_USER_LOG, &CMaestroDlg::OnLogMsg)
END_MESSAGE_MAP()


BOOL CMaestroDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

    if (!AfxSocketInit()) {                      // <-- añade esto
        AfxMessageBox(_T("Error inicializando sockets"));
        return FALSE;
    }

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

    motor_led.SetColor(RGB(0, 200, 0));            // verde
    accionamientos_led.SetColor(RGB(0, 200, 0));   // verde
    led_freno.SetColor(RGB(255, 0, 0));            // rojo
    led_izq.SetColor(RGB(255, 165, 0));            // ámbar
    led_der.SetColor(RGB(255, 165, 0));            // ámbar
	UpdateData(FALSE);

	return TRUE;
}

void CMaestroDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CMaestroDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMaestroDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMaestroDlg::OnBnClickedSalir()
{
	CDialog::OnOK();
}

static void Log(CMaestroDlg* pDlg, const char* msg)
{
    COleDateTime now = COleDateTime::GetCurrentTime();
    CString* line = new CString;
    line->Format("%s %s",
        (const char*)now.Format("%H:%M:%S"),
        msg);
    pDlg->PostMessage(WM_USER_LOG, 0, (LPARAM)line);
}

LRESULT CMaestroDlg::OnLogMsg(WPARAM wParam, LPARAM lParam)
{
    CString* line = (CString*)lParam;
    logs.AddString(*line);
    delete line;
    return 0;
}

UINT Motor(LPVOID lp)
{
    CMaestroDlg* pDlg = (CMaestroDlg*)lp;
    if (!AfxSocketInit()) return 0;

    CSocket sock;
    if (!sock.Create()) {
        Log(pDlg, "Motor: error creando socket");
        return 0;
    }

    if (!sock.Connect(pDlg->motor_ip, pDlg->motor_port))
    {
        sock.Close();
        pDlg->motor_led.SetMode(Led::OFF);
        Log(pDlg, "Motor: no responde (reinicia con Stop+Start)");
        return 0;
    }

    pDlg->motor_led.SetMode(Led::ON_SOLID);
    Log(pDlg, "Motor OK..");

    unsigned short trans = 0;
    bool connOk = true;

    while (pDlg->start && connOk)
    {
        int temp = -1, rpm = -1;

        // Leer temperatura
        {
            Modbus f;
            f.transactionId = ++trans;
            f.unitId = 0x01;
            f.address = 400;
            f.quantity = 1;
            unsigned char req[12];
            int n = f.BuildReadRequest(req);
            if (sock.Send(req, n) <= 0) connOk = false;
            else {
                unsigned char resp[260];
                int len = sock.Receive(resp, sizeof(resp));
                if (len <= 0) connOk = false;
                else if (f.ParseResponse(resp, len)) temp = f.value;
            }
        }
        if (!connOk) break;

        // Leer RPM
        {
            Modbus f;
            f.transactionId = ++trans;
            f.unitId = 0x01;
            f.address = 401;
            f.quantity = 1;
            unsigned char req[12];
            int n = f.BuildReadRequest(req);
            if (sock.Send(req, n) <= 0) connOk = false;
            else {
                unsigned char resp[260];
                int len = sock.Receive(resp, sizeof(resp));
                if (len <= 0) connOk = false;
                else if (f.ParseResponse(resp, len)) rpm = f.value;
            }
        }
        if (!connOk) break;

        if (temp >= 0) pDlg->m_tempValue = temp;
        if (rpm >= 0) pDlg->m_rpmValue = rpm;

        // ---- Gauge Temperatura ----
        {
            CDC* pdc = pDlg->motor_temp.GetDC();
            CRect r; pDlg->motor_temp.GetClientRect(r);
            pdc->FillSolidRect(r, RGB(255, 255, 255));

            int cx = r.Width() / 2;
            int cy = r.bottom - 5;
            int radio = min(r.Width() / 2 - 5, r.Height() - 10);

            CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
            CPen* oldPen = pdc->SelectObject(&blackPen);
            pdc->Arc(cx - radio, cy - radio, cx + radio, cy + radio,
                cx + radio, cy, cx - radio, cy);

            double angle = 3.14159265 * (1.0 - (double)pDlg->m_tempValue / 300.0);
            int xe = cx + (int)(radio * cos(angle));
            int ye = cy - (int)(radio * sin(angle));

            CPen redPen(PS_SOLID, 2, RGB(255, 0, 0));
            pdc->SelectObject(&redPen);
            pdc->MoveTo(cx, cy); pdc->LineTo(xe, ye);
            pdc->SelectObject(oldPen);
            pDlg->motor_temp.ReleaseDC(pdc);
        }

        // ---- Gauge RPM ----
        {
            CDC* pdc = pDlg->motor_rpm.GetDC();
            CRect r; pDlg->motor_rpm.GetClientRect(r);
            pdc->FillSolidRect(r, RGB(255, 255, 255));

            int cx = r.Width() / 2;
            int cy = r.bottom - 5;
            int radio = min(r.Width() / 2 - 5, r.Height() - 10);

            CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
            CPen* oldPen = pdc->SelectObject(&blackPen);
            pdc->Arc(cx - radio, cy - radio, cx + radio, cy + radio,
                cx + radio, cy, cx - radio, cy);

            double angle = 3.14159265 * (1.0 - (double)pDlg->m_rpmValue / 7000.0);
            int xe = cx + (int)(radio * cos(angle));
            int ye = cy - (int)(radio * sin(angle));

            CPen redPen(PS_SOLID, 2, RGB(255, 0, 0));
            pdc->SelectObject(&redPen);
            pdc->MoveTo(cx, cy); pdc->LineTo(xe, ye);
            pdc->SelectObject(oldPen);
            pDlg->motor_rpm.ReleaseDC(pdc);
        }

        Sleep(250);
    }

    sock.Close();
    pDlg->motor_led.SetMode(Led::OFF);

    if (!connOk && pDlg->start) {
        // Salimos por error de socket, no porque el usuario parara
        Log(pDlg, "Motor: conexion perdida (reinicia con Stop+Start)");
    }

    return 0;
}
UINT Accionamientos(LPVOID lp)
{
    CMaestroDlg* pDlg = (CMaestroDlg*)lp;
    if (!AfxSocketInit()) return 0;

    CSocket sock;
    if (!sock.Create()) {
        Log(pDlg, "Accionamientos: error creando socket");
        return 0;
    }

    if (!sock.Connect(pDlg->accionamientos_ip, pDlg->accionamientos_port))
    {
        sock.Close();
        pDlg->accionamientos_led.SetMode(Led::OFF);
        Log(pDlg, "Accionamientos: no responde (reinicia con Stop+Start)");
        return 0;
    }

    pDlg->accionamientos_led.SetMode(Led::ON_SOLID);
    Log(pDlg, "Accionamientos OK..");

    unsigned short trans = 0;
    bool connOk = true;

    while (pDlg->start && connOk)
    {
        int freno = -1, izq = -1, der = -1;

        // Freno
        {
            Modbus f;
            f.transactionId = ++trans;
            f.unitId = 0x01;
            f.address = 400;
            f.quantity = 1;
            unsigned char req[12];
            int n = f.BuildReadRequest(req);
            if (sock.Send(req, n) <= 0) connOk = false;
            else {
                unsigned char resp[260];
                int len = sock.Receive(resp, sizeof(resp));
                if (len <= 0) connOk = false;
                else if (f.ParseResponse(resp, len)) freno = f.value;
            }
        }
        if (!connOk) break;

        // Izq
        {
            Modbus f;
            f.transactionId = ++trans;
            f.unitId = 0x01;
            f.address = 401;
            f.quantity = 1;
            unsigned char req[12];
            int n = f.BuildReadRequest(req);
            if (sock.Send(req, n) <= 0) connOk = false;
            else {
                unsigned char resp[260];
                int len = sock.Receive(resp, sizeof(resp));
                if (len <= 0) connOk = false;
                else if (f.ParseResponse(resp, len)) izq = f.value;
            }
        }
        if (!connOk) break;

        // Der
        {
            Modbus f;
            f.transactionId = ++trans;
            f.unitId = 0x01;
            f.address = 402;
            f.quantity = 1;
            unsigned char req[12];
            int n = f.BuildReadRequest(req);
            if (sock.Send(req, n) <= 0) connOk = false;
            else {
                unsigned char resp[260];
                int len = sock.Receive(resp, sizeof(resp));
                if (len <= 0) connOk = false;
                else if (f.ParseResponse(resp, len)) der = f.value;
            }
        }
        if (!connOk) break;

        pDlg->led_freno.SetMode(freno == 1 ? Led::ON_SOLID : Led::OFF);
        pDlg->led_izq.SetMode(izq == 1 ? Led::ON_BLINKING : Led::OFF);
        pDlg->led_der.SetMode(der == 1 ? Led::ON_BLINKING : Led::OFF);

        Sleep(250);
    }

    sock.Close();
    pDlg->accionamientos_led.SetMode(Led::OFF);
    pDlg->led_freno.SetMode(Led::OFF);
    pDlg->led_izq.SetMode(Led::OFF);
    pDlg->led_der.SetMode(Led::OFF);

    if (!connOk && pDlg->start) {
        Log(pDlg, "Accionamientos: conexion perdida (reinicia con Stop+Start)");
    }

    return 0;
}
void CMaestroDlg::OnClickedStart()
{
    UpdateData(TRUE);

    if (start) {
        Log(this, "Start Polling..");
        AfxBeginThread(Motor, this);
        Sleep(50);                              // <-- evita arranque simultáneo
        AfxBeginThread(Accionamientos, this);
    }
    else {
        Log(this, "Stop Polling..");
    }
}