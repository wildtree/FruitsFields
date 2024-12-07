//
// Map for Fruits Fields
//

#ifndef FFMAP_H
#define FFMAP_H

#include <M5Unified.h>
#include <M5GFX.h>

class FFMap
{
public:
    FFMap();
    ~FFMap();
    enum Dir { Left = 0, Right = 1, Up = 2, Down = 3 };

    uint8_t load(int scene);
    uint8_t get_id(int x, int y) const { return _field[x + 16 * y]; }
    uint8_t get(int x, int y) const {
        uint8_t v = _field[x + 16 * y];
        uint8_t r = v;
        switch (v)
        {
        case 0: r = 0; break;
        case 1: r = 1; break;
        case 2: 
            switch(_dir)
            {
            case Left: r = 6; break;
            case Right: r = 8; break;
            case Up: r = 10; break;
            case Down: r= 12; break;
            }
            break;
        case 3: r = _fruit + 14; break;
        case 4: r = 2; break;
        case 5: r = 3; break;
        case 6: r = 4; break;
        case 7: r = 5; break;
        }
        return r;
    }
    bool move(Dir d);
    bool crash(int x, int y, Dir dir);
    bool block_move(int &bx, int &by);
    uint8_t fruits() const { return _left; }
    Dir get_remkun(int &rx, int &ry) const { rx = _rx; ry = _ry; return _dir; }
    
protected:
    uint8_t *_field;
    uint8_t _fruit;
    Dir _dir;
    uint8_t _rx, _ry;
    uint8_t _left;
};

#endif