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

#ifndef OX
#define OX (16)
#endif
#ifndef OY
#define OY (16)
#endif
#ifndef SCALE
#define SCALE (2.0f)
#endif


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
    static void block_check(int bx, int by, FFMap::Dir dir, int ox = OX, int oy = OY);
    static void bgm_rewind() { bgmp = bgm; }
    static void bgm_play();
    static void title();
    static void demo1();
    static void demo2();
    static uint8_t _scene;
    static KeyBoard *_keyboard;
    static uint8_t _mode;
    static TextArea *_header, *_status;
    static Sprite *_rem[8], *_blocks[4], *_blank, *_wall;
    static bool in_play;
    static float bgm[];
    static float *bgmp;
    static uint16_t bgm_cnt;
    static bool _bgm;
    static uint8_t _demo_keys[];
    static uint8_t *_demo_key;
    static uint8_t _demo_mode;
};

#endif