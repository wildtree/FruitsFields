//
// Game routine
//
#ifndef GAME_H
#define GAME_H

#include <M5Unified.h>
#include <M5GFX.h>

#include <sprite.h>
#include <textarea.h>
#include <ffmap.h>
#include <keyboard.h>

class Game
{
public:
    static Game &instance() { static Game g; return g; }
    static void loop();
    static void on_timer();
protected:
    Game();
    ~Game();
    static void stage_select();
    static void play_game();
    static void draw_header(int scene, int fruits);
    static void give_up();
    static void stage_clear();
    static void block_check(int bx, int by, FFMap::Dir dir);
    static uint8_t _scene;
    static KeyBoard *_keyboard;
    static uint8_t _mode;
    static TextArea *_header, *_status;
    static Sprite *_rem[8], *_blocks[4], *_blank, *_wall;
};

#endif