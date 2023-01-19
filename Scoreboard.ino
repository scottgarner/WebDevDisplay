#include <OctoWS2811.h>

#include "src/fonts/HUNTER_font.h"
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

const int numPins = 1;
byte pinList[numPins] = {2};

const int ledsPerStrip = 512;
const int bytesPerLED = 3;

DMAMEM int displayMemory[ledsPerStrip * numPins * bytesPerLED / 4];
int drawingMemory[ledsPerStrip * numPins * bytesPerLED / 4];

//int pixelIndex = 0;

const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);

char text[23] = "HIGH ORDER HELLO WORLD";

void setup() {
  Serial.begin(115200);
  Serial.println("READY");

  leds.begin();
  leds.show();
}

int rows = 8;
int cols = ledsPerStrip / 8;

int offset = 0;

void loop() {

  for (byte i = 0; i < 22; i++) {
    char letter = text[i];

    char index = letter & 0x7F;
    if (index < ' ') {
      index = 0;
    } else {
      index -= ' ';
    }

    const unsigned char* letterData = &font[index][0];

    Serial.print("Letter: ");
    Serial.println(letter);

    Serial.print("Index: ");
    Serial.println(index * CHAR_WIDTH);

    Serial.println(font[33][0], HEX);

    for (byte x = 0; x < CHAR_WIDTH; x++) {
//          Serial.println(letterData[x], BIN); 

      for (byte y = 0; y < CHAR_HEIGHT; y++) {

         int pixelX = ((x + (i * CHAR_WIDTH)) + offset) % cols;
         int pixelIndex = (pixelX * rows) + ((pixelX%2==0)?y:7-y) ;

        if (letterData[x] & (1 << y)) {
          leds.setPixel(pixelIndex, 0x202020);
          Serial.print("1");
        } else {
          Serial.print("0");
          leds.setPixel(pixelIndex, 0x000000);
        }
      }
      Serial.println("");
    }

    Serial.print(letter);
  }
  Serial.println("!");

  //  for (int x = 0; x < cols; x++) {
  //    for (int y = 0; y < rows; y++) {
  //      int i = (x * rows) + y;
  //
  //      if (((x/2)+offset) % 2 == 0) {
  //        leds.setPixel(i,0x040000);
  //      } else {
  //        leds.setPixel(i,0x000004);
  //      }
  //    }
  //  }

  offset++;
  leds.show();
  delay(100);
}
