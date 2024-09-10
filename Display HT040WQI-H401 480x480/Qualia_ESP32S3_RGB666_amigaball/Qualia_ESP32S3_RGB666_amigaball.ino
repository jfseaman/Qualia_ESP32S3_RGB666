// Adapted from:
// Amiga Boing Ball Demo
// (c) 2019-20 Pawel A. Hernik
// cbm80amiga has some excellent libraries on https://github.com/cbm80amiga
// YT video: https://youtu.be/KwtkfmglT-c

// I have always been fascinated by the Amiga Ball.
// and have adapted cbm80amiga's example sketch for
// diff geometry and diff libraries
// many libraries support GFX graphics.
// but constructor, BEGIN() and DRAWIMAGE() methods vary.
// it is fairly easy to adapt for another GFX library.
// Due, ESP, Xmega ball runs faster than you want.
// but Uno is not unreasonable

#include <Arduino_GFX_Library.h>

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 480

Arduino_XCA9554SWSPI *expander = new Arduino_XCA9554SWSPI(
  PCA_TFT_RESET, PCA_TFT_CS, PCA_TFT_SCK, PCA_TFT_MOSI,
  &Wire, 0x3F);

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
  TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK,
  TFT_R1, TFT_R2, TFT_R3, TFT_R4, TFT_R5,
  TFT_G0, TFT_G1, TFT_G2, TFT_G3, TFT_G4, TFT_G5,
  TFT_B1, TFT_B2, TFT_B3, TFT_B4, TFT_B5,
  // 4.0" 720x720 round display
  1 /* hync_polarity */, 46 /* hsync_front_porch */, 2 /* hsync_pulse_width */, 44 /* hsync_back_porch */,
  1 /* vsync_polarity */, 50 /* vsync_front_porch */, 16 /* vsync_pulse_width */, 16 /* vsync_back_porch */
                                                                                     //    ,1, 30000000
);

Arduino_RGB_Display *lcd = new Arduino_RGB_Display(
  // 4" 480x480 square display from AliExpres with GT911 ts
  480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  expander, GFX_NOT_DEFINED /* RST */, st7701_type1_init_operations, sizeof(st7701_type1_init_operations));
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

#define SCR_WD SCREEN_WIDTH  //alter setRotation() for Landscape
#define SCR_HT SCREEN_HEIGHT

#define TFT_BEGIN() { lcd->begin(); lcd->setRotation(1);  }
#define DRAWIMAGE(x, y, w, h, pixels) lcd->draw16bitRGBBitmap(x, y, pixels, w, h)
#define RGBto565(r, g, b) RGB565(r, g, b)

#include "ball.h"

uint16_t palette[16];
uint16_t line[SCR_WD];
uint16_t bgCol = RGBto565(160, 160, 160);
uint16_t bgColS = RGBto565(90, 90, 90);
uint16_t lineCol = RGBto565(150, 40, 150);
uint16_t lineColS = RGBto565(80, 10, 80);

#define LINE_YS 20
#define LINE_XS1 30
#define LINE_XS2 6

#define BALL_WD 64
#define BALL_HT 64
#define BALL_SWD SCR_WD         //240
#define BALL_SHT (SCR_HT - 60)  //260

#define SP 20

#define SHADOW 10
// #define SHADOW 0

// AVR stats:
// with shadow        - 60-61ms/17fps
// without shadow     - 55-56ms/18fps

void drawBall(int x, int y) {
  static int oldx;  //C9341 41ms. D9341
  if (oldx > x) oldx = x;
  int wid = BALL_WD + SHADOW + 4;
  if (oldx + wid >= SCR_WD) wid = SCR_WD - oldx;
  int i, j, ii;
  int vlines = (SCR_WD / SP) - 2;  //10
  int hlines = (SCR_HT / SP) - 3;  //13
  for (j = 0; j < BALL_HT; j++) {
    uint8_t v, *img = (uint8_t *)ball + 16 * 2 + 6 + j * BALL_WD / 2 + BALL_WD / 2;
    int yy = y + j;
    //redraw the whole background line in the buffer.   when we only need oldx
    if (((yy - LINE_YS) % SP) == 0) {  //draw horiz graticule
      //for (i = 0; i < LINE_XS1; i++) line[i] = line[SCR_WD - 1 - i] = bgCol;
      for (i = 0; i <= SCR_WD - LINE_XS1 * 2; i++) line[i + LINE_XS1] = lineCol;
    } else {  //draw vert graticule
      for (i = 0; i < SCR_WD; i++) line[i] = bgCol;
      if (yy > LINE_YS)
        for (i = 0; i < vlines; i++) line[LINE_XS1 + i * SP] = lineCol;
    }
    for (i = BALL_WD - 2; i >= 0; i -= 2) {
      v = pgm_read_byte(--img);
      if (v >> 4) {
        line[x + i + 0] = palette[v >> 4];
#if SHADOW > 0
        ii = x + i + 0 + SHADOW;
        if (ii < SCR_WD) {
          if (line[ii] == bgCol) line[ii] = bgColS;
          else if (line[ii] == lineCol) line[ii] = lineColS;
        }
#endif
      }
      if (v & 0xf) {
        line[x + i + 1] = palette[v & 0xf];
#if SHADOW > 0
        ii = x + i + 1 + SHADOW;
        if (ii < SCR_WD) {
          if (line[ii] == bgCol) line[ii] = bgColS;
          else if (line[ii] == lineCol) line[ii] = lineColS;
        }
#endif
      }
    }
    //draws a complete horiz line. when we only need to draw ball + shadow + replace background e.g. 64+20+4
    //DRAWIMAGE(0, yy, SCR_WD, 1, line);      //C9341=64ms. D9341=104ms. A9341=125ms. 9341=112ms. 0000=69ms. DUE=9ms
    DRAWIMAGE(oldx, yy, wid, 1, line + oldx);  //C9341=41ms. D9341= 54ms. A9341= 63ms. 9341= 59ms. 0000=42ms. DUE=6ms
                                               //original cbm80amiga LIL9341_lib_AmigaBall example was 59ms.  So my "generic" edits have sacrificed efficiency
  }
  oldx = x;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(100);
  delay(1000);
  TFT_BEGIN();
  lcd->fillScreen(bgCol);
  int i, o;
  int vlines = (SCR_WD / SP) - 2;  //10
  int hlines = (SCR_HT / SP) - 3;  //13
  uint16_t *pal = (uint16_t *)ball + 3;
  for (i = 0; i < 16; i++) palette[i] = pgm_read_word(&pal[i]);
  for (i = 0; i < vlines; i++) lcd->drawFastVLine(LINE_XS1 + i * SP, LINE_YS, (hlines - 1) * SP, lineCol);
  for (i = 0; i < hlines; i++) lcd->drawFastHLine(LINE_XS1, LINE_YS + i * SP, SCR_WD - LINE_XS1 * 2, lineCol);
  lcd->drawFastHLine(LINE_XS2, SCR_HT - LINE_YS, SCR_WD - LINE_XS2 * 2, lineCol);
  int dy = SCR_HT - LINE_YS - (LINE_YS + SP * (hlines - 1));
  int dx = LINE_XS1 - LINE_XS2;
  o = 2 * 7 * dx / dy;
  lcd->drawFastHLine(LINE_XS2 + o, SCR_HT - LINE_YS - 7 * 2, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);
  o = 2 * (7 + 6) * dx / dy;
  lcd->drawFastHLine(LINE_XS2 + o, SCR_HT - LINE_YS - (7 + 6) * 2, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);
  o = 2 * (7 + 6 + 4) * dx / dy;
  lcd->drawFastHLine(LINE_XS2 + o, SCR_HT - LINE_YS - (7 + 6 + 4) * 2, SCR_WD - LINE_XS2 * 2 - o * 2, lineCol);
  for (i = 0; i < vlines; i++) lcd->drawLine(LINE_XS1 + i * SP, LINE_YS + SP * (hlines - 1), LINE_XS2 + i * (SCR_WD - LINE_XS2 * 2) / (vlines - 1), SCR_HT - LINE_YS, lineCol);
  //delay(10000);
}

int anim = 0, animd = 2;
int x = 0, y = 0;
int xd = 4, yd = 1;
unsigned long ms;

void loop() {
  ms = millis();
  for (int i = 0; i < 14; i++) {
    palette[i + 1] = ((i + anim) % 14) < 7 ? WHITE : RED;
    //int c=((i+anim)%14); // with pink between white and red
    //if(c<6) palette[i+1]=WHITE; else if(c==6 || c==13) palette[i+1]=RGBto565(255,128,128); else palette[i+1]=RED;
  }
  drawBall(x, y);
  anim += animd;
  if (anim < 0) anim += 14;
  x += xd;
  y += yd;
  if (x < 0) {
    x = 0;
    xd = -xd;
    animd = -animd;
  }
  if (x >= BALL_SWD - BALL_WD) {
    x = BALL_SWD - BALL_WD;
    xd = -xd;
    animd = -animd;
  }
  if (y < 0) {
    y = 0;
    yd = -yd;
  }
  if (y >= BALL_SHT - BALL_HT) {
    y = BALL_SHT - BALL_HT;
    yd = -yd;
  }
  delay(10);
  //ms = millis() - ms; Serial.println(ms); delay(500);
}
