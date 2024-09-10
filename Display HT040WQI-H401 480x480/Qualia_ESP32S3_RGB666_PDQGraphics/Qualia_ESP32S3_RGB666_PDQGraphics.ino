/*
  Adapted from the Adafruit and Xark's PDQ graphicstest sketch.

  See end of file for original header text and MIT license info.
*/

/*******************************************************************************
 * Start of Arduino_GFX setting
 ******************************************************************************/
#include <Arduino_GFX_Library.h>
#include <Adafruit_GFX.h>

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

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  // 4" 480x480 square display from AliExpres with GT911 ts
  480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  expander, GFX_NOT_DEFINED /* RST */, st7701_type1_init_operations, sizeof(st7701_type1_init_operations));
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

#ifdef ESP32
#undef F
#define F(s) (s)
#endif

int32_t w, h, n, n1, cx, cy, cx1, cy1, cn, cn1;
uint8_t tsa, tsb, tsc, ds;

#define TEXT_X 10
#define TEXT_Y 10

void setup()
{
  Serial.begin(115200);
  while (!Serial) delay(100);
  delay(1000);
  Serial.println("Arduino_GFX PDQgraphicstest example!");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  if (!gfx->begin())
  // if (!gfx->begin(80000000)) /* specify data bus speed */
  {
    Serial.println("gfx->begin() failed!");
  }

  w = gfx->width();
  h = gfx->height();
  n = min(w, h);
  n1 = n - 1;
  cx = w / 2;
  cy = h / 2;
  cx1 = cx - 1;
  cy1 = cy - 1;
  cn = min(cx1, cy1);
  cn1 = cn - 1;
  tsa = ((w <= 176) || (h <= 160)) ? 1 : (((w <= 240) || (h <= 240)) ? 2 : 3); // text size A
  tsb = ((w <= 272) || (h <= 220)) ? 1 : 2;                                    // text size B
  tsc = ((w <= 220) || (h <= 220)) ? 1 : 2;                                    // text size C
  ds = (w <= 160) ? 9 : 12;                                                    // digit size

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif
}

static inline uint32_t micros_start() __attribute__((always_inline));
static inline uint32_t micros_start()
{
  uint8_t oms = millis();
  while ((uint8_t)millis() == oms)
    ;
  return micros();
}

void loop(void)
{
  Serial.println(F("Benchmark               micro-secs"));

  int32_t usecFillScreen = testFillScreen();
  serialOut(F("Screen fill           : "), usecFillScreen, 1000, true);

  int32_t usecText = testText();
  serialOut(F("Text                  : "), usecText, 3000, true);

  int32_t usecPixels = testPixels();
  serialOut(F("Pixels                : "), usecPixels, 1000, true);

  int32_t usecLines = testLines();
  serialOut(F("Lines                 : "), usecLines, 1000, true);

  int32_t usecFastLines = testFastLines();
  serialOut(F("Horiz/Vert Lines      : "), usecFastLines, 1000, true);

  int32_t usecFilledRects = testFilledRects();
  serialOut(F("Filled Rectangles     : "), usecFilledRects, 1000, false);

  int32_t usecRects = testRects();
  serialOut(F("Outline Rectangles    : "), usecRects, 1000, true);

  int32_t usecFilledTrangles = testFilledTriangles();
  serialOut(F("Triangles (filled)    : "), usecFilledTrangles, 1000, false);

  int32_t usecTriangles = testTriangles();
  serialOut(F("Triangles (outline)   : "), usecTriangles, 1000, true);

  int32_t usecFilledCircles = testFilledCircles(10);
  serialOut(F("Circles (filled)      : "), usecFilledCircles, 1000, false);

  int32_t usecCircles = testCircles(10);
  serialOut(F("Circles (outline)     : "), usecCircles, 1000, true);

  int32_t usecFilledArcs = testFillArcs();
  serialOut(F("Arcs (filled)         : "), usecFilledArcs, 1000, false);

  int32_t usecArcs = testArcs();
  serialOut(F("Arcs (outline)        : "), usecArcs, 1000, true);

  int32_t usecFilledRoundRects = testFilledRoundRects();
  serialOut(F("Rounded rects (filled): "), usecFilledRoundRects, 1000, false);

  int32_t usecRoundRects = testRoundRects();
  serialOut(F("Rounded rects (outline): "), usecRoundRects, 1000, true);

#ifdef CANVAS
  uint32_t start = micros_start();
  gfx->flush();
  int32_t usecFlush = micros() - start;
  serialOut(F("flush (Canvas only)    : "), usecFlush, 0, false);
#endif

  Serial.println(F("Done!"));

  uint16_t c = 4;
  int8_t d = 1;
  for (int32_t i = 0; i < h; i++)
  {
    gfx->drawFastHLine(0, i, w, c);
    c += d;
    if (c <= 4 || c >= 11)
    {
      d = -d;
    }
  }
  gfx->setCursor(TEXT_X, TEXT_Y);

  gfx->setTextSize(tsa);
  gfx->setTextColor(MAGENTA);
  gfx->println(F("Arduino GFX PDQ")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  if (h > w)
  {
    gfx->setTextSize(tsb);
    gfx->setTextColor(GREEN);
    gfx->print(F("\nBenchmark ")); 
    gfx->setTextSize(tsc);
    if (ds == 12)
    {
      gfx->print(F("   "));
    }
    gfx->println(F("micro-secs")); gfx->setCursor(TEXT_X, gfx->getCursorY());
  }

  printnice(F("Screen fill "), usecFillScreen); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("Text        "), usecText); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("Pixels      "), usecPixels); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("Lines       "), usecLines); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("H/V Lines   "), usecFastLines); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("Rectangles F"), usecFilledRects); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("Rectangles  "), usecRects); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("Triangles F "), usecFilledTrangles); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("Triangles   "), usecTriangles); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("Circles F   "), usecFilledCircles); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("Circles     "), usecCircles); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("Arcs F      "), usecFilledArcs); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("Arcs        "), usecArcs); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("RoundRects F"), usecFilledRoundRects); gfx->setCursor(TEXT_X, gfx->getCursorY());
  printnice(F("RoundRects  "), usecRoundRects); gfx->setCursor(TEXT_X, gfx->getCursorY());

  if ((h > w) || (h > 240))
  {
    gfx->setTextSize(tsc);
    gfx->setTextColor(GREEN);
    gfx->print(F("\n"));
    gfx->setCursor(TEXT_X, gfx->getCursorY());
    gfx->print(F("Benchmark Complete!"));
  }

#ifdef CANVAS
  gfx->flush();
#endif

  delay(30 * 1000L);
}

#ifdef ESP32
void serialOut(const char *item, int32_t v, uint32_t d, bool clear)
#else
void serialOut(const __FlashStringHelper *item, int32_t v, uint32_t d, bool clear)
#endif
{
#ifdef CANVAS
  gfx->flush();
#endif
  Serial.print(item);
  if (v < 0)
  {
    Serial.println(F("N/A"));
  }
  else
  {
    Serial.println(v);
  }
  delay(d);
  if (clear)
  {
    gfx->fillScreen(BLACK);
  }
}

#ifdef ESP32
void printnice(const char *item, long int v)
#else
void printnice(const __FlashStringHelper *item, long int v)
#endif
{
  gfx->setTextSize(tsb);
  gfx->setTextColor(CYAN);
  gfx->print(item);

  gfx->setTextSize(tsc);
  gfx->setTextColor(YELLOW);
  if (v < 0)
  {
    gfx->println(F("      N / A"));
  }
  else
  {
    char str[32] = {0};
#ifdef RTL8722DM
    sprintf(str, "%d", (int)v);
#else
    sprintf(str, "%ld", v);
#endif
    for (char *p = (str + strlen(str)) - 3; p > str; p -= 3)
    {
      memmove(p + 1, p, strlen(p) + 1);
      *p = ',';
    }
    while (strlen(str) < ds)
    {
      memmove(str + 1, str, strlen(str) + 1);
      *str = ' ';
    }
    gfx->println(str);
  }
}

int32_t testFillScreen()
{
  uint32_t start = micros_start();
  // Shortened this tedious test!
  gfx->fillScreen(WHITE);
  gfx->fillScreen(RED);
  gfx->fillScreen(GREEN);
  gfx->fillScreen(BLUE);
  gfx->fillScreen(BLACK);

  return micros() - start;
}

int32_t testText()
{
  uint32_t start = micros_start();
  gfx->setCursor(TEXT_X, TEXT_Y);

  gfx->setTextSize(1);
  gfx->setTextColor(WHITE, BLACK);
  gfx->println(F("Hello World!")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(2);
  gfx->setTextColor(gfx->color565(0xff, 0x00, 0x00));
  gfx->print(F("RED "));
  gfx->setTextColor(gfx->color565(0x00, 0xff, 0x00));
  gfx->print(F("GREEN "));
  gfx->setTextColor(gfx->color565(0x00, 0x00, 0xff));
  gfx->println(F("BLUE")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(tsa);
  gfx->setTextColor(YELLOW);
  gfx->println(1234.56); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextColor(WHITE);
  gfx->println((w > 128) ? 0xDEADBEEF : 0xDEADBEE, HEX); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextColor(CYAN, WHITE);
  gfx->println(F("Groop,")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(tsc);
  gfx->setTextColor(MAGENTA, WHITE);
  gfx->println(F("I implore thee,")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(1);
  gfx->setTextColor(NAVY, WHITE);
  gfx->println(F("my foonting turlingdromes.")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextColor(DARKGREEN, WHITE);
  gfx->println(F("And hooptiously drangle me")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextColor(DARKCYAN, WHITE);
  gfx->println(F("with crinkly bindlewurdles,")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextColor(MAROON, WHITE);
  gfx->println(F("Or I will rend thee")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextColor(PURPLE, WHITE);
  gfx->println(F("in the gobberwartsb")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextColor(OLIVE, WHITE);
  gfx->println(F("with my blurglecruncheon,")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextColor(DARKGREY, WHITE);
  gfx->println(F("see if I don't!")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(2);
  gfx->setTextColor(RED);
  gfx->println(F("Size 2")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(3);
  gfx->setTextColor(ORANGE);
  gfx->println(F("Size 3")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(4);
  gfx->setTextColor(YELLOW);
  gfx->println(F("Size 4")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(5);
  gfx->setTextColor(GREENYELLOW);
  gfx->println(F("Size 5")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(6);
  gfx->setTextColor(GREEN);
  gfx->println(F("Size 6")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(7);
  gfx->setTextColor(BLUE);
  gfx->println(F("Size 7")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(8);
  gfx->setTextColor(PURPLE);
  gfx->println(F("Size 8")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  gfx->setTextSize(9);
  gfx->setTextColor(PALERED);
  gfx->println(F("Size 9")); gfx->setCursor(TEXT_X, gfx->getCursorY());

  return micros() - start;
}

int32_t testPixels()
{
  uint32_t start = micros_start();

  for (int16_t y = 0; y < h; y++)
  {
    for (int16_t x = 0; x < w; x++)
    {
      gfx->drawPixel(x, y, gfx->color565(x << 3, y << 3, x * y));
    }
#ifdef ESP8266
    yield(); // avoid long run triggered ESP8266 WDT restart
#endif
  }

  return micros() - start;
}

int32_t testLines()
{
  uint32_t start;
  int32_t x1, y1, x2, y2;

  start = micros_start();

  x1 = y1 = 0;
  y2 = h - 1;
  for (x2 = 0; x2 < w; x2 += 6)
  {
    gfx->drawLine(x1, y1, x2, y2, BLUE);
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6)
  {
    gfx->drawLine(x1, y1, x2, y2, BLUE);
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x1 = w - 1;
  y1 = 0;
  y2 = h - 1;
  for (x2 = 0; x2 < w; x2 += 6)
  {
    gfx->drawLine(x1, y1, x2, y2, BLUE);
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6)
  {
    gfx->drawLine(x1, y1, x2, y2, BLUE);
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x1 = 0;
  y1 = h - 1;
  y2 = 0;
  for (x2 = 0; x2 < w; x2 += 6)
  {
    gfx->drawLine(x1, y1, x2, y2, BLUE);
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6)
  {
    gfx->drawLine(x1, y1, x2, y2, BLUE);
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x1 = w - 1;
  y1 = h - 1;
  y2 = 0;
  for (x2 = 0; x2 < w; x2 += 6)
  {
    gfx->drawLine(x1, y1, x2, y2, BLUE);
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6)
  {
    gfx->drawLine(x1, y1, x2, y2, BLUE);
  }
#ifdef ESP8266
  yield(); // avoid long run triggered ESP8266 WDT restart
#endif

  return micros() - start;
}

int32_t testFastLines()
{
  uint32_t start;
  int32_t x, y;

  start = micros_start();

  for (y = 0; y < h; y += 5)
  {
    gfx->drawFastHLine(0, y, w, RED);
  }
  for (x = 0; x < w; x += 5)
  {
    gfx->drawFastVLine(x, 0, h, BLUE);
  }

  return micros() - start;
}

int32_t testFilledRects()
{
  uint32_t start;
  int32_t i, i2;

  start = micros_start();

  for (i = n; i > 0; i -= 6)
  {
    i2 = i / 2;

    gfx->fillRect(cx - i2, cy - i2, i, i, gfx->color565(i, i, 0));
  }

  return micros() - start;
}

int32_t testRects()
{
  uint32_t start;
  int32_t i, i2;

  start = micros_start();
  for (i = 2; i < n; i += 6)
  {
    i2 = i / 2;
    gfx->drawRect(cx - i2, cy - i2, i, i, GREEN);
  }

  return micros() - start;
}

int32_t testFilledCircles(uint8_t radius)
{
  uint32_t start;
  int32_t x, y, r2 = radius * 2;

  start = micros_start();

  for (x = radius; x < w; x += r2)
  {
    for (y = radius; y < h; y += r2)
    {
      gfx->fillCircle(x, y, radius, MAGENTA);
    }
  }

  return micros() - start;
}

int32_t testCircles(uint8_t radius)
{
  uint32_t start;
  int32_t x, y, r2 = radius * 2;
  int32_t w1 = w + radius;
  int32_t h1 = h + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros_start();

  for (x = 0; x < w1; x += r2)
  {
    for (y = 0; y < h1; y += r2)
    {
      gfx->drawCircle(x, y, radius, WHITE);
    }
  }

  return micros() - start;
}

int32_t testFillArcs()
{
  int16_t i, r = 360 / cn;
  uint32_t start = micros_start();

  for (i = 6; i < cn; i += 6)
  {
    gfx->fillArc(cx1, cy1, i, i - 3, 0, i * r, RED);
  }

  return micros() - start;
}

int32_t testArcs()
{
  int16_t i, r = 360 / cn;
  uint32_t start = micros_start();

  for (i = 6; i < cn; i += 6)
  {
    gfx->drawArc(cx1, cy1, i, i - 3, 0, i * r, WHITE);
  }

  return micros() - start;
}

int32_t testFilledTriangles()
{
  uint32_t start;
  int32_t i;

  start = micros_start();

  for (i = cn1; i > 10; i -= 5)
  {
    gfx->fillTriangle(cx1, cy1 - i, cx1 - i, cy1 + i, cx1 + i, cy1 + i,
                      gfx->color565(0, i, i));
  }

  return micros() - start;
}

int32_t testTriangles()
{
  uint32_t start;
  int32_t i;

  start = micros_start();

  for (i = 0; i < cn; i += 5)
  {
    gfx->drawTriangle(
        cx1, cy1 - i,     // peak
        cx1 - i, cy1 + i, // bottom left
        cx1 + i, cy1 + i, // bottom right
        gfx->color565(0, 0, i));
  }

  return micros() - start;
}

int32_t testFilledRoundRects()
{
  uint32_t start;
  int32_t i, i2;

  start = micros_start();

  for (i = n1; i > 20; i -= 6)
  {
    i2 = i / 2;
    gfx->fillRoundRect(cx - i2, cy - i2, i, i, i / 8, gfx->color565(0, i, 0));
  }

  return micros() - start;
}

int32_t testRoundRects()
{
  uint32_t start;
  int32_t i, i2;

  start = micros_start();

  for (i = 20; i < n1; i += 6)
  {
    i2 = i / 2;
    gfx->drawRoundRect(cx - i2, cy - i2, i, i, i / 8, gfx->color565(i, 0, 0));
  }

  return micros() - start;
}

/***************************************************
  Original sketch text:

  This is an example sketch for the Adafruit 2.2" SPI display.
  This library works with the Adafruit 2.2" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/1480

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/