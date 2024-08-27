// SPDX-FileCopyrightText: 2023 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

/*
GFX 1.4.7 there is no support for Arduino_ESP32RGBPanel in "GFX Library for Arduino"
for Boards from 3.0.0 on so 
be sure to revert to 2.0.17 of boards.
It will take a couple itterations through bootsel/port to get the coms stabalized.
*/

#include <Arduino_GFX_Library.h>

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720

Arduino_XCA9554SWSPI *expander = new Arduino_XCA9554SWSPI(
  PCA_TFT_RESET, PCA_TFT_CS, PCA_TFT_SCK, PCA_TFT_MOSI,
  &Wire, 0x3F);

// 4.0" 720x720 round display
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
  TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK,
  TFT_R1, TFT_R2, TFT_R3, TFT_R4, TFT_R5,
  TFT_G0, TFT_G1, TFT_G2, TFT_G3, TFT_G4, TFT_G5,
  TFT_B1, TFT_B2, TFT_B3, TFT_B4, TFT_B5,
  1 /* hync_polarity */, 46 /* hsync_front_porch */, 2 /* hsync_pulse_width */, 44 /* hsync_back_porch */,
  1 /* vsync_polarity */, 50 /* vsync_front_porch */, 16 /* vsync_pulse_width */, 16 /* vsync_back_porch */
                                                                                     //    ,1, 30000000
);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  SCREEN_WIDTH /* width */, SCREEN_WIDTH /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  expander, GFX_NOT_DEFINED /* RST */, hd40015c40_init_operations, sizeof(hd40015c40_init_operations));
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/
 
int times_to_test = 1000;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(100);
  delay(500);

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  Serial.println("Beginning");
  // Init Display

  Wire.setClock(1000000);  // speed up I2C
  Serial.println("Initialize GFX.");
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);
  Serial.println("Initialized!");
}

void loop() {
  // use the buttons to turn off
  if (!expander->digitalRead(PCA_BUTTON_DOWN)) {
    expander->digitalWrite(PCA_TFT_BACKLIGHT, LOW);
  }
  // and on the backlight
  if (!expander->digitalRead(PCA_BUTTON_UP)) {
    expander->digitalWrite(PCA_TFT_BACKLIGHT, HIGH);
  }
  if (times_to_test == 1000) {
    Serial.println("Begin drawing screen.");
  }
  while (times_to_test) {
    gfx->setTextColor(GREEN);
    for (int x = 0; x < 16; x++) {
      gfx->setCursor(10 + x * 8, 2);
      gfx->print(x + 25, 16);
    }
    gfx->setTextColor(BLUE);
    for (int y = 0; y < 16; y++) {
      gfx->setCursor(2, 12 + y * 10);
      gfx->print(y + 27, 16);
    }

    for (int x = 0; x < 16; x++) {
      gfx->setCursor(10 + x * 8, 2);
      gfx->print(x + 45, 16);
    }
    gfx->setTextColor(BLUE);
    for (int y = 0; y < 16; y++) {
      gfx->setCursor(2, 12 + y * 10);
      gfx->print(y + 27, 16);
    }

    char c = 0;
    for (int y = 0; y < 16; y++) {
      for (int x = 0; x < 16; x++) {
        gfx->drawChar(10 + (x + 25) * 8, 12 + (y + 27) * 10, c++, WHITE, BLACK);
      }
    }

    c = 0;
    for (int y = 0; y < 16; y++) {
      for (int x = 0; x < 16; x++) {
        gfx->drawChar(10 + (x + 45) * 8, 12 + (y + 27) * 10, c++, WHITE, BLACK);
      }
    }

    --times_to_test;
  }
  if (times_to_test == 0) {
    Serial.println("Drawing done.");
    --times_to_test;
  }
}
