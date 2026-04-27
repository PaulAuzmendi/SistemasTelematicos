
// EsclavoMotorDlg.h: archivo de encabezado
//

#pragma once
#include "MotorSocket.h"



// Cuadro de diálogo de CEsclavoMotorDlg
class CEsclavoMotorDlg : public CDialogEx
{
// Construcción
public:
	CEsclavoMotorDlg(CWnd* pParent = nullptr);	// Constructor estándar

// Datos del cuadro de diálogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ESCLAVOMOTOR_DIALOG };
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
	int m_temp;
	int m_rpm;
	afx_msg void OnBnClickedExit();
	MotorSocket* pServerSock;

	afx_msg void OnBnClickedStart();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
