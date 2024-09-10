#include <arduino_sprite.h>
#include <blitter.h>

/* 
The majority of the code is generated with Chat-GTP
It did a pretty good job but it's colorwheel form
is really slow to generate so Iused Adafruits
also generated but smaller/faster

GFX 1.4.7 there is no support for Arduino_ESP32RGBPanel in "GFX Library for Arduino"
for Boards from 3.0.0 on so 
be sure to revert to 2.0.17 of boards.
It will take a couple itterations through bootsel/port to get the coms stabalized.
*/

#include <Arduino_GFX_Library.h>
#include <Adafruit_GFX.h>
#include <Arduino_Sprite.h>

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

#define height SCREEN_HEIGHT

// Global variables for the center of the wheel
int half_width = height / 2;
int half_height = height / 2;

// Pointer to the global color wheel canvas
GFXcanvas16 *colorWheel = nullptr;

void generateColorWheel() {
  float angle;
  uint8_t r, g, b;
  int index, scaled_index;
  uint16_t* buffer = colorWheel->getBuffer();

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
          scaled_index = (y * 2 + dy) * SCREEN_WIDTH + (x * 2 + dx);
          buffer[scaled_index] = color;
        }
      }
    }
  }
}

void setup() {
  Serial.begin(115200);  // Start serial communication at 115200 baud rate
  while (!Serial) delay(100);
  delay(500);
  Serial.println("Initializing GFX object");
  gfx->begin();         // Initialize the display
  gfx->setRotation(1);  // Set display rotation if necessary
  Serial.println("Done.");

  // Allocate the colorWheel object at runtime
  Serial.println("Instantiate the canvas.");
  colorWheel = new GFXcanvas16(height, height);
    // Create a sprite for the ball
  Serial.println("Done.");
  Serial.println("Generate the colorWheel.");
  generateColorWheel();  // Generate the color wheel on the global colorWheel canvas
  Serial.println("Done.");
}

void loop() {
  gfx->fillScreen(BLACK);  // Clear the screen
  delay(500);
  unsigned long startTime = micros();  // Record the start time

  // Draw the colorWheel to the display
  gfx->draw16bitRGBBitmap(0, 0, colorWheel->getBuffer(), height, height);

  unsigned long endTime = micros();                   // Record the end time
  unsigned long executionTime = endTime - startTime;  // Calculate the execution time
  // Print the execution time to the Serial Monitor
  Serial.print("Execution Time: ");
  Serial.print(executionTime);
  Serial.println(" microseconds");

  delay(2500);  // Delay to make the output readable (adjust as necessary)
}