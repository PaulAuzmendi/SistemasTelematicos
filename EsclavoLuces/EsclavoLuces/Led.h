#pragma once

class Led : public CStatic
{
public:
    enum Mode { OFF, ON_SOLID, ON_BLINKING };

    Led();

    void SetColor(COLORREF c);
    void SetMode(Mode m);
    void Tick();                 // llamar desde el Timer del diálogo

protected:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()

private:
    COLORREF m_color;
    Mode     m_mode;
    bool     m_blinkVisible;
};