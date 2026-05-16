
// MaestroDlg.h: archivo de encabezado
//

#pragma once
#include "Led.h"
#include "CWebSocket.h"




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
	Led motor_led;
	CStatic motor_temp;
	CStatic motor_rpm;
	CListBox logs;
	BOOL start;
	afx_msg void OnClickedStart();
	int m_tempValue;
	int m_rpmValue;
	CString accionamientos_ip;
	int accionamientos_port;
	Led led_izq;
	Led led_der;
	Led led_freno;
	Led accionamientos_led;
	afx_msg LRESULT OnLogMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString luces_ip;
	int luces_port;
	Led luces_led;
	int m_frenoState;
	int m_izqState;
	int m_derState;
	int m_tiempo;
	CWebSocket s_listen;          
	void OnWebAccept();           
	BOOL m_motorConn;
	BOOL m_accConn;
	BOOL m_lucesConn;
};
