
// EsclavoMotorDlg.cpp: archivo de implementación
//

#include "pch.h"
#include "framework.h"
#include "EsclavoMotor.h"
#include "EsclavoMotorDlg.h"
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


// Cuadro de diálogo de CEsclavoMotorDlg



CEsclavoMotorDlg::CEsclavoMotorDlg(CWnd* pParent /*= nullptr*/)
	: CDialogEx(IDD_ESCLAVOMOTOR_DIALOG, pParent)
	, m_port(3502)
	, m_temp(0)
	, m_rpm(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEsclavoMotorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDX_Slider(pDX, IDC_TEMP, m_temp);
	DDX_Slider(pDX, IDC_RPM, m_rpm);
}

BEGIN_MESSAGE_MAP(CEsclavoMotorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_EXIT, &CEsclavoMotorDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_START, &CEsclavoMotorDlg::OnBnClickedStart)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// Controladores de mensajes de CEsclavoMotorDlg

BOOL CEsclavoMotorDlg::OnInitDialog()
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

	// TODO: agregar aquí inicialización adicional
	((CSliderCtrl*)GetDlgItem(IDC_TEMP))->SetRange(0, 300);
	((CSliderCtrl*)GetDlgItem(IDC_RPM))->SetRange(0, 7000);
	UpdateData(FALSE);

	return TRUE;  // Devuelve TRUE  a menos que establezca el foco en un control
}

void CEsclavoMotorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CEsclavoMotorDlg::OnPaint()
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
HCURSOR CEsclavoMotorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEsclavoMotorDlg::OnBnClickedExit()
{
	CDialog::OnOK();
}

void CEsclavoMotorDlg::OnBnClickedStart()
{
	UpdateData();   // refresca m_port con lo que haya en la caja

	pServerSock = new MotorSocket();
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

	// Opcional: deshabilitar el botón Start para que no se pueda crear dos veces
	GetDlgItem(IDC_START)->EnableWindow(FALSE);

}

void CEsclavoMotorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	UpdateData(TRUE);   // copia el valor del slider a m_temp y m_rpm
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
