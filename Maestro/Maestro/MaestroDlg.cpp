// MaestroDlg.cpp: archivo de implementación
//

#include "pch.h"
#include "framework.h"
#include "Maestro.h"
#include "MaestroDlg.h"
#include "afxdialogex.h"
#include <math.h>


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
}

BEGIN_MESSAGE_MAP(CMaestroDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SALIR, &CMaestroDlg::OnBnClickedSalir)
	ON_BN_CLICKED(IDC_START, &CMaestroDlg::OnClickedStart)
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

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

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


UINT Motor(LPVOID lp)
{
	CMaestroDlg* pDlg = (CMaestroDlg*)lp;

	if (!AfxSocketInit()) {
		return 0;
	}

	CSocket sock;
	if (!sock.Create()) return 0;
	if (!sock.Connect(pDlg->motor_ip, pDlg->motor_port)) {
		sock.Close();
		return 0;
	}

	// ---- Pintar LED en verde (conectado) ----
	{
		CDC* pdc = pDlg->motor_led.GetDC();
		CRect r;
		pDlg->motor_led.GetClientRect(r);
		pdc->FillSolidRect(r, RGB(0, 200, 0));
		pDlg->motor_led.ReleaseDC(pdc);
	}

	unsigned short trans = 0;

	while (pDlg->start)
	{
		int temp = -1, rpm = -1;

		// ---- Trama 1: Temperatura (addr 400) ----
		{
			trans++;
			unsigned short addr = 400;
			unsigned char req[12] = {
				(unsigned char)(trans >> 8), (unsigned char)(trans & 0xFF),
				0x00, 0x00, 0x00, 0x06, 0x01, 0x03,
				(unsigned char)(addr >> 8), (unsigned char)(addr & 0xFF),
				0x00, 0x01
			};
			sock.Send(req, 12);
			unsigned char resp[20];
			int len = sock.Receive(resp, sizeof(resp));
			if (len >= 11) temp = (resp[9] << 8) | resp[10];
		}

		// ---- Trama 2: RPM (addr 401) ----
		{
			trans++;
			unsigned short addr = 401;
			unsigned char req[12] = {
				(unsigned char)(trans >> 8), (unsigned char)(trans & 0xFF),
				0x00, 0x00, 0x00, 0x06, 0x01, 0x03,
				(unsigned char)(addr >> 8), (unsigned char)(addr & 0xFF),
				0x00, 0x01
			};
			sock.Send(req, 12);
			unsigned char resp[20];
			int len = sock.Receive(resp, sizeof(resp));
			if (len >= 11) rpm = (resp[9] << 8) | resp[10];
		}

		if (temp >= 0) pDlg->m_tempValue = temp;
		if (rpm >= 0)  pDlg->m_rpmValue = rpm;
		TRACE(_T("Temp: %d | RPM: %d\n"), temp, rpm);

		// ---- Dibujar Gauge Temperatura (0..300) ----
		{
			CDC* pdc = pDlg->motor_temp.GetDC();
			CRect r;
			pDlg->motor_temp.GetClientRect(r);
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
			pdc->MoveTo(cx, cy);
			pdc->LineTo(xe, ye);

			pdc->SelectObject(oldPen);
			pDlg->motor_temp.ReleaseDC(pdc);
		}

		// ---- Dibujar Gauge RPM (0..7000) ----
		{
			CDC* pdc = pDlg->motor_rpm.GetDC();
			CRect r;
			pDlg->motor_rpm.GetClientRect(r);
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
			pdc->MoveTo(cx, cy);
			pdc->LineTo(xe, ye);

			pdc->SelectObject(oldPen);
			pDlg->motor_rpm.ReleaseDC(pdc);
		}

		Sleep(250);
	}

	sock.Close();

	// ---- Pintar LED en gris (desconectado) ----
	{
		CDC* pdc = pDlg->motor_led.GetDC();
		CRect r;
		pDlg->motor_led.GetClientRect(r);
		pdc->FillSolidRect(r, RGB(192, 192, 192));
		pDlg->motor_led.ReleaseDC(pdc);
	}

	return 0;
}


void CMaestroDlg::OnClickedStart()
{
	UpdateData(TRUE);

	if (start)
	{
		AfxBeginThread(Motor, this);
	}
}