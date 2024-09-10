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
#define DELAY_TIME 100
#define TIMES_TO_SHOW 200
uint16_t times_to_cls = TIMES_TO_SHOW;

void setup(void) {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  while (!Serial)
    ;
  Serial.println("Arduino_GFX Hello World example");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif
  randomSeed(millis());
  gfx->setCursor(10, 10);
  gfx->setTextColor(RED);
  gfx->println("Hello World!");

  delay(DELAY_TIME);  // 5 seconds
}

void loop() {
  if (!(--times_to_cls)) {
    Serial.println("Clear screen and repeating");
    // delay(DELAY_TIME);
    gfx->fillScreen(BLACK);
    randomSeed(millis());
    times_to_cls = TIMES_TO_SHOW;
  }
  gfx->setCursor(random(gfx->width()), random(gfx->height()));
  gfx->setTextColor(random(0xffff), random(0xffff));
  gfx->setTextSize(random(6) /* x scale */, random(6) /* y scale */, random(2) /* pixel_margin */);
  gfx->println("Hello World!");

  delay(DELAY_TIME);  // 1 second
}
