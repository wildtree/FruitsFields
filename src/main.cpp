#include <Arduino.h>

#include <M5Unified.h>
#include <M5GFX.h>

#include <game.h>
#include <sprite.h>
#include <ffmap.h>
#include <textarea.h>
#include <keyboard.h>

#ifdef M5ATOM_LITE
#include <m5gfx_st7789.h>
M5GFX_ST7789 extDisplay;
#endif
#ifdef M5STACK
#include <Wire.h>
#include <SD.h>
#endif

static hw_timer_t *_timer = nullptr;

void 
setup() 
{
  // put your setup code here, to run once:
  auto cfg = M5.config();
  cfg.clear_display = true;
  M5.begin(cfg);
#ifdef M5ATOM_LITE
  extDisplay.initExt();
  M5.addDisplay(extDisplay);
#endif
  M5.Display.setRotation(ROTATION);
  M5.Display.clear(BLACK);

#ifdef M5STACK
  uint8_t ssPin = M5.getPin(m5::pin_name_t::sd_spi_ss);
  while (false == SD.begin(ssPin /*GPIO_NUM_4*/, SPI, 25000000))
  {
    M5.Display.println("SD Wait ...");
    delay(500);
  }
#endif
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
