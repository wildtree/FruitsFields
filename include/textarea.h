//
// Text Area for Fruits Fields
//
#ifndef TEXTAREA_H
#define TEXTAREA_H

#include <M5Unified.h>
#include <M5GFX.h>

class TextArea
{
public:
    enum Alignment { Left = 0, Center = 1, Right = 2 };
    TextArea(M5GFX &display, int x, int y)
        : _x(x), _y(y)
    {
        _sprite = new M5Canvas(&display);
        _sprite->setColorDepth(8);
        _sprite->createSprite(320 - 2 * x, 16);
    }
    TextArea(M5Canvas &canvas, int x, int y)
        : _x(x), _y(y)
    {
        _sprite = new M5Canvas(&canvas);
        _sprite->createSprite(canvas.width() - 2 * x, 16);
        _sprite->setColorDepth(3);
    }
    virtual ~TextArea()
    {
        delete _sprite;
    }

    virtual void cls() const
    {
        _sprite->fillRect(0, 0, _sprite->width(), _sprite->height(), 0);
    }
    virtual void print(const char *str, uint8_t color, Alignment align = Left)
    {
        _sprite->setFont(&fonts::AsciiFont8x16);
        _sprite->setTextColor(color);

        int x = 0;
        int len = strlen(str) * 8;
        if (align == Center)
        {
            x = (_sprite->width() - len) / 2;
        }
        else if (align == Right)
        {
            x = (_sprite->width() - len);
        }
        _sprite->setCursor(x, 0);
        _sprite->print(str);
    }
    void flush()
    {
    #ifdef M5ATOM_LITE
        _sprite->pushRotateZoomWithAA((_x + _sprite->width() / 2) * 0.67, (_y + _sprite->height() / 2) * 0.67, 0, 0.67, 0.67);
    #else
        _sprite->pushSprite(_x, _y);
    #endif
    }
protected:
    M5Canvas *_sprite;
    int _x, _y;
};

class MultiLineTextArea 
{
public:
    MultiLineTextArea(M5GFX &display, int x, int y, int w, int h)
        : _x(x), _y(y), _w(w), _h(h), _tx(0), _ty(0)
    {
        _sprite = new M5Canvas(&display);
        _sprite->setColorDepth(8);
        _sprite->createSprite(_w, _h);
    }
    MultiLineTextArea(M5Canvas &canvas, int x, int y, int w, int h)
        : _x(x), _y(y), _w(w), _h(h), _tx(0), _ty(0)
    {
        _sprite = new M5Canvas(&canvas);
        _sprite->createSprite(_w, _h);
        _sprite->setColorDepth(8);
    }

    void print(const char *str, uint8_t color)
    {
        _sprite->setTextColor(color);
        for(int i = 0 ; i < strlen(str) ; i++)
        {
            uint8_t c = str[i];
            if (_tx + 8 > _w || c == '\n')
            {
                scrollLine();
                _tx = 0;
            }
            if (isprint(c)) _sprite->drawChar(c, _tx, _ty);
        }
    }

    void cls()
    {
        _sprite->fillRect(0, 0, _w, _h, 0);
    }

    void flush()
    {
        _sprite->pushSprite(_x, _y);
    }
protected:
    M5Canvas *_sprite;
    int _x, _y;
    int _w, _h;

    int _tx, _ty;

    int scrollLine()
    {
        uint8_t _buf[_w];
        int ly = _ty + 16;
        if (ly >= _h)
        {
            M5.Display.startWrite();
            for (int y = 0 ; y < _h - 16 ; y++)
            {
                _sprite->readRect(0, y + 16, _w, 1, _buf);
                _sprite->pushImage(0, y, _w, 1, _buf);
            }
            M5.Display.endWrite();
            ly = _ty;
        }
        _sprite->fillRect(0, ly, _w, 16, 0);
        flush();
        _ty = ly;
        _tx = 0;
        return _ty;
    }
};

#endif