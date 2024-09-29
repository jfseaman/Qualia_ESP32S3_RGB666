// SPDX-FileCopyrightText: 2023 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

/*
GFX 1.4.7 there is no support for Arduino_ESP32RGBPanel in "GFX Library for Arduino"
for Boards from 3.0.0 on so 
be sure to revert to 2.0.17 of boards.
It will take a couple itterations through bootsel/port to get the coms stabalized.
*/
/*******************************************************************************
 * Start of Arduino_GFX setting
 ******************************************************************************/
#include <Arduino_GFX_Library.h>
#include <Adafruit_GFX.h>
#include <fonts/FreeMono24pt7b.h>
#include <fonts/FreeMono18pt7b.h>
#include <fonts/FreeMono12pt7b.h>
#include <fonts/FreeMono9pt7b.h>
#include <fonts/FreeMonoBold24pt7b.h>
#include <fonts/FreeMonoBold18pt7b.h>
#include <fonts/FreeMonoBold12pt7b.h>
#include <fonts/FreeMonoBold9pt7b.h>
#include <fonts/FreeSansBold12pt7b.h>
#include <fonts/FreeSans24pt7b.h>
#include <fonts/FreeSans18pt7b.h>
#include <fonts/FreeSans12pt7b.h>
#include <fonts/FreeSans9pt7b.h>
#include <fonts/FreeSansBold24pt7b.h>
#include <fonts/FreeSansBold18pt7b.h>
#include <fonts/FreeSansBold12pt7b.h>
#include <fonts/FreeSansBold9pt7b.h>
#include <fonts/FreeSerif24pt7b.h>
#include <fonts/FreeSerif18pt7b.h>
#include <fonts/FreeSerif12pt7b.h>
#include <fonts/FreeSerif9pt7b.h>
#include <fonts/FreeSerifBold24pt7b.h>
#include <fonts/FreeSerifBold18pt7b.h>
#include <fonts/FreeSerifBold12pt7b.h>
#include <fonts/FreeSerifBold9pt7b.h>

/* Definitions : COMPILER_VERSION, COMPILER_FLAGS
	Initialize these strings per platform
*/
#ifndef COMPILER_VERSION 
 #ifdef __GNUC__
 #define COMPILER_VERSION "GCC"__VERSION__
 #else
 #define COMPILER_VERSION "Please put compiler version here (e.g. gcc 4.1)"
 #endif
#endif

void generateColorWheel();
unsigned long testFillScreenOnce(int);
unsigned long testText(uint8_t, uint8_t);
unsigned long testLines(uint16_t);
unsigned long testFastLines(uint16_t, uint16_t);
unsigned long testFilledRects(uint16_t, uint16_t);
unsigned long testCircles(uint8_t, uint16_t);
unsigned long testTriangles();
unsigned long testFilledTriangles();
unsigned long testFillScreen();
unsigned long testRects(uint16_t);
unsigned long testFilledCircles(uint8_t, uint16_t);
unsigned long testRoundRects();
unsigned long testFilledRoundRects();
unsigned long testColorWheel();
int16_t testTextMakeX();

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
  // , 1, 30000000 /* This makes fast text drawing but slow for all block fills and jitter*/
  // , 1, 6000000L /* Seems to provide best performance, no jitter, drawing not as "nice" during draw */
  ,
  1, 12000000L /* Best looking triangle/circle/square/rounded square drawing */
);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  SCREEN_WIDTH /* width */, SCREEN_HEIGHT /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  expander, GFX_NOT_DEFINED /* RST */, hd40015c40_init_operations, sizeof(hd40015c40_init_operations));

/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

#define TEST_DELAY 500
#define FILL_WAIT 400
#define TEXT_X 200
#define TEXT_Y 150

uint16_t *colorWheel;
int width;
int height;
int half_width;
int half_height;
uint8_t rotation;

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
  Serial.println("Initialize gfx->");
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }
  if (gfx->enableRoundMode()) {
    Serial.println("RoundMode enabled.");
  }
  // Set global reused values once to not to make so many function calls
  width = gfx->width();
  height = gfx->height();
  half_width = width / 2;
  half_height = height / 2;
  gfx->fillScreen(BLACK);
  Serial.println("Initialized!");
  Serial.println("Generating colorwheel");
  colorWheel = (uint16_t *)ps_malloc(width * height * sizeof(uint16_t));
  if (colorWheel) {
    generateColorWheel();
  }
  Serial.println("Done.");
}

void runBenchmark(void) {
  Serial.println(F("Benchmark                Time (microseconds)"));
  delay(TEST_DELAY);
  Serial.print(F("Screen fill Once RED     "));
  Serial.println(testFillScreenOnce(RED));
  delay(TEST_DELAY * 10);

  Serial.print(F("Screen fill Once BLUE    "));
  Serial.println(testFillScreenOnce(BLUE));
  delay(TEST_DELAY * 10);

  Serial.print(F("Screen fill Once GREEN   "));
  Serial.println(testFillScreenOnce(GREEN));
  delay(TEST_DELAY * 10);


  Serial.print(F("Fill "));
  Serial.println(testFillScreen());
  delay(TEST_DELAY);

  gfx->setFont();
  Serial.print(F("Text default             "));
  Serial.println(testText(0, 3));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeMono24pt7b);
  Serial.print(F("Text Mono24              "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeMono18pt7b);
  Serial.print(F("Text Mono18              "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeMono12pt7b);
  Serial.print(F("Text Mono12              "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeMono9pt7b);
  Serial.print(F("Text Mono9               "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeMonoBold24pt7b);
  Serial.print(F("Text MonoBold24          "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeMonoBold18pt7b);
  Serial.print(F("Text MonoBold18          "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeMonoBold12pt7b);
  Serial.print(F("Text MonoBold12          "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeMonoBold9pt7b);
  Serial.print(F("Text MonoBold9           "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSans24pt7b);
  Serial.print(F("Text Sans24              "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSans18pt7b);
  Serial.print(F("Text Sans18              "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSans12pt7b);
  Serial.print(F("Text Sans12              "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSans9pt7b);
  Serial.print(F("Text Sans9               "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSansBold24pt7b);
  Serial.print(F("Text SansBold24          "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSansBold18pt7b);
  Serial.print(F("Text SansBold18          "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSansBold12pt7b);
  Serial.print(F("Text SansBold12          "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSansBold9pt7b);
  Serial.print(F("Text SansBold9           "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSerif24pt7b);
  Serial.print(F("Text Serif24             "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSerif18pt7b);
  Serial.print(F("Text Serif18             "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSerif12pt7b);
  Serial.print(F("Text Serif12             "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSerif9pt7b);
  Serial.print(F("Text Serif9              "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSerifBold24pt7b);
  Serial.print(F("Text SerifBold24         "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSerifBold18pt7b);
  Serial.print(F("Text SerifBold18         "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSerifBold12pt7b);
  Serial.print(F("Text SerifBold12         "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  gfx->setFont(&FreeSerifBold9pt7b);
  Serial.print(F("Text SerifBold9          "));
  Serial.println(testText(rotation, rotation));
  delay(TEST_DELAY * 4);

  Serial.print(F("Lines                    "));
  Serial.println(testLines(CYAN));
  delay(TEST_DELAY);

  Serial.print(F("Horiz/Vert Lines         "));
  Serial.println(testFastLines(RED, BLUE));
  delay(TEST_DELAY);

  Serial.print(F("Rectangles (outline)     "));
  Serial.println(testRects(GREEN));
  delay(TEST_DELAY);

  Serial.print(F("Rectangles (filled)      "));
  Serial.println(testFilledRects(YELLOW, MAGENTA));
  delay(TEST_DELAY);

  Serial.print(F("Circles (filled)         "));
  Serial.println(testFilledCircles(10, MAGENTA));
  delay(TEST_DELAY);

  Serial.print(F("Circles (outline)        "));
  Serial.println(testCircles(10, WHITE));
  delay(TEST_DELAY);

  Serial.print(F("Triangles (outline)      "));
  Serial.println(testTriangles());
  delay(TEST_DELAY);

  Serial.print(F("Triangles (filled)       "));
  Serial.println(testFilledTriangles());
  delay(TEST_DELAY);

  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(testRoundRects());
  delay(TEST_DELAY);

  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(testFilledRoundRects());
  delay(TEST_DELAY);

  Serial.print(F("Color Wheel              "));
  Serial.println(testColorWheel());
  delay(TEST_DELAY);


  Serial.println(F("Done!"));
}


void loop(void) {
  Serial.printf("%s\n", __FILE__);
  Serial.printf("SDK Version       : %s\n", ESP.getSdkVersion());
  Serial.print("Sketch MD5        : ");
  Serial.println(ESP.getSketchMD5());
  Serial.printf("ESP32 Chip model  : %s, Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());

#ifdef USB_VID
  Serial.printf("ESP32 VID, PID    : 0x%x, 0x%x\n", USB_VID, USB_PID);
#endif
#ifdef USB_MANUFACTURER
  Serial.printf("Manufacturer      : %s %s\n", USB_MANUFACTURER, USB_PRODUCT);
#endif
	Serial.printf("Compiler version  : %s\n",COMPILER_VERSION);
  for (rotation = 0; rotation < 4; rotation++) {
    runBenchmark();
    gfx->setRotation(rotation);
    delay(1000);
  }
}

unsigned long testFillScreenOnce(int color) {
  unsigned long start, t = 0;
  start = micros();
  gfx->fillScreen(color);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  return t;
}

unsigned long testFillScreen() {
  unsigned long start, t = 0;
  Serial.print(".");
  start = micros();
  gfx->fillScreen(BLACK);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(NAVY);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(DARKGREEN);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(DARKCYAN);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(MAROON);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(PURPLE);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(OLIVE);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(LIGHTGREY);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(DARKGREY);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(BLUE);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(GREEN);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(CYAN);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(RED);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(MAGENTA);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(YELLOW);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(WHITE);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(ORANGE);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(GREENYELLOW);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(".");
  start = micros();
  gfx->fillScreen(PALERED);
  t += micros() - start;
  yield();
  delay(FILL_WAIT);
  Serial.print(" ");
  return t;
}

unsigned long testText(uint8_t rotS, uint8_t rotE) {
  gfx->fillScreen(BLACK);
  uint8_t rot = rotS; // starting rotation position
  unsigned long start = micros();
  do {
    gfx->setRotation(rot);
    gfx->setCursor(TEXT_X, TEXT_Y);
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->setTextColor(WHITE);
    gfx->setTextSize(1);
    gfx->println("Hello World!");
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->setTextColor(YELLOW);
    gfx->setTextSize(2);
    gfx->println(1234.56);
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->setTextColor(RED);
    gfx->setTextSize(3);
    gfx->println(0xDEADBEEF, HEX);
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->println();
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->setTextColor(GREEN);
    gfx->setTextSize(5);
    gfx->println("Groop");
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->setTextSize(2);
    gfx->println("I implore thee,");
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->setTextSize(1);
    gfx->println("my foonting turlingdromes.");
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->println("And hooptiously drangle me");
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->println("with crinkly bindlewurdles,");
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->println("Or I will rend thee");
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->println("in the gobberwarts");
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->println("with my blurglecruncheon,");
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
    gfx->println("see if I don't!");
    gfx->setCursor(testTextMakeX(), gfx->getCursorY());
  } while (++rot <= rotE);
  unsigned long t = micros() - start;
  // delay(TEST_DELAY * 4);
  gfx->setRotation(rotation);  // put the rotation back
  return t;
}

int16_t testTextMakeX() {
  // Calculate the farthest visible pixels on this row
  int dy = gfx->getCursorY() - half_height;
  int dx = sqrt(sq(half_height) - sq(dy));  // sq() is the square function in Arduino

  return (half_width - dx) + 10;
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int x1, y1, x2, y2,
    w = width,
    h = height;

  gfx->fillScreen(BLACK);
  yield();

  x1 = y1 = 0;
  y2 = h - 1;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) gfx->drawLine(x1, y1, x2, y2, color);
  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) gfx->drawLine(x1, y1, x2, y2, color);
  t = micros() - start;  // fillScreen doesn't count against timing

  yield();
  gfx->fillScreen(BLACK);
  yield();

  x1 = w - 1;
  y1 = 0;
  y2 = h - 1;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) gfx->drawLine(x1, y1, x2, y2, color);
  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6) gfx->drawLine(x1, y1, x2, y2, color);
  t += micros() - start;

  yield();
  gfx->fillScreen(BLACK);
  yield();

  x1 = 0;
  y1 = h - 1;
  y2 = 0;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) gfx->drawLine(x1, y1, x2, y2, color);
  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) gfx->drawLine(x1, y1, x2, y2, color);
  t += micros() - start;

  yield();
  gfx->fillScreen(BLACK);
  yield();

  x1 = w - 1;
  y1 = h - 1;
  y2 = 0;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) gfx->drawLine(x1, y1, x2, y2, color);
  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6) gfx->drawLine(x1, y1, x2, y2, color);

  yield();
  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int x, y, w = width, h = height;

  gfx->fillScreen(BLACK);
  start = micros();
  for (y = 0; y < h; y += 5) gfx->drawFastHLine(0, y, w, color1);
  for (x = 0; x < w; x += 5) gfx->drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int n, i, i2,
    cx = width / 2,
    cy = height / 2;

  gfx->fillScreen(BLACK);
  n = min(width, height);
  start = micros();
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    gfx->drawRect(cx - i2, cy - i2, i, i, color);
  }

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int n, i, i2,
    cx = width / 2 - 1,
    cy = height / 2 - 1;

  gfx->fillScreen(BLACK);
  n = min(width, height);
  for (i = n; i > 0; i -= 6) {
    i2 = i / 2;
    start = micros();
    gfx->fillRect(cx - i2, cy - i2, i, i, color1);
    t += micros() - start;
    // Outlines are not included in timing results
    gfx->drawRect(cx - i2, cy - i2, i, i, color2);
    yield();
  }

  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = width, h = height, r2 = radius * 2;

  gfx->fillScreen(BLACK);
  start = micros();
  for (x = radius; x < w; x += r2) {
    for (y = radius; y < h; y += r2) {
      gfx->fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, r2 = radius * 2,
            w = width + radius,
            h = height + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for (x = 0; x < w; x += r2) {
    for (y = 0; y < h; y += r2) {
      gfx->drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testTriangles() {
  unsigned long start;
  int n, i, cx = width / 2 - 1,
            cy = height / 2 - 1;

  gfx->fillScreen(BLACK);
  n = min(cx, cy);
  start = micros();
  for (i = 0; i < n; i += 5) {
    gfx->drawTriangle(
      cx, cy - i,      // peak
      cx - i, cy + i,  // bottom left
      cx + i, cy + i,  // bottom right
      gfx->color565(i, i, i));
  }

  return micros() - start;
}

unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int i, cx = width / 2 - 1,
         cy = height / 2 - 1;

  gfx->fillScreen(BLACK);
  start = micros();
  for (i = min(cx, cy); i > 10; i -= 5) {
    start = micros();
    gfx->fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                      gfx->color565(0, i * 10, i * 10));
    t += micros() - start;
    gfx->drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                      gfx->color565(i * 10, i * 10, 0));
    yield();
  }

  return t;
}

unsigned long testRoundRects() {
  unsigned long start;
  int w, i, i2,
    cx = width / 2 - 1,
    cy = height / 2 - 1;

  gfx->fillScreen(BLACK);
  w = min(width, height);
  start = micros();
  for (i = 0; i < w; i += 6) {
    i2 = i / 2;
    gfx->drawRoundRect(cx - i2, cy - i2, i, i, i / 8, gfx->color565(i, 0, 0));
  }

  return micros() - start;
}

unsigned long testFilledRoundRects() {
  unsigned long start;
  int i, i2,
    cx = width / 2 - 1,
    cy = height / 2 - 1;

  gfx->fillScreen(BLACK);
  start = micros();
  for (i = min(width, height); i > 20; i -= 6) {
    i2 = i / 2;
    gfx->fillRoundRect(cx - i2, cy - i2, i, i, i / 8, gfx->color565(0, i, 0));
    yield();
  }

  return micros() - start;
}

unsigned long testColorWheel() {
  unsigned long start;
  gfx->fillScreen(BLACK);
  start = micros();
  gfx->draw16bitRGBBitmap(0, 0, colorWheel, width, height);
  return micros() - start;
}

// https://chat.openai.com/share/8edee522-7875-444f-9fea-ae93a8dfa4ec
void generateColorWheel() {
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
