// PixhawkのTeremetoryをM5 Stamp PicoのBluetooth Serialに転送するプログラム
// 2023/5/25
//
// ・特徴
// 通信状態によりLED(SK6812)の色が変わる

#include "Arduino.h"
#include <FastLED.h>
#include <BluetoothSerial.h>

uint64_t chipid;
char chipname[64];
BluetoothSerial SerialBT;

#define NUM_LEDS 1
#define DATA_PIN 27
CRGB leds[NUM_LEDS];


void setup() {
  Serial.begin(19200);
  FastLED.addLeds<SK6812, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical

  chipid = ESP.getEfuseMac();
  sprintf( chipname, "MAVBT_%04X", (uint16_t)(chipid >> 32));
  Serial.printf("Bluetooth: %s\n", chipname);
  SerialBT.begin(chipname);

  leds[0] = 0x7f007f;
  FastLED.show();
}

void loop() {
  bool isRead = false;

  while (Serial.available() > 0) {
      int c = Serial.read();
      SerialBT.write(c);
      if (c == 0xFD) {
        leds[0] = 0x7f7f00;
        FastLED.show();
      } else if (isRead == false) {
        leds[0] = 0x007f00;
        FastLED.show();
      }
      isRead = true;
      yield();
  }

  isRead = false;
  while (SerialBT.available() > 0) {
      Serial.write(SerialBT.read());
      if (isRead == false) {
        isRead = true;
        leds[0] = 0x00007f;
        FastLED.show();
      }
      yield();
  }

  if (isRead == false) {
    leds[0] = 0x7f007f;
    FastLED.show();
  }
}
