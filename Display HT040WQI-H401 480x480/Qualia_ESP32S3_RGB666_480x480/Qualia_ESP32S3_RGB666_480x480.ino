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
// #include <Adafruit_FT6206.h>
// #include <Adafruit_CST8XX.h>
#include <TAMC_GT911.h>

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 480
#define TOUCH_SDA SDA
#define TOUCH_SCL SCL
#define TOUCH_INT 16
#define TOUCH_RST -1

int foo = PCA

Arduino_XCA9554SWSPI *expander = new Arduino_XCA9554SWSPI(
  PCA_TFT_RESET, PCA_TFT_CS, PCA_TFT_SCK, PCA_TFT_MOSI,
  &Wire, 0x3F);

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
  TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK,
  TFT_R1, TFT_R2, TFT_R3, TFT_R4, TFT_R5,
  TFT_G0, TFT_G1, TFT_G2, TFT_G3, TFT_G4, TFT_G5,
  TFT_B1, TFT_B2, TFT_B3, TFT_B4, TFT_B5,
  // 1 /* hsync_polarity */, 50 /* hsync_front_porch */, 2 /* hsync_pulse_width */, 44 /* hsync_back_porch */,
  // 1 /* vsync_polarity */, 16 /* vsync_front_porch */, 2 /* vsync_pulse_width */, 18 /* vsync_back_porch */
  // 4.0" 720x720 round display
  1 /* hync_polarity */, 46 /* hsync_front_porch */, 2 /* hsync_pulse_width */, 44 /* hsync_back_porch */,
  1 /* vsync_polarity */, 50 /* vsync_front_porch */, 16 /* vsync_pulse_width */, 16 /* vsync_back_porch */
                                                                                     //    ,1, 30000000
);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  // 2.1" 480x480 round display
  //  480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  //  expander, GFX_NOT_DEFINED /* RST */, TL021WVC02_init_operations, sizeof(TL021WVC02_init_operations));

  // 2.8" 480x480 round display
  //  480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  //  expander, GFX_NOT_DEFINED /* RST */, TL028WVC01_init_operations, sizeof(TL028WVC01_init_operations));

  // 3.4" 480x480 square display
  //  480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  //  expander, GFX_NOT_DEFINED /* RST */, tl034wvs05_b1477a_init_operations, sizeof(tl034wvs05_b1477a_init_operations));

  // 4" 480x480 square display from AliExpres with GT911 ts
  480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  expander, GFX_NOT_DEFINED /* RST */, st7701_type1_init_operations, sizeof(st7701_type1_init_operations));

// 3.2" 320x820 rectangle bar display
//    320 /* width */, 820 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
//    expander, GFX_NOT_DEFINED /* RST */, tl032fwv01_init_operations, sizeof(tl032fwv01_init_operations));

// 4.0" 720x720 square display
//    720 /* width */, 720 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
//    expander, GFX_NOT_DEFINED /* RST */, NULL, 0);

// 4.0" 720x720 round display
// SCREEN_WIDTH /* width */, SCREEN_HEIGHT /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
// expander, GFX_NOT_DEFINED /* RST */, hd40015c40_init_operations, sizeof(hd40015c40_init_operations));
// needs also the rgbpanel to have these pulse/sync values:
//    1 /* hync_polarity */, 46 /* hsync_front_porch */, 2 /* hsync_pulse_width */, 44 /* hsync_back_porch */,
//    1 /* vsync_polarity */, 50 /* vsync_front_porch */, 16 /* vsync_pulse_width */, 16 /* vsync_back_porch */

uint16_t *colorWheel;

// The Capacitive touchscreen overlays uses hardware I2C (SCL/SDA)

// Most touchscreens use FocalTouch with I2C Address often but not always 0x48!
// #define I2C_TOUCH_ADDR 0x48

// 2.1" 480x480 round display use CST826 touchscreen with I2C Address at 0x15
// #define I2C_TOUCH_ADDR 0x15  // often but not always 0x48!
#define I2C_TOUCH_ADDR 0x14  // GT911 touch on 480x480 board

// Adafruit_FT6206 focal_ctp = Adafruit_FT6206();  // this library also supports FT5336U!
// Adafruit_CST8XX cst_ctp = Adafruit_CST8XX();
TAMC_GT911 tp = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, SCREEN_WIDTH, SCREEN_HEIGHT);

bool touchOK = false;  // we will check if the touchscreen exists
bool isFocalTouch = false;

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
  Serial.println("Initialized!");
  Serial.println("Begin drawing screen.");
  gfx->fillScreen(BLACK);
  Serial.println("Turn on backlight.");
  expander->pinMode(PCA_TFT_BACKLIGHT, OUTPUT);
  expander->digitalWrite(PCA_TFT_BACKLIGHT, HIGH);
  Serial.println("Draw color wheel.");
  colorWheel = (uint16_t *)ps_malloc(gfx->width() * gfx->height() * sizeof(uint16_t));
  if (colorWheel) {
    generateColorWheel(colorWheel);
    gfx->draw16bitRGBBitmap(0, 0, colorWheel, gfx->width(), gfx->height());
  }
  Serial.println("Drawing done.");
  Serial.println("Setting up touch screen.");
  tp.begin(I2C_TOUCH_ADDR);
  tp.setRotation(ROTATION_NORMAL); // This also serves as an internal TP reset so there are not floating values on loop start.
  Serial.println("Done.");
  touchOK = true;
  // if (!focal_ctp.begin(0, &Wire, I2C_TOUCH_ADDR)) {
  //   // Try the CST826 Touch Screen
  //   if (!cst_ctp.begin(&Wire, I2C_TOUCH_ADDR)) {
  //     Serial.print("No Touchscreen found at address 0x");
  //     Serial.println(I2C_TOUCH_ADDR, HEX);
  //     touchOK = false;
  //   } else {
  //     Serial.println("CST826 Touchscreen found");
  //     touchOK = true;
  //     isFocalTouch = false;
  //   }
  // } else {
  //   Serial.println("Focal Touchscreen found");
  //   touchOK = true;
  //   isFocalTouch = true;
  // }
}

void loop() {
  if (touchOK) {
    // if (isFocalTouch && focal_ctp.touched()) {
    //   TS_Point p = focal_ctp.getPoint(0);
    //   Serial.printf("(%d, %d)\n", p.x, p.y);
    //   gfx->fillRect(p.x, p.y, 5, 5, WHITE);
    // } else if (!isFocalTouch && cst_ctp.touched()) {
    //   CST_TS_Point p = cst_ctp.getPoint(0);
    //   Serial.printf("(%d, %d)\n", p.x, p.y);
    //   gfx->fillRect(p.x, p.y, 5, 5, WHITE);
    // }
    tp.read();
    if (tp.isTouched) {
      for (int i = 0; i < tp.touches; i++) {
        Serial.print("Touch ");
        Serial.print(i + 1);
        Serial.print(": ");
        ;
        Serial.print("  x: ");
        Serial.print(tp.points[i].x);
        Serial.print("  y: ");
        Serial.print(tp.points[i].y);
        Serial.print("  size: ");
        Serial.println(tp.points[i].size);
        Serial.println(' ');
      }
    }
  }

  // use the buttons to turn off
  if (!expander->digitalRead(PCA_BUTTON_DOWN)) {
    expander->digitalWrite(PCA_TFT_BACKLIGHT, LOW);
  }
  // and on the backlight
  if (!expander->digitalRead(PCA_BUTTON_UP)) {
    expander->digitalWrite(PCA_TFT_BACKLIGHT, HIGH);
  }
}

// https://chat.openai.com/share/8edee522-7875-444f-9fea-ae93a8dfa4ec
void generateColorWheel(uint16_t *colorWheel) {
  int width = gfx->width();
  int height = gfx->height();
  int half_width = width / 2;
  int half_height = height / 2;
  float angle;
  uint8_t r, g, b;
  int index, scaled_index;

  for (int y = 0; y < half_height; y++) {
    for (int x = 0; x < half_width; x++) {
      index = y * half_width + x;
      angle = atan2(y - half_height / 2, x - half_width / 2);
      r = uint8_t(127.5 * (cos(angle) + 1));
      g = uint8_t(127.5 * (sin(angle) + 1));
      b = uint8_t(255 - (r + g) / 2);
      uint16_t color = RGB565(r, g, b);

      // Scale this pixel into 4 pixels in the full buffer
      for (int dy = 0; dy < 2; dy++) {
        for (int dx = 0; dx < 2; dx++) {
          scaled_index = (y * 2 + dy) * width + (x * 2 + dx);
          colorWheel[scaled_index] = color;
        }
      }
    }
  }
}
