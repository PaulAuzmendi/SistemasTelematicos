#include "pch.h"
#include "Led.h"

BEGIN_MESSAGE_MAP(Led, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()

Led::Led()
    : m_color(RGB(0, 200, 0))     // verde por defecto; se cambia con SetColor
    , m_mode(OFF)
    , m_blinkVisible(true)
{
}

void Led::SetColor(COLORREF c)
{
    m_color = c;
    Invalidate(FALSE);
}

void Led::SetMode(Mode m)
{
    if (m_mode == m) return;
    m_mode = m;
    // (quitar el m_blinkVisible = true que tenías aquí)
    Invalidate(FALSE);
}

void Led::Tick()
{
    m_blinkVisible = !m_blinkVisible;     // toggle SIEMPRE
    if (m_mode == ON_BLINKING)             // pero solo repinta si blinking
        Invalidate(FALSE);
}

void Led::OnPaint()
{
    CPaintDC dc(this);
    CRect r;
    GetClientRect(r);

    COLORREF c = RGB(192, 192, 192);  // gris si OFF o en fase invisible del parpadeo
    if (m_mode == ON_SOLID)                       c = m_color;
    else if (m_mode == ON_BLINKING && m_blinkVisible)  c = m_color;

    dc.FillSolidRect(r, c);
}