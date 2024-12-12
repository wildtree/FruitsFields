//
// Characters for Fruits Fields
//

#ifndef SPRITE_H
#define SPRITE_H

#include <M5Unified.h>
#include <M5GFX.h>

#include <ffmap.h>

class Sprite
{
protected:
    M5Canvas _sprite; // スプライト

    void init(const uint8_t *bitmap)
    {
        _sprite.createSprite(8, 8);
        _sprite.setColorDepth(8);
        if (bitmap)
        {
            for (int y = 0 ; y < 8 ; y++)
            {
                for (int x = 0 ; x < 8 ; x++)
                {
                    _sprite.setColor(bitmap[x + 8 * y]);
                    _sprite.drawPixel(x, y);
                }
            }
        }
    }

public:
    Sprite(M5Canvas &screen, const uint8_t *bitmap = nullptr) : _sprite(&screen) {
        init(bitmap);
    }
    Sprite(M5GFX &display, const uint8_t *bitmap = nullptr) : _sprite(&display) {
        init(bitmap);
    }
    ~Sprite() {
        _sprite.deletePalette();
    }

    inline void draw(int x, int y, float scale = 1.0f)
    {
        if (scale == 1.0f)
        {
            _sprite.pushSprite(x, y);
        }
        else
        {
            // scale するときは、指定した座標を中心にして描画されるので、オフセットしてやらないと狙った場所に描画されない。
            _sprite.pushRotateZoomWithAA((float)(x + 4) * scale, (float)(y + 4) * scale, 0, scale, scale);
        }
    }
    inline void draw(M5GFX &display, int x, int y, float scale = 1.0f)
    {
        if (scale == 1.0f)
        {
            _sprite.pushSprite(&display, x, y);
        }
        else
        {
            _sprite.pushRotateZoomWithAA(&display, (float)(x + 4) * scale, (float)(y + 4) * scale, 0, scale, scale);
        }
    }
    inline void draw(M5Canvas &canvas, int x, int y, float scale = 1.0f)
    {
        if (scale == 1.0f)
        {
            _sprite.pushSprite(&canvas, x, y);
        }
        else
        {
            _sprite.pushRotateZoomWithAA(&canvas, (float)(x + 4) * scale, (float)(y + 4) * scale, 0, scale, scale);
        }
    }
};

class Sprites
{
public:
    static const int num_sprites = 34;
    static Sprites &instance() { static Sprites instance; return instance; }
    static Sprite *get(int id) { return _sprite[id]; }
    static void flush() { 
#ifdef M5ATOM_LITE
        _canvas->pushRotateZoomWithAA(120.0f, VOFST + 84.0f, 0.0f, 1.5f, 1.5f);
#else
        _canvas->pushRotateZoomWithAA(160.0f, 112.0f, 0.0f, 2.0f, 2.0f); 
#endif
    }
    static void init_fields() {
        _canvas->fillRect(0, 0, _canvas->width(), _canvas->height(), 0x00); // clear buffer
        for (int x = 0 ; x < 18 ; x++)
        {
            _sprite[1]->draw(8 + 8 * x, 0);
            _sprite[1]->draw(8 + 8 * x, 88);
        }
        for (int y = 0 ; y < 10 ; y++)
        {
            _sprite[1]->draw(8, 8 + 8 * y);
            _sprite[1]->draw(144, 8 + 8 * y);
        }
    }
    static uint8_t load_map(int scene)
    {
        uint8_t f = _map->load(scene);
        for (int x = 0 ; x < 16 ; x++)
        {
            for (int y = 0 ; y < 10 ; y++)
            {
                _sprite[_map->get(x, y)]->draw(16 + x * 8, 8 + y * 8);
            }
        }
        return f;
    }
    static uint8_t demo_map()
    {
        uint8_t f = _map->load(100);
        for (int x = 0 ; x < 16 ; x++)
        {
            for (int y = 0 ; y < 6 ; y++)
            {
                _sprite[_map->get(x, y)]->draw(M5.Displays(0), OX + x * 8, VOFST + OY * 2 + y * 8, SCALE);
            }
        }
        return f;
    }
    static FFMap *map() { return _map; }
    static void draw_title() {
#ifdef M5ATOM_LITE
        _title->pushRotateZoomWithAA(160 * 0.67, (VOFST + 25) * 0.67, 0, 0.67, 0.67);
#else
        _title->pushSprite(120,0); 
#endif
    }
protected:
    static Sprite *_sprite[num_sprites];
    static M5Canvas *_canvas;
    static FFMap *_map;
    static M5Canvas *_title;
    Sprites();
    ~Sprites();
};

#endif