
#include <M5Unified.h>
#include <M5GFX.h>

#include <game.h>
#include <keyboard.h>
#include <textarea.h>


uint8_t Game::_scene = 0;
uint8_t Game::_mode = 0;
KeyBoard *Game::_keyboard = nullptr;

TextArea *Game::_header = nullptr;
TextArea *Game::_status = nullptr;

Sprite *Game::_rem[8];;
Sprite *Game::_blocks[4];
Sprite *Game::_blank;
Sprite *Game::_wall;

bool Game::in_play = false;

static void IRAM_ATTR
on_timer_cb()
{
    Game::instance().on_timer();
}

float Game::bgm[] = {
    476.32395038167937,476.32395038167937,476.32395038167937,476.32395038167937,476.32395038167937,434.8323170731707,406.50447882736154,379.3218085106383,406.50447882736154,406.50447882736154,406.50447882736154,406.50447882736154,406.50447882736154,379.3218085106383,340.97506830601094,465.65998134328356,476.32395038167937,476.32395038167937,476.32395038167937,476.32395038167937,476.32395038167937,434.8323170731707,406.50447882736154,379.3218085106383,406.50447882736154,406.50447882736154,406.50447882736154,406.50447882736154,406.50447882736154,379.3218085106383,340.97506830601094,465.65998134328356,465.65998134328356,340.97506830601094,379.3218085106383,340.97506830601094,465.65998134328356,340.97506830601094,379.3218085106383,406.50447882736154,340.97506830601094,379.3218085106383,406.50447882736154,379.3218085106383,340.97506830601094,379.3218085106383,406.50447882736154,476.32395038167937,476.32395038167937,476.32395038167937,476.32395038167937,476.32395038167937,476.32395038167937,434.8323170731707,406.50447882736154,379.3218085106383,406.50447882736154,379.3218085106383,340.97506830601094,406.50447882736154,379.3218085106383,487.48779296875,487.48779296875, 0.0
};
float *Game::bgmp = bgm;
uint16_t Game::bgm_cnt = 0;
bool Game::_bgm = true;

uint8_t Game::_demo_keys[] = {
    0x1d, 0x1d, ' ', 0x1d, 0x1d, 0x1d, 0x1e, 0x1e, 0x1e, 0x1c, ' ', 0x1c, 0x1e, 0x1f, 0x1f, 0x1c, 0x1e, ' ', 0x1c, 0x1e, ' ', 0x1e, 0x1e, 0x1f, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1f, 0x1f, 0x1d, 0x1e, ' ', 0x1e, 0x1d, 0x1c, 0x1c, 0x1e, 0x1d, ' ', 0x1e, 0x1d, ' ', 0x1d, 0x1d,  0,
};
uint8_t *Game::_demo_key = nullptr;

static hw_timer_t *_timer = nullptr;

Game::Game()
{
    _scene = 0;
    _header = new TextArea(M5.Display, 0, 0);
    _status = new TextArea(M5.Display, 0, 224);
    _status->print("Connecting to BLE keyboard", 0x03, TextArea::Left);
    _status->flush();
    _keyboard = new BTKeyBoard();
    _status->cls();
    _status->flush();
    for (int i = 0 ; i < 8 ; i++)
    {
        _rem[i] = Sprites::instance().get(6 + i);
    }
    for (int b = 0 ; b < 4 ; b++)
    {
        _blocks[b] = Sprites::instance().get(2 + b);
    }
    _blank = Sprites::instance().get(0);
    _wall = Sprites::instance().get(1);
    Serial.println("Setup timer interrupt.");
    _timer = timerBegin(0, 80, true); // timer #0, 80MHz
    if (_timer)
    {
        timerAttachInterrupt(_timer, &on_timer_cb, true);
        timerAlarmWrite(_timer, 100000, true);  // each 0.1s (100ms)
        timerAlarmEnable(_timer);
        Serial.println("timer is started.");
    }
    _mode = 100; // title
}

Game::~Game()
{
    delete _header;
    delete _status;
    delete _keyboard;
}

void
Game::loop()
{
    switch (_mode)
    {
    case 0:
        stage_select();
        break;
    case 1:
        play_game();
        break;
    case 100:
        title();
        break;
    default:
        break;
    }
}

void
Game::title()
{
    Sprites::instance().draw_title();
    TextArea line(M5.Display, 0, 208);
    line.print("Hit [Space] key to start.", 0x1f, TextArea::Center);
    line.flush();
    Sprites::instance().demo_map();

    // demp loop
    for (_demo_key = _demo_keys ; *_demo_key !=0 ; _demo_key++)
    {
        uint8_t c;
        while(_keyboard->fetch_key(c))
        {
            if (c == ' ') _mode = 0;
            M5.Display.clear(BLACK);
            return;
        }
        //
        int bx, by, cx, cy, nx, ny, dx ,dy, d, id;
        bool walk = false;
        FFMap::Dir dir = Sprites::instance().map()->get_remkun(cx, cy);
        nx = cx;
        ny = cy;
        int fruits = Sprites::instance().map()->fruits();
        switch (*_demo_key)
        {
        case 0x1c: // left
            d = 0;
            walk = true;
            Sprites::instance().map()->move(FFMap::Left);
            break;
        case 0x1d: // right
            d = 1;
            walk = true;
            Sprites::instance().map()->move(FFMap::Right);
            break;
        case 0x1e: // up
            d = 2;
            walk = true;
            Sprites::instance().map()->move(FFMap::Up);
            break;
        case 0x1f: // down
            d = 3;
            walk = true;
            Sprites::instance().map()->move(FFMap::Down);
            break;
        case ' ': // hit space
            block_check(cx, cy, dir, 16, 32);
            break;
        }
        if (walk)
        {
            if (fruits != Sprites::instance().map()->fruits())
            {
                M5.Speaker.tone(441, 32);
            }
            Sprites::instance().map()->get_remkun(nx, ny);
            Sprite *r = _rem[d * 2];
            Sprite *h = _rem[d * 2 + 1];
            dx = (nx - cx);
            dy = (ny - cy);
            _blank->draw(M5.Display, 16 + cx * 8, 32 + cy * 8, 2.0);
            h->draw(M5.Display, 16 + cx * 8 + dx * 4, 32 + cy * 8 + dy * 4, 2.0);
            usleep(100000); //timing -- 0.1sec
            _blank->draw(M5.Display, 16 + cx * 8 + dx * 4, 32 + cy * 8 + dy * 4, 2.0);
            r->draw(M5.Display, 16 + nx * 8, 32 + ny * 8, 2.0);
        }
        usleep(100000);
    }
    usleep(1000000);
}

void
Game::stage_select()
{
    Sprites::instance().draw_title();
    Sprites::instance().init_fields();

    TextArea line1(M5.Display, 32, 120);
    TextArea line2(M5.Display, 32, 152);
    TextArea line3(M5.Display, 32, 184);

    line1.print("Select Starting Phase", 0x1f, TextArea::Center);
    char s[5];
    snprintf(s, 5, "[%02d]", _scene);
    line2.print(s, 0x1f, TextArea::Center);
    line3.print("Hit [Space] Key to Start", 0x1f, TextArea::Center);

    line1.flush();
    line2.flush();
    line3.flush();

    uint8_t c;
    while (_keyboard->fetch_key(c))
    {
        switch(c)
        {
        case ' ': // game start.
            _mode = 1;
            Sprites::instance().init_fields();
            Sprites::instance().load_map(_scene);
            Sprites::instance().flush();
            break;
        case 0x1c: // left
        case 0x1f: // down
            if (_scene-- == 0) _scene = 99;
            break;
        case 0x1d: // right
        case 0x1e: // up
            if (_scene++ == 99) _scene = 0;
            break;
        }
    }
}

void
Game::draw_header(int scene, int fruits)
{
    char s[40];
    snprintf(s, 40, "Phase %02d   Left Fruits %3d", scene, fruits);
    _header->cls();
    _header->print(s, 0x1f, TextArea::Center);
    _header->flush();
}

void
Game::give_up()
{
    TextArea line1(M5.Display, 32, 72);
    TextArea line2(M5.Display, 32, 136);
    TextArea line3(M5.Display, 32, 152);

    line1.print("** Give Up **", 0xe3, TextArea::Center);
    line2.print("Hit [Y] key if you want to cont", 0xe3, TextArea::Center);
    line3.print("Otherwise hit [Space] key", 0xe3, TextArea::Center);

    line1.flush();
    line2.flush();
    line3.flush();

    while (true)
    {
        uint8_t c;
        if (!_keyboard->fetch_key(c)) continue;
        if (c == 'y'||c == 'Y')
        {
            Sprites::instance().init_fields();
            Sprites::instance().load_map(_scene);
            Sprites::instance().flush();
            break;
        }
        else if(c == ' ')
        {
            _mode = 100;
            M5.Display.fillRect(0, 0, M5.Display.width(), M5.Display.height(), 0);
            break;
        }
    }
}

void
Game::block_check(int bx, int by, FFMap::Dir dir, int ox, int oy)
{
    int nx = bx;
    int ny = by;
    // Serial.printf("(bx, by, dir) = (%d, %d, %d)\r\n", bx, by, dir);
    switch(dir)
    {
    case FFMap::Left:
        if (bx == 0) return;
        --nx;
        break;
    case FFMap::Right:
        if (bx == 15) return;
        ++nx;
        break;
    case FFMap::Up:
        if (by == 0) return;
        --ny;
        break;
    case FFMap::Down:
        if (by == 9) return;
        ++ny;
        break;
    }
    uint8_t c = Sprites::instance().map()->get_id(nx, ny);
    // Serial.printf("(nx, ny, c) = (%d, %d, %d)\r\n", nx, ny, c);
    if (c < 4||c > 7) return;
    Sprite *block = _blocks[c - 4];
    if (Sprites::instance().map()->crash(nx, ny, dir))
    {
        M5.Speaker.tone(330,32);
        _blank->draw(M5.Display, ox + nx * 8, oy + ny * 8, 2.0);
    }
    else
    {
        int px = nx;
        int py = ny;
        bool beep = true;
        while (Sprites::instance().map()->block_move(nx, ny))
        {
            if (beep)
            {
                M5.Speaker.tone(330,32);
                beep = false;
            }
            // Serial.printf("block move loop: (nx, ny) = (%d, %d)\r\n", nx, ny);
            _blank->draw(M5.Display, ox + px * 8, oy + py * 8, 2.0);
            block->draw(M5.Display, ox + nx * 8, oy + ny * 8, 2.0);
            px = nx;
            py = ny;
        }
    }
}

void
Game::stage_clear()
{
    TextArea line1(M5.Display, 32, 72);
    TextArea line2(M5.Display, 32, 104);
    TextArea line3(M5.Display, 32, 136);

    line1.print("Nice Play!", 0x1f, TextArea::Center);
    line2.print("You have cleared this phase.", 0x1f, TextArea::Center);
    line3.print("Hit [Space] key to try next", 0x1f, TextArea::Center);

    line1.flush();
    line2.flush();
    line3.flush();
    
    while(true)
    {
        uint8_t c;
        if (!_keyboard->fetch_key(c)) continue;
        if (c == ' ')
        {
            if (++_scene == 100) _scene = 0;
            Sprites::instance().init_fields();
            Sprites::instance().load_map(_scene);
            Sprites::instance().flush();
            break;
        }
    }
}

void
Game::play_game()
{
    draw_header(_scene, Sprites::instance().map()->fruits());
    uint8_t c;
    in_play = true;
    while(_keyboard->fetch_key(c))
    {
        int bx, by, cx, cy, nx, ny, dx ,dy, d, id;
        bool walk = false;
        //Sprite *blank = Sprites::instance().get(0);
        FFMap::Dir dir = Sprites::instance().map()->get_remkun(cx, cy);
        nx = cx;
        ny = cy;
        int fruits = Sprites::instance().map()->fruits();
        switch (c)
        {
        case 0x1b: // ESC
            in_play = false;
            bgmp = bgm;
            give_up();
            break;
        case 's':
        case 'S':
            _bgm = !_bgm;   // flip BGM mode
            if (_bgm) bgm_rewind();
            break;
        case 0x1c: // left
            d = 0;
            walk = true;
            Sprites::instance().map()->move(FFMap::Left);
            break;
        case 0x1d: // right
            d = 1;
            walk = true;
            Sprites::instance().map()->move(FFMap::Right);
            break;
        case 0x1e: // up
            d = 2;
            walk = true;
            Sprites::instance().map()->move(FFMap::Up);
            break;
        case 0x1f: // down
            d = 3;
            walk = true;
            Sprites::instance().map()->move(FFMap::Down);
            break;
        case ' ': // hit space
            block_check(cx, cy, dir);
            break;
        }
        if (walk)
        {
            if (fruits != Sprites::instance().map()->fruits())
            {
                M5.Speaker.tone(441, 32);
            }
            Sprites::instance().map()->get_remkun(nx, ny);
            Sprite *r = _rem[d * 2];
            Sprite *h = _rem[d * 2 + 1];
            dx = (nx - cx);
            dy = (ny - cy);
            _blank->draw(M5.Display, 16 + cx * 8, 16 + cy * 8, 2.0);
            h->draw(M5.Display, 16 + cx * 8 + dx * 4, 16 + cy * 8 + dy * 4, 2.0);
            // Serial.println("half step.");
            usleep(100000); //timing -- 0.1sec
            _blank->draw(M5.Display, 16 + cx * 8 + dx * 4, 16 + cy * 8 + dy * 4, 2.0);
            r->draw(M5.Display, 16 + nx * 8, 16 + ny * 8, 2.0);
        }
        if (Sprites::instance().map()->fruits() == 0)
        {
            in_play = false;
            bgmp = bgm;
            stage_clear();
        }
    }
}

void
Game::bgm_play()
{
    if (!_bgm) return;
    if (bgm_cnt++ == 2)
    {
        bgm_cnt  = 0;
        float tone = *bgmp++;
        if (tone == 0.0) {
            bgm_rewind();
            tone = *bgmp++;
        }
        M5.Speaker.tone(tone, 100);
    }
}

void
Game::on_timer()
{
    if (_keyboard != nullptr) _keyboard->on_timer();
    if (in_play) bgm_play();
}
