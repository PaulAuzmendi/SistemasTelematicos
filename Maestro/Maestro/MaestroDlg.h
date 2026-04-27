
// MaestroDlg.h: archivo de encabezado
//

#pragma once


// Cuadro de diálogo de CMaestroDlg
class CMaestroDlg : public CDialogEx
{
// Construcción
public:
	CMaestroDlg(CWnd* pParent = nullptr);	// Constructor estándar

// Datos del cuadro de diálogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAESTRO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// Compatibilidad con DDX/DDV


// Implementación
protected:
	HICON m_hIcon;

	// Funciones de asignación de mensajes generadas
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSalir();
	CString motor_ip;
	int motor_port;
	CStatic motor_led;
	CStatic motor_temp;
	CStatic motor_rpm;
	CListBox logs;
	BOOL start;
	afx_msg void OnClickedStart();
	int m_tempValue;
	int m_rpmValue;
};
