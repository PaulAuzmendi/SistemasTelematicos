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
	, motor_port(3502)
	, start(FALSE)
    , accionamientos_ip(_T("127.0.0.1"))
    , accionamientos_port(3503)
    , luces_ip(_T("127.0.0.1"))
    , luces_port(3504)
    , m_frenoState(0)
    , m_izqState(0)
    , m_derState(0)
    , m_tiempo(250)
    , m_motorConn(FALSE)
    , m_accConn(FALSE)
    , m_lucesConn(FALSE)
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
    DDX_Text(pDX, IDC_LUCES_IP, luces_ip);
    DDX_Text(pDX, IDC_LUCES_PORT, luces_port);
    DDX_Control(pDX, IDC_LUCES_LED, luces_led);
    DDX_Text(pDX, IDC_POLLING, m_tiempo);
}

BEGIN_MESSAGE_MAP(CMaestroDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SALIR, &CMaestroDlg::OnBnClickedSalir)
	ON_BN_CLICKED(IDC_START, &CMaestroDlg::OnClickedStart)
    ON_MESSAGE(WM_USER_LOG, &CMaestroDlg::OnLogMsg)
    ON_WM_TIMER()
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
    luces_led.SetColor(RGB(0, 200, 0));   // verde "conectado"
    led_freno.SetColor(RGB(255, 0, 0));            // rojo
    led_izq.SetColor(RGB(255, 165, 0));            // ámbar
    led_der.SetColor(RGB(255, 165, 0));            // ámbar
    SetTimer(1, 400, NULL);
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
    pDlg->m_motorConn = TRUE;
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

        Sleep(pDlg->m_tiempo);
    }

    sock.Close();
    pDlg->motor_led.SetMode(Led::OFF);
    pDlg->m_motorConn = FALSE;


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
    if (!sock.Connect(pDlg->accionamientos_ip, pDlg->accionamientos_port)) {
        sock.Close();
        pDlg->accionamientos_led.SetMode(Led::OFF);
        Log(pDlg, "Accionamientos: no responde (reinicia con Stop+Start)");
        return 0;
    }

    pDlg->accionamientos_led.SetMode(Led::ON_SOLID);
    pDlg->m_accConn = TRUE;
    Log(pDlg, "Accionamientos OK..");

    unsigned short trans = 0;
    bool connOk = true;

    while (pDlg->start && connOk)
    {
        int freno = -1, izq = -1, der = -1;
        unsigned short addrs[3] = { 400, 401, 402 };
        int* outs[3] = { &freno, &izq, &der };

        for (int k = 0; k < 3 && connOk; k++) {
            Modbus f;
            f.transactionId = ++trans;
            f.unitId = 0x01;
            f.address = addrs[k];
            f.quantity = 1;
            unsigned char req[12];
            int n = f.BuildReadRequest(req);
            if (sock.Send(req, n) <= 0) { connOk = false; break; }
            unsigned char resp[260];
            int len = sock.Receive(resp, sizeof(resp));
            if (len <= 0) { connOk = false; break; }
            if (f.ParseResponse(resp, len)) *outs[k] = f.value;
        }
        if (!connOk) break;

        // Publicar estado para que el hilo Luces lo lea
        pDlg->m_frenoState = (freno == 1) ? 1 : 0;
        pDlg->m_izqState = (izq == 1) ? 1 : 0;
        pDlg->m_derState = (der == 1) ? 1 : 0;

        // LEDs locales
        pDlg->led_freno.SetMode(freno == 1 ? Led::ON_SOLID : Led::OFF);
        pDlg->led_izq.SetMode(izq == 1 ? Led::ON_BLINKING : Led::OFF);
        pDlg->led_der.SetMode(der == 1 ? Led::ON_BLINKING : Led::OFF);

        Sleep(pDlg->m_tiempo);
    }

    sock.Close();
    pDlg->accionamientos_led.SetMode(Led::OFF);
    pDlg->m_accConn = FALSE;

    pDlg->led_freno.SetMode(Led::OFF);
    pDlg->led_izq.SetMode(Led::OFF);
    pDlg->led_der.SetMode(Led::OFF);

    // Reset para que Luces no se quede escribiendo valores stale
    pDlg->m_frenoState = 0;
    pDlg->m_izqState = 0;
    pDlg->m_derState = 0;

    if (!connOk && pDlg->start) {
        Log(pDlg, "Accionamientos: conexion perdida (reinicia con Stop+Start)");
    }
    return 0;
}
UINT Luces(LPVOID lp)
{
    CMaestroDlg* pDlg = (CMaestroDlg*)lp;
    if (!AfxSocketInit()) return 0;

    CSocket sock;
    if (!sock.Create()) {
        Log(pDlg, "Luces: error creando socket");
        return 0;
    }
    if (!sock.Connect(pDlg->luces_ip, pDlg->luces_port)) {
        sock.Close();
        pDlg->luces_led.SetMode(Led::OFF);
        Log(pDlg, "Luces: no responde (reinicia con Stop+Start)");
        return 0;
    }

    pDlg->luces_led.SetMode(Led::ON_SOLID);
    pDlg->m_lucesConn = TRUE;
    Log(pDlg, "Luces OK..");

    unsigned short trans = 0;
    bool connOk = true;

    while (pDlg->start && connOk)
    {
        // Snapshot del estado en este ciclo
        int frenoS = pDlg->m_frenoState;
        int izqS = pDlg->m_izqState;
        int derS = pDlg->m_derState;

        unsigned short addrs[5] = { 500, 501, 502, 503, 504 };
        unsigned short vals[5] = {
            (unsigned short)frenoS,
            (unsigned short)izqS,
            (unsigned short)derS,
            (unsigned short)izqS,
            (unsigned short)derS
        };

        for (int k = 0; k < 5 && connOk; k++) {
            Modbus f;
            f.transactionId = ++trans;
            f.unitId = 0x01;
            f.address = addrs[k];
            f.value = vals[k];
            unsigned char req[12];
            int n = f.BuildWriteRequest(req);
            if (sock.Send(req, n) <= 0) { connOk = false; break; }
            unsigned char resp[260];
            int len = sock.Receive(resp, sizeof(resp));
            if (len <= 0) { connOk = false; break; }
            f.ParseResponse(resp, len);   // eco
        }
        if (!connOk) break;

        Sleep(pDlg->m_tiempo);
    }

    sock.Close();
    pDlg->luces_led.SetMode(Led::OFF);
    pDlg->m_lucesConn = FALSE;


    if (!connOk && pDlg->start) {
        Log(pDlg, "Luces: conexion perdida (reinicia con Stop+Start)");
    }
    return 0;
}
void CMaestroDlg::OnClickedStart()
{
    UpdateData(TRUE);

    if (start) {
        Log(this, "Start Polling..");
        AfxBeginThread(Motor, this);
        Sleep(50);
        AfxBeginThread(Accionamientos, this);
        Sleep(50);
        AfxBeginThread(Luces, this);

        if (s_listen.Create(8080, SOCK_STREAM) && s_listen.Listen()) {
            Log(this, "WebServer running on port 8080");
        }
        else {
            Log(this, "WebServer: error al iniciar en 8080");
            s_listen.Close();
        }
    }
    else {
        Log(this, "Stop Polling..");
        s_listen.Close();
    }
}

void CMaestroDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1) {
        led_izq.Tick();
        led_der.Tick();
    }
    CDialogEx::OnTimer(nIDEvent);
}

static CStringA getMainPage()
{
    return R"HTML(<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>Centralita</title>
<style>
  * { box-sizing: border-box; }
  body {
    font-family: -apple-system, "Segoe UI", system-ui, sans-serif;
    background: #0f0f10;
    color: #e6e6e6;
    margin: 0;
    padding: 24px;
  }
  .container {
    max-width: 720px;
    margin: 0 auto;
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 16px;
  }
  .card {
    background: #1a1a1c;
    border: 1px solid #2a2a2d;
    border-radius: 16px;
    padding: 24px;
  }
  .card h2 {
    margin: 0 0 8px 0;
    font-size: 11px;
    font-weight: 600;
    text-transform: uppercase;
    letter-spacing: 2px;
    color: #707075;
  }
  .number {
    font-family: "SF Mono", Consolas, Menlo, monospace;
    font-size: 64px;
    font-weight: 200;
    color: #fff;
    letter-spacing: -2px;
    line-height: 1.1;
  }
  .number .unit {
    font-size: 16px;
    color: #707075;
    margin-left: 8px;
    font-weight: 400;
    letter-spacing: 0;
  }
  .full { grid-column: span 2; }
  .car-wrap { display: flex; justify-content: center; padding: 8px 0; }

  .status-row {
    display: flex;
    justify-content: space-around;
    font-size: 13px;
    color: #aaa;
  }
  .dot {
    display: inline-block;
    width: 10px;
    height: 10px;
    border-radius: 50%;
    background: #3a3a3d;
    margin-right: 8px;
    vertical-align: middle;
  }
  .dot.on {
    background: #4ade80;
    box-shadow: 0 0 8px rgba(74, 222, 128, 0.6);
  }

  /* Car SVG */
  .body-shape { fill: #2d2d30; stroke: #3a3a3d; stroke-width: 1; }
  .window    { fill: #15151a; }
  .wheel     { fill: #0a0a0a; }
  .headlight { fill: #f5f5dc; opacity: 0.85; }
  .signal    { fill: #3a3a3d; transition: fill 0.1s; }
  .brake     { fill: #3a3a3d; transition: fill 0.1s; }

  .signal.on {
    fill: #ff9500;
    animation: blink 0.8s steps(1) infinite;
    filter: drop-shadow(0 0 6px rgba(255, 149, 0, 0.7));
  }
  .brake.on {
    fill: #ef4444;
    filter: drop-shadow(0 0 8px rgba(239, 68, 68, 0.8));
  }
  @keyframes blink {
    0%, 49%   { opacity: 1; }
    50%, 100% { opacity: 0.15; }
  }
</style>
</head>
<body>
  <div class="container">
    <div class="card">
      <h2>RPM</h2>
      <div class="number"><span id="rpm">--</span><span class="unit">rpm</span></div>
    </div>
    <div class="card">
      <h2>Temperatura</h2>
      <div class="number"><span id="temp">--</span><span class="unit">&deg;C</span></div>
    </div>

    <div class="card full">
      <h2>Vehiculo</h2>
      <div class="car-wrap">
        <svg viewBox="0 0 200 400" width="180" height="360" xmlns="http://www.w3.org/2000/svg">
          <!-- car body -->
          <rect class="body-shape" x="20" y="20" width="160" height="360" rx="32" ry="32"/>

          <!-- windshield (front) -->
          <path class="window" d="M 38 62 L 162 62 L 152 112 L 48 112 Z"/>
          <!-- rear window -->
          <path class="window" d="M 48 290 L 152 290 L 162 340 L 38 340 Z"/>

          <!-- wheels (4 corners, on the sides) -->
          <rect class="wheel" x="10" y="70"  width="14" height="50" rx="3"/>
          <rect class="wheel" x="176" y="70"  width="14" height="50" rx="3"/>
          <rect class="wheel" x="10" y="282" width="14" height="50" rx="3"/>
          <rect class="wheel" x="176" y="282" width="14" height="50" rx="3"/>

          <!-- front headlights -->
          <rect class="headlight" x="60" y="26" width="35" height="10" rx="3"/>
          <rect class="headlight" x="105" y="26" width="35" height="10" rx="3"/>

          <!-- front turn signals -->
          <rect class="signal" id="sig-fl" x="26" y="26" width="28" height="10" rx="3"/>
          <rect class="signal" id="sig-fr" x="146" y="26" width="28" height="10" rx="3"/>

          <!-- rear brake lights -->
          <rect class="brake" id="brake-l" x="58" y="365" width="38" height="10" rx="3"/>
          <rect class="brake" id="brake-r" x="104" y="365" width="38" height="10" rx="3"/>

          <!-- rear turn signals -->
          <rect class="signal" id="sig-rl" x="26" y="365" width="28" height="10" rx="3"/>
          <rect class="signal" id="sig-rr" x="146" y="365" width="28" height="10" rx="3"/>
        </svg>
      </div>
    </div>

    <div class="card full">
      <div class="status-row">
        <div><span class="dot" id="dot-motor"></span>Motor</div>
        <div><span class="dot" id="dot-acc"></span>Accionamientos</div>
        <div><span class="dot" id="dot-luces"></span>Luces</div>
      </div>
    </div>
  </div>

<script>
async function update() {
  try {
    const r = await fetch('/data');
    if (!r.ok) return;
    const d = await r.json();

    if (d.motor) {
      document.getElementById('rpm').textContent  = (d.motor.rpm  ?? '--');
      document.getElementById('temp').textContent = (d.motor.temp ?? '--');
      document.getElementById('dot-motor').classList.toggle('on', !!d.motor.connected);
    }
    if (d.acc) {
      const izq = d.acc.izq === 1, der = d.acc.der === 1, fre = d.acc.freno === 1;
      document.getElementById('sig-fl').classList.toggle('on', izq);
      document.getElementById('sig-rl').classList.toggle('on', izq);
      document.getElementById('sig-fr').classList.toggle('on', der);
      document.getElementById('sig-rr').classList.toggle('on', der);
      document.getElementById('brake-l').classList.toggle('on', fre);
      document.getElementById('brake-r').classList.toggle('on', fre);
      document.getElementById('dot-acc').classList.toggle('on', !!d.acc.connected);
    }
    if (d.luces) {
      document.getElementById('dot-luces').classList.toggle('on', !!d.luces.connected);
    }
  } catch (e) {}
}
setInterval(update, 300);
update();
</script>
</body>
</html>)HTML";
}
static CStringA getDataJSON(CMaestroDlg* pDlg)
{
    CStringA json;
    json.Format(
        "{"
        "\"motor\":{\"connected\":%s,\"temp\":%d,\"rpm\":%d},"
        "\"acc\":{\"connected\":%s,\"freno\":%d,\"izq\":%d,\"der\":%d},"
        "\"luces\":{\"connected\":%s}"
        "}",
        pDlg->m_motorConn ? "true" : "false",
        pDlg->m_tempValue,
        pDlg->m_rpmValue,
        pDlg->m_accConn ? "true" : "false",
        pDlg->m_frenoState,
        pDlg->m_izqState,
        pDlg->m_derState,
        pDlg->m_lucesConn ? "true" : "false"
    );
    return json;
}

void CMaestroDlg::OnWebAccept()
{
    CWebSocket s_cliente;
    if (!s_listen.Accept(s_cliente)) return;

    char bufweb[10240];
    int len = s_cliente.Receive(bufweb, sizeof(bufweb) - 1);
    if (len <= 0) { s_cliente.Close(); return; }
    bufweb[len] = 0;

    // --- Extraer el path de la linea "GET /path HTTP/1.1" ---
    CStringA req(bufweb);
    CStringA path = "/";
    int posGet = req.Find("GET ");
    if (posGet != -1) {
        int posHttp = req.Find(" HTTP", posGet);
        if (posHttp != -1) {
            path = req.Mid(posGet + 4, posHttp - (posGet + 4));
        }
    }

    CString logLine;
    logLine.Format("Web GET %s", (LPCSTR)path);
    //Log(this, logLine);

    // --- Routing ---
    CStringA body;
    CStringA contentType;

    if (path == "/data") {
        body = getDataJSON(this);
        contentType = "application/json";
    }
    else if (path == "/" || path.Left(2) == "/?") {
        body = getMainPage();
        contentType = "text/html; charset=utf-8";
    }
    else {
        body = "Not Found";
        contentType = "text/plain";
    }

    // --- Respuesta HTTP ---
    CStringA header;
    header.Format(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        (LPCSTR)contentType, body.GetLength());

    s_cliente.Send(header.GetBuffer(), header.GetLength());
    s_cliente.Send(body.GetBuffer(), body.GetLength());
    s_cliente.Close();
}