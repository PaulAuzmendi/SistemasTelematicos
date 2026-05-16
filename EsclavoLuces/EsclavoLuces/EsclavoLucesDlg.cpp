
// EsclavoLucesDlg.cpp: archivo de implementación
//

#include "pch.h"
#include "framework.h"
#include "EsclavoLuces.h"
#include "EsclavoLucesDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cuadro de diálogo CAboutDlg utilizado para el comando Acerca de

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Datos del cuadro de diálogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV

// Implementación
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


// Cuadro de diálogo de CEsclavoLucesDlg



CEsclavoLucesDlg::CEsclavoLucesDlg(CWnd* pParent /*= nullptr*/)
	: CDialogEx(IDD_ESCLAVOLUCES_DIALOG, pParent)
	, m_port(3504)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEsclavoLucesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDX_Control(pDX, IDC_FRENO, led_freno);
	DDX_Control(pDX, IDC_DELANTE_IZQ, led_izq_del);
	DDX_Control(pDX, IDC_DELANTE_DER, led_der_del);
	DDX_Control(pDX, IDC_TRASERO_IZQ, led_izq_tras);
	DDX_Control(pDX, IDC_TRASERO_DER, led_der_tras);
}

BEGIN_MESSAGE_MAP(CEsclavoLucesDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_START, &CEsclavoLucesDlg::OnBnClickedStart)
END_MESSAGE_MAP()


// Controladores de mensajes de CEsclavoLucesDlg

BOOL CEsclavoLucesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Agregar el elemento de menú "Acerca de..." al menú del sistema.

	// IDM_ABOUTBOX debe estar en el intervalo de comandos del sistema.
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

	// Establecer el icono para este cuadro de diálogo.  El marco de trabajo realiza esta operación
	//  automáticamente cuando la ventana principal de la aplicación no es un cuadro de diálogo
	SetIcon(m_hIcon, TRUE);			// Establecer icono grande
	SetIcon(m_hIcon, FALSE);		// Establecer icono pequeño
	
	led_freno.SetColor(RGB(255, 0, 0));     // rojo
	led_izq_del.SetColor(RGB(255, 165, 0));   // ámbar
	led_der_del.SetColor(RGB(255, 165, 0));
	led_izq_tras.SetColor(RGB(255, 165, 0));
	led_der_tras.SetColor(RGB(255, 165, 0));

	SetTimer(1, 400, NULL);   // tick para los intermitentes

	return TRUE;  // Devuelve TRUE  a menos que establezca el foco en un control
}

void CEsclavoLucesDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// Si agrega un botón Minimizar al cuadro de diálogo, necesitará el siguiente código
//  para dibujar el icono.  Para aplicaciones MFC que utilicen el modelo de documentos y vistas,
//  esta operación la realiza automáticamente el marco de trabajo.

void CEsclavoLucesDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Contexto de dispositivo para dibujo

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrar icono en el rectángulo de cliente
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Dibujar el icono
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// El sistema llama a esta función para obtener el cursor que se muestra mientras el usuario arrastra
//  la ventana minimizada.
HCURSOR CEsclavoLucesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEsclavoLucesDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		led_izq_del.Tick();
		led_der_del.Tick();
		led_izq_tras.Tick();
		led_der_tras.Tick();
	}
	CDialogEx::OnTimer(nIDEvent);
}
void CEsclavoLucesDlg::OnBnClickedStart()
{
	UpdateData();

	pServerSock = new LucesSocket();
	pServerSock->pDlg = this;

	if (!pServerSock->Create(m_port, SOCK_STREAM)) {
		MessageBox(_T("Error al crear el socket"));
		delete pServerSock;
		pServerSock = NULL;
		return;
	}

	if (!pServerSock->Listen()) {
		MessageBox(_T("Error al ponerse a escuchar"));
		return;
	}

	GetDlgItem(IDC_START)->EnableWindow(FALSE);
}
