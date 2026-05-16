
// EsclavoAccionamientosDlg.h: archivo de encabezado
//

#pragma once
#include "AccionamientoSocket.h"


// Cuadro de diálogo de CEsclavoAccionamientosDlg
class CEsclavoAccionamientosDlg : public CDialogEx
{
// Construcción
public:
	CEsclavoAccionamientosDlg(CWnd* pParent = nullptr);	// Constructor estándar

// Datos del cuadro de diálogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ESCLAVOACCIONAMIENTOS_DIALOG };
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
	BOOL freno;
	BOOL izquierdo;
	BOOL derecho;
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedFreno();
	afx_msg void OnBnClickedIzq();
	afx_msg void OnBnClickedDer();
	AccionamientoSocket* pServerSock;

	BOOL m_start;
};
