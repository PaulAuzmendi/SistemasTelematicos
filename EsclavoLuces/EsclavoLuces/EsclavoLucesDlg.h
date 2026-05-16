
// EsclavoLucesDlg.h: archivo de encabezado
//

#pragma once
#include "Led.h"
#include "LucesSocket.h"


// Cuadro de diálogo de CEsclavoLucesDlg
class CEsclavoLucesDlg : public CDialogEx
{
// Construcción
public:
	CEsclavoLucesDlg(CWnd* pParent = nullptr);	// Constructor estándar

// Datos del cuadro de diálogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ESCLAVOLUCES_DIALOG };
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
	int m_port;
	Led led_freno;
	Led led_izq_del;
	Led led_der_del;
	Led led_izq_tras;
	Led led_der_tras;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	LucesSocket* pServerSock;
	afx_msg void OnBnClickedStart();
};
