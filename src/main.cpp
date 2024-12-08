#include <Arduino.h>

#include <M5Unified.h>
#include <M5GFX.h>

#include <game.h>
#include <sprite.h>
#include <ffmap.h>
#include <textarea.h>
#include <keyboard.h>

static hw_timer_t *_timer = nullptr;

void 
setup() 
{
  // put your setup code here, to run once:
  auto cfg = M5.config();
  cfg.clear_display = true;
  M5.begin(cfg);
  M5.Display.setRotation(1);
  M5.Display.clear(BLACK);

  Serial.begin(115200);
  Serial.println("Fruits Fields for M5Stack");

  M5.Speaker.begin();
  M5.Speaker.setVolume(128);
}

void 
loop() 
{
  M5.update();
  Game::instance().loop();
}
